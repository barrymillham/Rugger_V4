#pragma once
#include "gameobject.h"
#include "Bullet.h"
#include<vector>

using std::vector;

class Player :
	public GameObject
{
public:
	Player(void);
	~Player(void);

	//Player takes a pointer to a bullet which will be handled completely by the player class: update, drawing, and all
	void init(Box* b, vector<Bullet*> theBullets, float r, Vector3 pos, Vector3 vel, float sp, float s);
	void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	void update(float dt, D3DXVECTOR3 moveAxis);

	void shoot(D3DXVECTOR3 moveAxis);
	void rotateTargeting(int s);
	void charge();
	bool fired;
	bool firedLastFrame;

private:
	float speed;
	float radius;
	double timeSinceLastShot;
	vector<Bullet*> bullets;
};

