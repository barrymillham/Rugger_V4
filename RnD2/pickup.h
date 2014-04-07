#ifndef PICKUP_H
#define PICKUP_H
#include "GameObject.h"
#include "Audio.h"
#include "constants.h"

class Pickup : public GameObject
{
public:
	Pickup(){}
	Pickup(Box *b, int* value, bool MOD, int amount, const char* sound, Audio* a);
	~Pickup();

	//Width and height in integral number of boxes(bricks)
	virtual void init(Vector3 pos);
	virtual void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	virtual void update(float dt);
	void activate();
	//void setPosition (Vector3 pos) {position = pos;}
	//Vector3 getPosition() {return position;}
	//void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	//float getRadiusSquare() {return radiusSquared;}
	//float getRadius() {return radius;}
	//Matrix getWorldMatrix() {return world;}
	//void setWorld(Matrix w){world = w;}
	//void setScale(float s) {scale = s; radiusSquared = (s*radius)*(s*radius);}
	//float getScale() {return scale;}
	//void setActive() {active = true;}
	//void setInActive() {active = false;}
	//bool getActiveState() {return active;}
	//bool collided(GameObject *gameObject);

	float getWidth(){
		return width;
	}
	float getHeight(){
		return height;
	}
	float getDepth(){
		return depth;
	}

private:
	float radius;
	float radiusSquared;
	int* value;
	Audio* audio;
	bool mod;
	int amount;
	char* SOUND;
};

#endif