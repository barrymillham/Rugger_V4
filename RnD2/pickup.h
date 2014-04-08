#ifndef PICKUP_H
#define PICKUP_H
#include "GameObject.h"
#include "Audio.h"
#include "constants.h"
#include<vector>
using std::vector;

class Pickup : public GameObject
{
public:
	Pickup(){}
	Pickup(Box *b, int* value, bool MOD, int amount, int mapIndex, const char* sound, Audio* a);
	~Pickup();

	//Width and height in integral number of boxes(bricks)
	virtual void update(float dt);
	void activate();
	void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
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

	int getMapIndex() {return mapIndex;}
	D3DXVECTOR3 getColor(){return box->getColor();}

private:
	float radius;
	float radiusSquared;
	int* value;
	Audio* audio;
	bool mod;
	int amount;
	char* SOUND;
	int mapIndex;
	vector<Vector3> mapLocations;
	ID3D10EffectScalarVariable* mfxGlow;
};

#endif