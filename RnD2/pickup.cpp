#include "pickup.h"

Pickup::Pickup(Box *b, int* value, bool MOD, int amount, const char* sound, Audio* a) {
	radius = 1;
	radius *= 1.01f; //fudge factor
	active = true;
	Identity(&world);
	Identity(&mTranslate);
	Identity(&mRotate);
	Identity(&mScale);
	width = 1;
	height = 0.5;
	depth = 1;
	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;
	radiusSquared = radius * radius;
	box = b;


	Pickup::value = value;
	audio = a;
	mod = MOD;
	Pickup::amount = amount;
	Pickup::SOUND = const_cast<char*>(sound);
}

void Pickup::activate() {
	if (mod == INCREASE)
		(*value)+=amount;
	else if (mod == DECREASE) 
		(*value)-=amount;

	audio->playCue(SOUND);
	setInActive();
}

Pickup::~Pickup()
{
	box = NULL;
}

void Pickup::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectTechnique* mTech, Matrix* mVP)
{
	if (!active)
		return;

	Matrix mWVP = world* (*mVP);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex( p )->Apply(0);
		box->draw();
	}
}

void Pickup::init(Vector3 pos)
{
	position = pos;
}

void Pickup::update(float dt)
{
	Identity(&world);
	
	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);
}