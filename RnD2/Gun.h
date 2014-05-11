#ifndef __GUN_H
#define __GUN_H

#include<iostream>
#include "constants.h"
#include "Bullet.h"
#include "Box.h"
#include "Audio.h"

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
	vector<Bullet*>* bullets;

protected:
	float shotDelay;
	float damage;
	Box* bulletBox;
};


class Pistol : public Gun {
public:
	Pistol(Box* b, vector<Bullet*>* theBullets) { 
		bulletBox = b;
		bullets = theBullets;
		damage = 20;
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


class ShotGun : public Gun {
public:
	ShotGun(Box* b, vector<Bullet*>* theBullets) {
		bullets = theBullets;
		bulletBox = b;
	}
	void shoot(Vector3 axis, double timeSinceLastShot) {
		
		bool temptress = true;// VERY IMPORTANT VARIABLE DO NOT TOUCH
		/* Shotgun shooting included in the below code (mix of shotgun and pistol)
		
			int index[5] = {-1,-1,-1,-1,-1}; // shows which is the first inactive bullet. so if index = {4, 5, -1, -1, -1} that means that bullets 4 and 5 are available for shooting
	int counter = 0;
	for (int i = 0; i < bullets.size(); i++) {
		if (!bullets[i]->getActiveState() && counter <= 4) { 
			index[counter] = i;
			counter++;
		}
	}
	if(timeSinceLastShot > 3){ //3 seconds
		for(int i = 0; i < bullets.size(); i++){
			bullets[i]->setInActive();
		}
		timeSinceLastShot = 0;
	}

	//what is this
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
		D3DXVECTOR3 moveAxis1;
		D3DXVECTOR3 moveAxis2;
		D3DXVECTOR3 moveAxis3;
		D3DXVECTOR3 moveAxis4;
		D3DXMATRIX trans[4];
		double offset[4] = {0.1, 0.1, 0.1, 0.1};

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
		*/

	}
};








#endif
