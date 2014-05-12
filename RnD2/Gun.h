#ifndef __GUN_H
#define __GUN_H

#include<iostream>
#include "constants.h"
#include "Bullet.h"
#include "Box.h"
#include <ctime>
#include "Audio.h"
#include <string>

using namespace std;


class Gun {
public:
	virtual void shoot(Vector3 startingPosition, Vector3 axis, double timeSinceLastShot){} //Need to override in children
	void update(float dt) {
		for (int i = 0; i < bullets->size(); i++) {
			//a bullet will be set to inactive after it has been in the air for three seconds.
			//if it's inactive, it will be removed from the vector of bullets.
			if (!bullets->at(i)->getActiveState()) { 
				bullets->erase(bullets->begin()+i); //is this causing a mem leak?
			}
		}
		for (int i = 0; i < bullets->size(); i++) {
			bullets->at(i)->update(dt);
		}
	}
	void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectTechnique* mTech, Matrix* mVP) {
		for (int i = 0; i < bullets->size(); i++) 
			if(bullets->at(i)->getActiveState())
				bullets->at(i)->draw(mfxWVPVar, mTech, mVP);
	}
	float getShotDelay() {return shotDelay;}
	string getName() {return name;}
	vector<Bullet*>* bullets;

protected:
	float shotDelay;
	float damage;
	Box* bulletBox;
	string name;
};


class Pistol : public Gun {
public:
	Pistol(Box* b, vector<Bullet*>* theBullets) { 
		bulletBox = b;
		bullets = theBullets;
		damage = 20;
		shotDelay = 0.5f;
		name = "Pistol";
	}
	void shoot(Vector3 startingPosition, Vector3 axis, double timeSinceLastShot) {
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		int index = bullets->size()-1;
		bullets->at(index)->setDamage(damage);
		bullets->at(index)->setPosition(startingPosition);
		bullets->at(index)->setSpeed(bulletNS::SPEED);
		bullets->at(index)->setVelocity(axis);
		bullets->at(index)->setActive();
	}
};


class Shotgun : public Gun {
public:
	Shotgun(Box* b, vector<Bullet*>* theBullets) {
		srand(time(0));
		bulletBox = b;
		bullets = theBullets;
		damage = 4;
		shotDelay = 1.0f;
		name = "Shotgun";
	}

	float randOffset(float shotGunSprayConstant = 0.2) {
		float constant = shotGunSprayConstant; //increase for wider bullet spread
		float negate = (rand()%2 == 1)? -1.0f : 1.0f;
		return ((rand()%600)/100.0f+0.8f)*constant*negate;
	}

	void shoot(Vector3 startingPosition, Vector3 axis, double timeSinceLastShot) {		
		bool temptress = true; //VERY IMPORTANT VARIABLE DO NOT TOUCH
		
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));

		int bullet1 = bullets->size()-1;
		int bullet2 = bullets->size()-2;
		int bullet3 = bullets->size()-3;
		int bullet4 = bullets->size()-4;
		int bullet5 = bullets->size()-5;
		int bullet6 = bullets->size()-6;
		int bullet7 = bullets->size()-7;
		int bullet8 = bullets->size()-8;

		bullets->at(bullet1)->setDamage(damage);
		bullets->at(bullet1)->setPosition(startingPosition);
		bullets->at(bullet1)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet1)->setActive();

		bullets->at(bullet2)->setDamage(damage);
		bullets->at(bullet2)->setPosition(startingPosition);
		bullets->at(bullet2)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet2)->setActive();

		bullets->at(bullet3)->setDamage(damage);
		bullets->at(bullet3)->setPosition(startingPosition);
		bullets->at(bullet3)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet3)->setActive();

		bullets->at(bullet4)->setDamage(damage);
		bullets->at(bullet4)->setPosition(startingPosition);
		bullets->at(bullet4)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet4)->setActive();

		bullets->at(bullet5)->setDamage(damage);
		bullets->at(bullet5)->setPosition(startingPosition);
		bullets->at(bullet5)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet5)->setActive();

		bullets->at(bullet6)->setDamage(damage);
		bullets->at(bullet6)->setPosition(startingPosition);
		bullets->at(bullet6)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet6)->setActive();

		bullets->at(bullet7)->setDamage(damage);
		bullets->at(bullet7)->setPosition(startingPosition);
		bullets->at(bullet7)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet7)->setActive();

		bullets->at(bullet8)->setDamage(damage);
		bullets->at(bullet8)->setPosition(startingPosition);
		bullets->at(bullet8)->setSpeed(bulletNS::SPEED);
		bullets->at(bullet8)->setActive();

		//Give them different axis to travel
		bullets->at(bullet1)->setVelocity(axis);
		bullets->at(bullet2)->setVelocity(Vector3(axis.x + randOffset(), axis.y + randOffset(), axis.z + randOffset()));
		bullets->at(bullet3)->setVelocity(Vector3(axis.x + randOffset(), axis.y + randOffset(), axis.z + randOffset()));
		bullets->at(bullet4)->setVelocity(Vector3(axis.x + randOffset(), axis.y + randOffset(), axis.z + randOffset()));
		bullets->at(bullet5)->setVelocity(Vector3(axis.x + randOffset(), axis.y + randOffset(), axis.z + randOffset()));
		bullets->at(bullet6)->setVelocity(Vector3(axis.x + randOffset(0.1f), axis.y + randOffset(0.1f), axis.z + randOffset(0.1f)));
		bullets->at(bullet7)->setVelocity(Vector3(axis.x + randOffset(0.1f), axis.y + randOffset(0.1f), axis.z + randOffset(0.1f)));
		bullets->at(bullet8)->setVelocity(Vector3(axis.x + randOffset(0.1f), axis.y + randOffset(0.1f), axis.z + randOffset(0.1f)));

	}
};


class MachineGun : public Gun {
public:
	MachineGun(Box* b, vector<Bullet*>* theBullets) { 
		bulletBox = b;
		bullets = theBullets;
		damage = 10;
		shotDelay = 0.1f;
		name = "Machine Gun";
	}
	void shoot(Vector3 startingPosition, Vector3 axis, double timeSinceLastShot) {
		bullets->push_back(new Bullet(bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1));
		int index = bullets->size()-1;
		bullets->at(index)->setDamage(damage);
		bullets->at(index)->setPosition(startingPosition);
		bullets->at(index)->setSpeed(bulletNS::SPEED);
		bullets->at(index)->setVelocity(axis);
		bullets->at(index)->setActive();
	}
};







#endif
