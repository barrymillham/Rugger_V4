#include "LampPost.h"

LampPost::LampPost()
{
	radius = 0;
	active = true;
	Identity(&world);
	Identity(&mTranslate);
	Identity(&mRotate);
	Identity(&mScale);
	width = 0;
	height = 0;
	depth = 0;
	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;
}

LampPost::~LampPost()
{
	box = NULL;
}

void LampPost::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP)
{
	if (!active) return;

									//scale,			rot,			trans
	Matrix transformation = transform(Vector3(3,0.5,3), Vector3(0,0,0), Vector3(0,0,0));
	drawWithWorld(mfxWVPVar, mfxWorldVar, mTech, mVP, transformation);

	transformation = transform(Vector3(0.5,10,0.5), Vector3(0,0,0), Vector3(0,0,0));
	drawWithWorld(mfxWVPVar, mfxWorldVar, mTech, mVP, transformation);
	
	transformation = transform(Vector3(3,0.5,0.5), Vector3(0,0,0), Vector3(2.5,20,0));
	drawWithWorld(mfxWVPVar, mfxWorldVar, mTech, mVP, transformation);

	transformation = transform(Vector3(1.5,0.5,1.5), Vector3(0,0,0), Vector3(4.5,19,0));
	drawWithWorld(mfxWVPVar, mfxWorldVar, mTech, mVP, transformation);

	transformation = transform(Vector3(1,0.08,1), Vector3(0,0,0), Vector3(4.5,18.7,0));
	drawWithWorld(mfxWVPVar, mfxWorldVar, mTech, mVP, transformation, true);

}

Matrix LampPost::transform(Vector3 scale, Vector3 rotate, Vector3 translate) {
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

void LampPost::drawWithWorld(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP, Matrix transformation, bool glow) {
	if (glow)
		mfxGlow->SetInt(2);
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
	if (glow) mfxGlow->SetInt(0);
}

void LampPost::init(Box *b, Vector3 pos, float r, float s, int w, int h, int d, float rx, float ry, float rz)
{
	box = b;
	radius = r;
	radius *= 1.01f; //fudge factor
	position = pos;
	scale = s;
	radiusSquared = radius * radius;
	width = w;
	height = h;
	depth = d;
	rotX = rx;
	rotY = ry;
	rotZ = rz;
	//Translate(&world, position.x, position.y, position.z);
}

void LampPost::update(float dt)
{

	Identity(&world);

	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixRotationY(&mRotate, rotY);
	world = world * mScale * mRotate * mTranslate;
	//D3DXMatrixMultiply(&world, &mScale, &mTranslate);

}