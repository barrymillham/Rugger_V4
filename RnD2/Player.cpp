#include "Player.h"


Player::Player(void) : GameObject()
{
	health = 1000;
	ammo = 25;
	speed = 20;
}


Player::~Player(void)
{
	box = 0;
}

void Player::init(Box* bulletBox, vector<Bullet*>* bullets, Box* b, float r, Vector3 pos, Vector3 vel, float sp, Audio* a, float s, float w, float d, float h)
{ 
	gun = new Pistol(bulletBox, bullets);
	audio = a;
	box = b;
	radius = r;
	radius *= 1.01f;
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = radius * radius;
	width = s*w;
	height = s*h;
	depth = s*d;
	timeSinceLastShot = 0;
	score = 0;
	fired = false;
	health = 100;
	currentGun = 1;
}

void Player::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP)
{
	if (!active)
		return;
	Matrix mWVP = world* (*mVP);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&world);
    D3D10_TECHNIQUE_DESC techDesc;
    mTech->GetDesc( &techDesc );
	D3DXMatrixScaling(&mScale, width, height, depth);
    for(UINT p = 0; p < techDesc.Passes; ++p)
    {
        mTech->GetPassByIndex( p )->Apply(0);
		gun->draw(mfxWVPVar, mTech, mVP);
    }
}

void Player::grunt() {
	int num = rand()%7;

	switch (num) {
	case(0):
		audio->playCue(GRUNT1);
		break;
	case (1):
		audio->playCue(GRUNT2);
		break;
	case (2):
		audio->playCue(GRUNT3);
		break;
	case (3):
		audio->playCue(GRUNT4);
		break;
	case(4):
		audio->playCue(GRUNT5);
		break;
	case (5):
		audio->playCue(GRUNT6);
		break;
	case(6):
		audio->playCue(GRUNT7);
		break;
	default:
		audio->playCue(GRUNT1);
	}
}

void Player::update(float dt, D3DXVECTOR3 axis, Box* bulletBox, vector<Bullet*>* bullets)
{
	if(velocity != D3DXVECTOR3(0, 0, 0)) D3DXVec3Normalize(&velocity, &velocity);
	velocity *= speed;
	position += velocity*dt;
	Identity(&world);
	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);

	if(currentGun == 1){
		setGun(new Pistol(bulletBox, bullets));
	}else if(currentGun == 2){
		setGun(new Shotgun(bulletBox, bullets));
	}else if (currentGun == 3){
		setGun(new MachineGun(bulletBox, bullets));
	}else {
		setGun(new MachineGun(bulletBox, bullets));
	}
		
	gun->update(dt);
	if(fired) shoot(axis);

	timeSinceLastShot+=dt;
	if (timeSinceLastShot >= 1000000.0f) timeSinceLastShot = 100; //failsafe.
}

void Player::shoot(D3DXVECTOR3 moveAxis)
{
	timeSinceLastShot = 0;
	fired = false;
	if(ammo > 0) audio->playCue(FIRE);
	else {
		audio->playCue(OUT_OF_AMMO);
		return; 
	}

	gun->shoot(position, moveAxis, timeSinceLastShot);
	ammo--;
}
