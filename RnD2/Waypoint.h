#ifndef __WAYPOINT_H
#define __WAYPOINT_H

#include "d3dUtil.h"
#include <list>
using std::list;
#include <vector>
using std::vector;

enum CONTAINER {NONE, OPEN, CLOSED};

class Waypoint {
public:
	Waypoint()
	{
		fCost = 0;
		gCost = 0;
		active = true;
		containedIn = NONE;
	}

	Waypoint(D3DXVECTOR3 position);

	Waypoint(const Waypoint& w)
	{
		fCost = w.fCost;
		gCost = w.gCost;
		pos = w.pos;
		adjacentWaypoints = w.adjacentWaypoints;
		parent = w.parent;
	}

	Waypoint& operator=(const Waypoint& w)
	{
		fCost = w.fCost;
		gCost = w.gCost;
		pos = w.pos;
		adjacentWaypoints = w.adjacentWaypoints;
		parent = w.parent;
		return *this;
	}

	~Waypoint();


	D3DXVECTOR3 getPosition(){return pos;}
	void setPosition(const D3DXVECTOR3& p){pos = p;}

	bool isActive(){return active;}
	void setActive(bool a){active = a;}

	void addNeighbor(Waypoint* n){adjacentWaypoints.push_back(n);}
	vector<Waypoint*> getNeighbors(){return adjacentWaypoints;}

	float getFCost() const{return fCost;}
	void setFCost(const float& f){fCost = f;}
	float getGCost() const {return gCost;}
	void setGCost(const float& g){gCost = g;}

	void setParent(Waypoint* p){parent = p;}
	Waypoint* getParent(){return parent;}

	CONTAINER getContainer(){return containedIn;}
	void setContainer(const CONTAINER& c){containedIn = c;}
	
private:
	//Flag to speed up container searching
	CONTAINER containedIn;

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