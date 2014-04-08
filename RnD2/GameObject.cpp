
#include "GameObject.h"

GameObject::GameObject()
{
	radius = 0;
	speed = 0;
	active = true;
	Identity(&world);
	Identity(&mTranslate);
	Identity(&mRotate);
	Identity(&mScale);
	Identity(&transformation);
	facing = false;
	facedObject = NULL;
	facedCoordinate = Vector3(0,0,0);
	glow = false;
}

GameObject::~GameObject()
{
	box = NULL;
}

void GameObject::faceObject(GameObject *o) {
	facing = true;
	facedObject = o;
}

void GameObject::faceObject(Vector3 coordinate) {
	facing = true;
	facedCoordinate = coordinate;
}

void GameObject::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP)
{

	if (!active) return;	
	if (facing) {
		if (facedObject != NULL){
			Vector3 direction = facedObject->getPosition() - position;
			rotY = -atanf(direction.z/direction.x);
		} else {
			Vector3 direction = facedCoordinate - position;
			rotY = -atanf(direction.z/direction.x);
		}
	}

	transformation = transform(Vector3(1,1,1), Vector3(0,rotY,0), Vector3(0,0,0));
	drawWithWorld(mfxWVPVar, mfxWorldVar, mTech, mVP, transformation);
}

void GameObject::drawWithWorld(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP, Matrix transformation) {
	if (glow) {
		mfxGlow->SetInt(2);
	}
	else mfxGlow->SetInt(0);
	Matrix worldMatrix = GameObject::world;
	worldMatrix=transformation*worldMatrix;
	Matrix mWVP = worldMatrix* (*mVP);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&worldMatrix);
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex( p )->Apply(0);
		box->draw();
	}

	if (glow)
		mfxGlow->SetInt(0);
}


Matrix GameObject::transform(Vector3 scale, Vector3 rotate, Vector3 translate) {
	Matrix transformation = Matrix();
	Identity(&transformation);

	Matrix scaley;
	Matrix rotx;
	Matrix roty;
	Matrix rotz;
	Matrix rot;
	Matrix transy;
	Identity(&scaley);
	Identity(&rotx);
	Identity(&roty);
	Identity(&rotz);
	Identity(&rot);
	Identity(&transy);

	//scale rotate translate
	D3DXMatrixScaling(&scaley, scale.x, scale.y, scale.z);
	D3DXMatrixRotationX(&rotx, rotate.x);
	D3DXMatrixRotationY(&roty, rotate.y);
	D3DXMatrixRotationZ(&rotz, rotate.z);
	D3DXMatrixTranslation(&transy, translate.x, translate.y, translate.z);
	rot = rotx*roty*rotz;

	transformation *= scaley;
	transformation *= rot;
	transformation *= transy;
	return transformation;
}

void GameObject::init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s, float w, float h, float d)
{
	box = b;
	radius = r;
	radius *= 1.01; //fudge factor
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = radius * radius;
	width = w*s;
	depth = d*s;
	height = h*s;
	D3DXVECTOR3 temp = box->getColor();
	mfxCubeColorVar	= box->getMFX()->GetVariableByName("gCubeColor");
	mfxGlow			= box->getMFX()->GetVariableByName("gGlow")->AsScalar();
	mfxCubeColorVar->SetRawValue(&box->getColor(), 0, sizeof(D3DXVECTOR3));
}

void GameObject::update(float dt)
{
	position += velocity*dt;
	Identity(&world);
	Scale(&mScale, width, height, depth);
	Translate(&mTranslate, position.x, position.y, position.z);
	world = mScale * mTranslate;

}

//Note that this collision only works for axis-aligned cubes
bool GameObject::collided(GameObject *gameObject)
{
	//to get this collision working,
	//transform position vector by the rotY (The amount the cube is rotated around the y axis by)?

	if (!gameObject->getActiveState() || !active) return false;
	if( position.x - getWidth() <= gameObject->getPosition().x + gameObject->getWidth() && 
		position.x + getWidth() >= gameObject->getPosition().x - gameObject->getWidth() &&
		position.y - getHeight() <= gameObject->getPosition().y + gameObject->getHeight() &&
		position.y + getHeight() >= gameObject->getPosition().y - gameObject->getHeight() &&
		position.z - getDepth() <= gameObject->getPosition().z + gameObject->getDepth() &&
		position.z + getDepth() >= gameObject->getPosition().z - gameObject->getDepth()) 
			return true;
	else return false;
}