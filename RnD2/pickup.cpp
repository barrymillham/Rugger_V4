#include "pickup.h"

Pickup::Pickup(Box *b, int* value, bool MOD, int amount, int mapIndex, const char* sound, Audio* a, unsigned int level) {
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

	if(level == 1){
		mapLocations.push_back(Vector3(20,0,0));
		mapLocations.push_back(Vector3(-40,0,40));
		mapLocations.push_back(Vector3(40,0,40));
		mapLocations.push_back(Vector3(40,0,-40));
		mapLocations.push_back(Vector3(-40,0,-40));

		mapLocations.push_back(Vector3(0,0,80));
		mapLocations.push_back(Vector3(80,0,0));
		mapLocations.push_back(Vector3(0,0,-80));
		mapLocations.push_back(Vector3(-80,0,0));
	
		mapLocations.push_back(Vector3(-100,0,100));
		mapLocations.push_back(Vector3(100,0,100));
		mapLocations.push_back(Vector3(100,0,-100));
		mapLocations.push_back(Vector3(-100,0,-100));

		mapLocations.push_back(Vector3(-230,0,230));
		mapLocations.push_back(Vector3(230,0,230));
		mapLocations.push_back(Vector3(230,0,-230));
		mapLocations.push_back(Vector3(-230,0,-230));
	}
	else if(level == 2){
		mapLocations.push_back(Vector3(0,5,0));
		mapLocations.push_back(Vector3(0,10,0));
		mapLocations.push_back(Vector3(0,15,0));
		mapLocations.push_back(Vector3(0,20,0));

		mapLocations.push_back(Vector3(0,25,0));
		mapLocations.push_back(Vector3(0,30,0));
		mapLocations.push_back(Vector3(0,35,0));
		mapLocations.push_back(Vector3(0,40,0));

		mapLocations.push_back(Vector3(0,45,0));
		mapLocations.push_back(Vector3(0,50,0));
		mapLocations.push_back(Vector3(0,55,0));
		mapLocations.push_back(Vector3(0,60,0));

		mapLocations.push_back(Vector3(0,65,0));
		mapLocations.push_back(Vector3(0,70,0));
		mapLocations.push_back(Vector3(0,75,0));
		mapLocations.push_back(Vector3(0,80,0));

		mapLocations.push_back(Vector3(0,85,0));
		mapLocations.push_back(Vector3(0,90,0));
		mapLocations.push_back(Vector3(0,95,0));
		mapLocations.push_back(Vector3(0,100,0));

		mapLocations.push_back(Vector3(0,105,0));
		mapLocations.push_back(Vector3(0,110,0));
		mapLocations.push_back(Vector3(0,115,0));

	}
	position = mapLocations[mapIndex];
	Pickup::mapIndex = mapIndex;
	Pickup::value = value;
	audio = a;
	mod = MOD;
	Pickup::amount = amount;
	Pickup::SOUND = const_cast<char*>(sound);
	mfxGlow = box->getGlowVar();
	mfxCubeColorVar = box->getCubeColorVar();
	mfxCubeColorVar->SetRawValue(&box->getColor(), 0, sizeof(D3DXVECTOR3));
}

void Pickup::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP) {
	if (glow)
		mfxGlow->SetInt(2);
	else mfxGlow->SetInt(0);

	Matrix mWVP = GameObject::world * (*mVP);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)GameObject::world);
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex( p )->Apply(0);
		box->draw();
	}
	if (glow) mfxGlow->SetInt(0);
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

void Pickup::update(float dt)
{
	Identity(&world);
	
	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);
}