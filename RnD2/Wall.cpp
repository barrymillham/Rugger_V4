#include "Wall.h"

Wall::Wall()
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

Wall::~Wall()
{
	box = NULL;
}

void Wall::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP)
{
	if (!active)
		return;

	Matrix mWVP = world* (*mVP);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&world);
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex( p )->Apply(0);
		box->draw();
	}
}

void Wall::init(Box *b, float r, Vector3 pos, float s, int w, int h, int d, float rx, float ry, float rz)
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

void Wall::update(float dt)
{

	Identity(&world);

	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);

}