#ifndef __WAYPOINT_H
#define __WAYPOINT_H

#include "d3dUtil.h"
#include <list>
using std::list;

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
	list<Waypoint*> getNeighbors(){return adjacentWaypoints;}
	float getCost(){return cost;}
	void setCost(float f){cost = f;}


private:
	//Position of the waypoint in 3d space, hopefully will correspond to its index in the main array of waypoints
	D3DXVECTOR3 pos;

	//Heuristic cost and 
	float cost;

	//If a waypoint is possible to be moved through
	bool active;

	//1-depth adjacency list
	list<Waypoint*> adjacentWaypoints;
};

#endif