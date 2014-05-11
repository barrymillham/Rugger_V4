#pragma once

#include "gameobject.h"

namespace bulletNS {
	const int SPEED = 300;
	const float MASS = 5000;
}

class Bullet :	public GameObject
{
public:
	Bullet(Box* b, float r, Vector3 pos, Vector3 vel, float sp, float s);
	~Bullet(void);

	void init(Box* b, float r, Vector3 pos, Vector3 vel, float sp, float s);
	void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	void update(float dt);
	float getMass(){return mass;}
	void setActive() {
		active = true;
		timeShot = 0.0f;
	}
	void setInActive() {
		active = false;
		timeShot = 0.0f;
	}
	void setDamage(int d) {damage = d;}

private:
	float timeShot;
	float speed;
	float radius;
	float mass;
	int damage;
};

