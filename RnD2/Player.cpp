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

void Player::update(float dt, D3DXVECTOR3 moveAxis, bool shotgun)
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
		shoot(moveAxis, shotgun);

	timeSinceLastShot+=dt;
}

void Player::shoot(D3DXVECTOR3 moveAxis, bool shotgun)
{
	if(ammo == 0) return;
	int index[5] = {-1,-1,-1,-1,-1};
	int counter = 0;
	D3DXVECTOR3 moveAxis1;
	D3DXVECTOR3 moveAxis2;
	D3DXVECTOR3 moveAxis3;
	D3DXVECTOR3 moveAxis4;
	D3DXMATRIX trans[4];
	double offset[4];

	for(int i=0; i < 4; i++){
		offset[i] = .1;
	}

	D3DXMatrixTranslation(&trans[0], 1.0+offset[0], 1.0+offset[0], 1.0+offset[0]);
	D3DXMatrixTranslation(&trans[1], 1.0+offset[1], 1.0+offset[1], 1.0+offset[1]);
	D3DXMatrixTranslation(&trans[2], 1.0+offset[2], 1.0+offset[2], 1.0+offset[2]);
	D3DXMatrixTranslation(&trans[3], 1.0+offset[3], 1.0+offset[3], 1.0+offset[3]);
	Transform(&moveAxis1, &moveAxis, &trans[0]);
	Transform(&moveAxis2, &moveAxis, &trans[1]);
	Transform(&moveAxis3, &moveAxis, &trans[2]);
	Transform(&moveAxis4, &moveAxis, &trans[3]);

	D3DXVec3Normalize(&moveAxis, &moveAxis);
	D3DXVec3Normalize(&moveAxis1, &moveAxis1);
	D3DXVec3Normalize(&moveAxis2, &moveAxis2);
	D3DXVec3Normalize(&moveAxis3, &moveAxis3);
	D3DXVec3Normalize(&moveAxis4, &moveAxis4);

	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i]->getActiveState() && counter <= 4) { 
			index[counter] = i;
			counter++;
		}
	}

	if(timeSinceLastShot > 3){
		for(int i = 0; i < bullets.size(); i++){
			bullets[i]->setInActive();
		}
		timeSinceLastShot = 0;
	}
	if(shotgun){
		if((index[0] == -1 || index[1] == -1 || index[2] == -1 || index[3] == -1 || index[4] == -1)) return;
	}
	else{
		if (index[0] == -1) return;
	}

	bullets[index[0]]->setPosition(position);
	bullets[index[0]]->setSpeed(bulletNS::SPEED);
	bullets[index[0]]->setVelocity(moveAxis);
	bullets[index[0]]->setActive();

	if(shotgun){
		bullets[index[1]]->setPosition(position);
		bullets[index[1]]->setSpeed(bulletNS::SPEED);
		bullets[index[1]]->setVelocity(moveAxis1);
		bullets[index[1]]->setActive();

		bullets[index[2]]->setPosition(position);
		bullets[index[2]]->setSpeed(bulletNS::SPEED);
		bullets[index[2]]->setVelocity(moveAxis2);
		bullets[index[2]]->setActive();

		bullets[index[3]]->setPosition(position);
		bullets[index[3]]->setSpeed(bulletNS::SPEED);
		bullets[index[3]]->setVelocity(moveAxis3);
		bullets[index[3]]->setActive();

		bullets[index[4]]->setPosition(position);
		bullets[index[4]]->setSpeed(bulletNS::SPEED);
		bullets[index[4]]->setVelocity(moveAxis4);
		bullets[index[4]]->setActive();
	}
	
	//ammo--;
	timeSinceLastShot = 0;
	fired = false;
}
