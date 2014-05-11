#include "Bullet.h"


Bullet::Bullet(Box* b, float r, Vector3 pos, Vector3 vel, float sp, float s) {
	active = false;
	init(b, r, pos, vel, sp, s);
}


Bullet::~Bullet(void)
{
}

void Bullet::init(Box* b, float r, Vector3 pos, Vector3 vel, float sp, float s)
{
	box = b;
	radius = r;
	radius *= 1.01;
	position = pos;
	velocity = vel;
	speed = sp;
	scale = s;
	radiusSquared = radius * radius;
	width = height = depth = scale;
	mass = bulletNS::MASS;
	active = false;
}

void Bullet::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectTechnique* mTech, Matrix* mVP)
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

void Bullet::update(float dt)
{
	timeShot += dt;
	if (timeShot >= 3.0f) {
		setInActive();
	}
	Normalize(&velocity, &velocity);
	velocity *= bulletNS::SPEED;
	position += velocity*dt;
	Identity(&world);
	Translate(&world, position.x, position.y, position.z);

}