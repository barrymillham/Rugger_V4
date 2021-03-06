#ifndef WALL_H
#define WALL_H
#include "GameObject.h"

class Wall : public GameObject
{
public:
	Wall();
	~Wall();

	virtual void update(float dt);

	float getWidth(){return width;}
	float getHeight(){return height;}
	float getDepth(){return depth;}

private:
	float radius;
	float radiusSquared;
};

#endif