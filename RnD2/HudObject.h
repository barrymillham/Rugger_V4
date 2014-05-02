#ifndef __HUDOBJECT_H
#define __HUDOBJECT_H
#include "GameObject.h"

class HudObject : public GameObject
{
public:
	HudObject();
	~HudObject();

	virtual void update(float dt);

	float getWidth(){return width;}
	float getHeight(){return height;}
	float getDepth(){return depth;}

private:
	float radius;
	float radiusSquared;
};

#endif