#include "Enemy.h"
#include <queue>
using std::priority_queue;

void queue_remove(priority_queue<Waypoint*, vector<Waypoint*>, WaypointCompare>& pq, Waypoint* w);
float heuristic(Waypoint* x, Waypoint* y);

Enemy::Enemy()
{
	radius = 0.0f;
	active = false;
	Identity(&world);
	Identity(&mTranslate);
	Identity(&mRotate);
	Identity(&mScale);
	width = 0;
	height = 0;
	depth = 0;
	rotX = 0.0f;
	rotY = 0.0f;
	rotZ = 0.0f;
	speed = 5.0f;
	target = 0;
	velocity = Vector3(0.0f, 0.0f, 0.0f);
	lastAttacked = 0.5f;
	health = 100;
}

Enemy::~Enemy()
{
	box = NULL;
}

void Enemy::init(Box *b, float r, Vector3 pos, float s, int w, int h, int d, float rx, float ry, float rz)
{
	box = b;
	radius = r;
	radius *= 1.01f; //fudge factor
	position = pos;
	scale = s;
	radiusSquared = radius * radius;
	width = w*s;
	height = h*s;
	depth = d*s;
	rotX = rx;
	rotY = ry;
	rotZ = rz;
	//Translate(&world, position.x, position.y, position.z);

	destination = D3DXVECTOR3(0, 0, 0);
	initWaypoints();
}

//Call this after calculating collisions
void Enemy::update(float dt)
{
	Identity(&world);
	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);
}

//Call this to set appropriate velocity
void Enemy::update(float dt, Player* p)
{
	if(!active) return;

	if(health <= 0)
	{
		active = false;
		p->addScore(10);
		return;
	}
	oldPos = position;

	Identity(&world);
	lastAttacked += dt;
	float dist = D3DXVec3Length(&(position - p->getPosition()));
	
	if(dist <= 15)
	{
		nav.clear();
		velocity = D3DXVECTOR3(0,0,0);
		attack(p);
		facing = true;
	}
	else if (dist <= 50)
	{
		facing = true;
		nav.clear();
		D3DXVECTOR3 tar;
		D3DXVec3Normalize(&tar, &(p->getPosition() - position));
		tar.y = 0;
		velocity = tar * speed;
	}
	else
	{
		//calculate the path from the nearest waypoint to the nearest waypoint to the player
		facing = false;
		if(nav.empty())
		{
			calculatePath(p);
		}
		else
		{
			if(target == 0)
			{
				target = nav.front();
				nav.pop_front();
			}
			if(D3DXVec3Length(&(position - target->getPosition())) > 2)
			{
				D3DXVECTOR3 tar;
				D3DXVec3Normalize(&tar, &(target->getPosition() - position));
				velocity = tar * speed;
			}
			//we are AT the destination (of this leg of the journey)
			else
			{
				calculatePath(p);
				nav.pop_front();
				if(!nav.empty()) target = nav.front();
			}
		}
	}

	position = position + velocity * dt;
	update(dt);
}

void Enemy::attack(Player* p)
{
	if(lastAttacked >= 0.5f){
		lastAttacked = 0;
		p->damage(5);
		p->grunt();
	}
}

list<Waypoint*> Enemy::pathfindAStar(Waypoint* src, Waypoint* dest)
{
	priority_queue<Waypoint*, vector<Waypoint*>, WaypointCompare> openWay;
	vector<Waypoint*> closedWay;
	list<Waypoint*> path;
	if(src == dest)
	{
		path.push_front(src);
		return path;
	}

	for(int i=0; i<WAYPOINT_SIZE; i++)
	{
		for(int j=0; j<WAYPOINT_SIZE; j++)
		{
			waypoints[i][j]->setContainer(NONE);
			waypoints[i][j]->setFCost(0);
			waypoints[i][j]->setGCost(0);
			waypoints[i][j]->setParent(0);
			//wayLine[i][j].setBox(&inactiveLine);
		}
	}

	//find path
	//src = waypoints[0][0];
	src->setFCost(heuristic(src, dest));
	
	int nodesConsidered = 0;
	src->setContainer(OPEN);
	openWay.push(src);

	//OPEN = priority queue containing START
	//CLOSED = empty set
	//while lowest rank in OPEN is not the GOAL:
	while(!openWay.empty())
	{
		nodesConsidered++;
		//current = remove lowest rank item from OPEN
		Waypoint* current = openWay.top();
		openWay.pop();
		current->setContainer(NONE);

		if(current == dest) 
			break;
		//for neighbors of current
		for(int i=0; i<current->getNeighbors().size(); i++)
		{
			Waypoint* n = current->getNeighbors()[i];
			if(n->isActive())
			{
				//cost to get to n from current
				float gCost = current->getGCost() + 1;

				//Cost to get from n to target
				float hCost = heuristic(dest, n);

				//total cost through waypoint n
				float cost = gCost + hCost;

				//if neighbor in OPEN and cost less than g(neighbor):
				//and this solution is better than what we've seen
				if(n->getContainer() == OPEN && gCost < n->getGCost())
				{
					//remove neighbor from OPEN, because new path is better
					queue_remove(openWay, n);
					//n->setParent(0);
					n->setContainer(NONE);
				}
				//if neighbor in CLOSED and cost less than g(neighbor):
				//and this solution is better than what we've seen
				if(n->getContainer() == CLOSED && gCost < n->getGCost())
				{
					//remove neighbor from CLOSED
					for(int i=0; i<closedWay.size(); i++)
					{
						if(closedWay[i] == n)
						{
							closedWay[i] = closedWay[closedWay.size()-1];
							closedWay.pop_back();
						}
					}
					//n->setParent(0);
					n->setContainer(NONE);
				}
				//if neighbor not in OPEN and neighbor not in CLOSED:
				if(n->getContainer() == NONE && n->getFCost() <= current->getFCost())
				{
					//set g(neighbor) to cost
					n->setFCost(cost);
					n->setGCost(gCost);
					//set neighbor's parent to current
					n->setParent(current);
					//add neighbor to OPEN
					n->setContainer(OPEN);
					openWay.push(n);
					//set priority queue rank to g(neighbor) + h(neighbor)
				}
			}
		}
		//add current to CLOSED
		current->setContainer(CLOSED);
		closedWay.push_back(current);
	}

	Waypoint* c = dest;
	while(c->getParent() != 0)
	{
		path.push_front(c);
		c = c->getParent();
	}
	path.push_front(c);
	return path;
}

Waypoint* Enemy::findNearestWaypoint(D3DXVECTOR3& p)
{
	//Short-circuit the search by automatically targeting the center if they are in the square
	if(D3DXVec3LengthSq(&(p - D3DXVECTOR3(0,0,0))) < 55*55)
	{
		return waypoints[2][2];
	}

	//Otherwise, make the nearest waypoint anything but the center of the square
	Waypoint* nearest = waypoints[0][0];
	for(int i=0; i<WAYPOINT_SIZE; i++)
	{
		for(int j=0; j<WAYPOINT_SIZE; j++)
		{
			if(i ==2 && j == 2){}
			else{
				if(D3DXVec3LengthSq(&(nearest->getPosition() - p)) > D3DXVec3LengthSq(&(waypoints[i][j]->getPosition() - p))) 
				{
					nearest = waypoints[i][j];
				}
			}
		}
	}
	return nearest;
}

void Enemy::initWaypoints()
{
	for(int i=0; i<WAYPOINT_SIZE; i++){
		for(int j=0; j<WAYPOINT_SIZE; j++)
		{
			waypoints[i][j] = new Waypoint(D3DXVECTOR3(i*100 - 200, 0, j*100 - 200));
			waypoints[i][j]->setContainer(NONE);
			//wayLine[i][j].init(&inactiveLine, 1.0f, D3DXVECTOR3(waypoints[i][j]->getPosition().x, 2, waypoints[i][j]->getPosition().z), D3DXVECTOR3(0,0,0), 0.0f, 0.125f);
		}
	}

	for(int i=0; i<WAYPOINT_SIZE; i++)
	{
		for(int j=0; j<WAYPOINT_SIZE; j++)
		{
			//Currently just waypoints in the cardinal directions
			if(i-1 >= 0) waypoints[i][j]->addNeighbor(waypoints[i-1][j]);
			if(i+1 < WAYPOINT_SIZE) waypoints[i][j]->addNeighbor(waypoints[i+1][j]);
			if(j-1 >= 0) waypoints[i][j]->addNeighbor(waypoints[i][j-1]);
			if(j+1 < WAYPOINT_SIZE) waypoints[i][j]->addNeighbor(waypoints[i][j+1]);
		}
	}
}

void queue_remove(priority_queue<Waypoint*, vector<Waypoint*>, WaypointCompare>& pq, Waypoint* w)
{
	priority_queue<Waypoint*, vector<Waypoint*>, WaypointCompare> hold;
	int limit = pq.size();
	for(int i=0; i<limit; i++)
	{
		if(pq.top() == w) pq.pop();//don't copy it over
		else
		{
			hold.push(pq.top());
			pq.pop();
		}
	}
	pq = hold;
}

float heuristic(Waypoint* x, Waypoint* y)
{
	return abs(x->getPosition().x - y->getPosition().x) + abs(x->getPosition().z - y->getPosition().z);
}

vector<D3DXVECTOR3> Enemy::waypointPositions()
{
	vector<D3DXVECTOR3> wp;
	for(int i=0; i<WAYPOINT_SIZE; i++)
	{
		for(int j=0; j<WAYPOINT_SIZE; j++)
		{
			wp.push_back(waypoints[i][j]->getPosition());
		}
	}
	return wp;
}

void Enemy::calculatePath(Player* p)
{
	target = 0;
	nav.clear();

	//find nearest waypoint to the enemy
	src = findNearestWaypoint(position);
	//find waypoint nearest to player
	dest = findNearestWaypoint(p->getPosition());
	//Waypoint* dest = waypoints[rand()%WAYPOINT_SIZE][rand()%WAYPOINT_SIZE];

	//calculate path from nearest waypoint to the player's nearest waypoint
	//If the source is not the destination, calculate normally
	if(src != dest)
	{
		nav = pathfindAStar(src, dest);
	}
	//however if the source and destination are the same, only use that waypoint and stay there
	else 
	{
		nav.clear();
		nav.push_front(src);
	}
	//nav = pathfindAStar(src, dest);
}