#include "pickup.h"

Pickup::Pickup(Box *b, int* value, int MOD, int amount, int mapIndex, const char* sound, Audio* a, unsigned int level, int gun_Type, int* gun) {
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
		mapLocations.push_back(Vector3(500,0,1250));
		mapLocations.push_back(Vector3(-450,0,1150));
		mapLocations.push_back(Vector3(475,0,700));
		mapLocations.push_back(Vector3(-750,0,125));

		mapLocations.push_back(Vector3(750,0,25));
		mapLocations.push_back(Vector3(-400,0,-50));
		mapLocations.push_back(Vector3(200,0,-700));
		mapLocations.push_back(Vector3(-200,0,-800));

		mapLocations.push_back(Vector3(75,0,700));
		mapLocations.push_back(Vector3(75,0,-1100));
		mapLocations.push_back(Vector3(900,0,950));
		mapLocations.push_back(Vector3(-75,0,1200));

		mapLocations.push_back(Vector3(-600,0,1250));
		mapLocations.push_back(Vector3(750,0,275));
		mapLocations.push_back(Vector3(-350,0,350));
		mapLocations.push_back(Vector3(-75,0,-300));

		mapLocations.push_back(Vector3(-900,0,-300));
		mapLocations.push_back(Vector3(850,0,-700));
		mapLocations.push_back(Vector3(-875,0,-1050));
		mapLocations.push_back(Vector3(950,0,1600));

		mapLocations.push_back(Vector3(-950,0,1600));
		mapLocations.push_back(Vector3(550,0,125));
		mapLocations.push_back(Vector3(-750,0,-150));

	}
	position = mapLocations[mapIndex];
	Pickup::mapIndex = mapIndex;
	Pickup::value = value;
	Pickup::gun = gun;
	audio = a;
	mod = MOD;
	gunT = gun_Type;
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
	if(gunT == 0) *gun = 0;
	else if(gunT == 1) *gun = 1;
	else if(gunT == 2) *gun = 2;


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