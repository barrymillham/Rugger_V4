#include "Player.h"


Player::Player(void) : GameObject()
{
	health = 100;
	ammo = 25;
	speed = 200;
}


Player::~Player(void)
{
	box = 0;
}

void Player::init(Box* b, vector<Bullet*> theBullets, float r, Vector3 pos, Vector3 vel, float sp, Audio* a, float s, float w, float d, float h)
{ 
	audio = a;
	box = b;
	Player::bullets = theBullets;
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
	health = 100;
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
        //box->draw();
		for (int i = 0; i < bullets.size(); i++) 
			if(bullets[i]->getActiveState())
				bullets[i]->draw(mfxWVPVar, mTech, mVP);
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

void Player::update(float dt, D3DXVECTOR3 moveAxis)
{
	D3DXVec3Normalize(&velocity, &velocity);
	velocity *= speed;
	position += velocity*dt;
	Identity(&world);
	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);
	
	for (int i = 0; i < bullets.size(); i++)
		bullets[i]->update(dt);
	
	if(fired == true)
		shoot(moveAxis);

	timeSinceLastShot+=dt;
}

void Player::shoot(D3DXVECTOR3 moveAxis)
{
	if(ammo == 0) return;
	int index = -1;
	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i]->getActiveState()) { 
			index = i;
			i = bullets.size();
		}
	}

	if(timeSinceLastShot > 3){
		for(int i = 0; i < bullets.size(); i++){
			bullets[i]->setInActive();
		}
		timeSinceLastShot = 0;
	}

	if (index == -1) return;

	bullets[index]->setPosition(position);
	bullets[index]->setSpeed(bulletNS::SPEED);
	bullets[index]->setVelocity(moveAxis);
	bullets[index]->setActive();
	ammo--;
	timeSinceLastShot = 0;
	fired = false;
}

void Player::rotateTargeting(int s)
{
//unneeded, we will use the camera vectors for targeting
}