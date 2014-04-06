#ifndef ENEMY_H
#define ENEMY_H
#include "GameObject.h"
#include "Waypoint.h"
#include <vector>
using std::vector;
#include <list>
using std::list;
#include "Player.h"

//enum STATE {};


namespace enemyNS {
	const float SPEED = 20.0f;
	const float NIGHT_SPEED = 50.0f;
	const float DAY_SPEED = 10.0f;
}

class Enemy : public GameObject
{
public:
	Enemy();
	~Enemy();

	//Width and height in integral number of boxes(bricks)
	virtual void init(Box *b, float r, Vector3 pos, float s = 1, int width = 1, int height = 1, int depth = 1, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
	virtual void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	virtual void update(float dt);

	void update(float dt, Player* p);
	void ai();
	void attack(Player* p);
	void damage(int d) {health -= d;}

	void setDestination(D3DXVECTOR3& d) {destination = d;}
	D3DXVECTOR3 getDestination(){return destination;}
	D3DXVECTOR3 getOldPos(){return oldPos;}

	list<Waypoint*> pathfindAStar(Waypoint* source, Waypoint* target);
	Waypoint* findNearestWaypoint(D3DXVECTOR3&);

	//For positioning waypoint indicators
	vector<D3DXVECTOR3> waypointPositions();

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
	D3DXVECTOR3 destination;

	list<Waypoint*> nav;
	//Waypoint* waypoints;
	float speed;

	Waypoint* waypoints[WAYPOINT_SIZE][WAYPOINT_SIZE];
	void initWaypoints();

	Waypoint* target;

	float lastAttacked;
	int health;
	D3DXVECTOR3 oldPos;
};


#endif