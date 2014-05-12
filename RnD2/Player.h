#pragma once
#include "gameobject.h"
#include "Bullet.h"
#include<vector>
#include "Audio.h"
#include "constants.h"
#include "Gun.h"

using std::vector;

class Player :	public GameObject {
public:
	Player(void);
	~Player(void);

	//Player takes a pointer to a bullet which will be handled completely by the player class: update, drawing, and all
	void init(Box* bulletBox, vector<Bullet*>* bullets, Box* b, float r, Vector3 pos, Vector3 vel, float sp, Audio* a, float s = 1, float w = 1, float d = 1, float h = 1);
	void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	void update(float dt, D3DXVECTOR3 moveAxis, Box* bulletBox, vector<Bullet*>* bullets);

	void shoot(D3DXVECTOR3 moveAxis);
	void rotateTargeting(int s);
	bool fired;
	bool firedLastFrame;
	void damage(int d){health -= d;}
	int getHealth(){return health;}
	void setHealth(int h){health = h;}
	int getAmmo(){return ammo;}
	void setAmmo(int a){ammo = a;}
	void addAmmo(int a){ammo += a;}
	void addScore(int s) {score += s;}
	int getScore() {return score;}
	float getSpeed(){return static_cast<float>(speed);}
    void setSpeed(int s){speed = s;}
	void grunt();
	void setGun(Gun* g) {gun = g;}
	int speed;
	int health;
	int ammo;
	int score;
	int gunT;

	Gun* gun;

private:
	float radius;
	double timeSinceLastShot;
	
	Audio* audio;
};

