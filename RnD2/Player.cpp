#include "Player.h"


Player::Player(void) : GameObject()
{
	health = 100;
}


Player::~Player(void)
{
	box = 0;
}

void Player::init(Box* b, vector<Bullet*> theBullets, float r, Vector3 pos, Vector3 vel, float sp, float s, float w, float d, float h)
{ 
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

void Player::update(float dt, D3DXVECTOR3 moveAxis)
{
	position += velocity*dt;
	Identity(&world);
	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);
	//Translate(&world, position.x, position.y, position.z);
	for (int i = 0; i < bullets.size(); i++)
		bullets[i]->update(dt);
	
	if(fired == true)
	{
		shoot(moveAxis);
	}

	timeSinceLastShot+=dt;
}

void Player::charge() {
}

void Player::shoot(D3DXVECTOR3 moveAxis)
{
	//If the player's got an active bullet on the level, he doesn't get to shoot
	int index = -1;
	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i]->getActiveState()) { //If the bullet at index i is not in use
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
	timeSinceLastShot = 0;
	fired = false;
}

void Player::rotateTargeting(int s)
{
//unneeded, we will use the camera vectors for targeting
}