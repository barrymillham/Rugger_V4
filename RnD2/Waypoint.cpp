#include "Waypoint.h"

Waypoint::Waypoint(D3DXVECTOR3 position): 
	pos(position), active(true)
{}

Waypoint::~Waypoint() {}

void Waypoint::calculateScore(Waypoint* target)
{
	
}