#ifndef __WAYPOINT_H
#define __WAYPOINT_H

#include "d3dUtil.h"
#include <list>
using std::list;
#include <vector>
using std::vector;

class Waypoint {
public:
	Waypoint(D3DXVECTOR3 position);
	~Waypoint();

	void calculateScore(Waypoint* target);

	D3DXVECTOR3 getPosition(){return pos;}
	void setPosition(D3DXVECTOR3 p){pos = p;}
	bool isActive(){return active;}
	void setActive(bool a){active = a;}
	void addNeighbor(Waypoint* n){adjacentWaypoints.push_back(n);}
	vector<Waypoint*> getNeighbors(){return adjacentWaypoints;}

	float getFCost() const{return fCost;}
	void setFCost(float f){fCost = f;}
	float getGCost() const {return gCost;}
	void setGCost(float g){gCost = g;}

	void setParent(Waypoint* p){parent = p;}
	Waypoint* getParent(){return parent;}


private:
	//Position of the waypoint in 3d space, hopefully will correspond to its index in the main array of waypoints
	D3DXVECTOR3 pos;

	//Heuristic cost and actual cost
	float fCost;
	float gCost;

	//If a waypoint is possible to be moved through
	bool active;

	//1-depth adjacency list
	vector<Waypoint*> adjacentWaypoints;

	//Parent pointer to reconstruct final path
	Waypoint* parent;
};

struct WaypointCompare {
	bool operator() (const Waypoint* w1, const Waypoint* w2) const
	{
		return w1->getFCost() > w2->getFCost();
	}
};

#endif