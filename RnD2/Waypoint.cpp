#include "Waypoint.h"

Waypoint::Waypoint(D3DXVECTOR3 position): 
	pos(position), active(true)
{
	parent = 0;
	fCost = 0;
	gCost = 0;
}

Waypoint::~Waypoint() {}

void Waypoint::calculateScore(Waypoint* target)
{
	
}