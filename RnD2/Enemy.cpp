#include "Enemy.h"
#include <queue>
using std::priority_queue;

void queue_remove(priority_queue<Waypoint*, vector<Waypoint*>, WaypointCompare>& pq, Waypoint* w);
float heuristic(Waypoint* x, Waypoint* y);

Enemy::Enemy()
{
	radius = 0;
	active = true;
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
	speed = 5;
	target = 0;
	velocity = Vector3(0,0,0);
}

Enemy::~Enemy()
{
	box = NULL;
}

void Enemy::draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP)
{
	if (!active)
		return;

	Matrix mWVP = world* (*mVP);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&world);
	D3D10_TECHNIQUE_DESC techDesc;
	mTech->GetDesc( &techDesc );
	for(UINT p = 0; p < techDesc.Passes; ++p)
	{
		mTech->GetPassByIndex( p )->Apply(0);
		box->draw();
	}
}

void Enemy::init(Box *b, float r, Vector3 pos, float s, int w, int h, int d, float rx, float ry, float rz)
{
	box = b;
	radius = r;
	radius *= 1.01f; //fudge factor
	position = pos;
	scale = s;
	radiusSquared = radius * radius;
	width = w;
	height = h;
	depth = d;
	rotX = rx;
	rotY = ry;
	rotZ = rz;
	//Translate(&world, position.x, position.y, position.z);

	destination = D3DXVECTOR3(0, 0, 0);
	initWaypoints();
}

void Enemy::update(float dt)
{
	Identity(&world);
	D3DXMatrixScaling(&mScale, width, height, depth);
	D3DXMatrixTranslation(&mTranslate, position.x, position.y, position.z);
	D3DXMatrixMultiply(&world, &mScale, &mTranslate);
}

void Enemy::update(float dt, Player* p, const int& WAYPT_SIZE)
{
	Identity(&world);
	
	//if(D3DXVec3Length(&(position - p->getPosition())) >= 0) //if the enemy is far enough away from the player, pathfind
	if(true)
	{
		//calculate the path from the nearest waypoint to the nearest waypoint to the player
		Waypoint* src = waypoints[0][0];
		Waypoint* dest = waypoints[WAYPT_SIZE-1][WAYPT_SIZE-1];
		if(nav.empty())
		{
			nav = pathfindAStar(src, dest, WAYPT_SIZE);
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
				float t = 0;
				//calculate the time when our position is the same as the target
				D3DXVECTOR3 a = target->getPosition() - position;
				t = (-(D3DXVec3Dot(&a, &a)) - sqrt(pow(D3DXVec3Dot(&a, &velocity), 2) - (D3DXVec3Dot(&velocity, &velocity)*D3DXVec3Dot(&a, &a))))/(D3DXVec3Dot(&velocity, &velocity));

				int x = 0;
			}
			else
			{
				target = nav.front();
				nav.pop_front();
			}
		}
	}
	//Otherwise, just do vector tracking for movement
	else
	{

	}
	position = position + velocity * dt;
	update(dt);
}

list<Waypoint*> Enemy::pathfindAStar(Waypoint* src, Waypoint* dest, const int& WAYPT_SIZE)
{
	priority_queue<Waypoint*, vector<Waypoint*>, WaypointCompare> openWay;
	vector<Waypoint*> closedWay;
	list<Waypoint*> path;

	for(int i=0; i<WAYPT_SIZE; i++)
	{
		for(int j=0; j<WAYPT_SIZE; j++)
		{
			waypoints[i][j]->setContainer(NONE);
			waypoints[i][j]->setFCost(0);
			waypoints[i][j]->setGCost(0);
			waypoints[i][j]->setParent(0);
			//wayLine[i][j].setBox(&inactiveLine);
		}
	}

	//find path
	src = waypoints[0][0];
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

void Enemy::initWaypoints()
{
	for(int i=0; i<10; i++){
		for(int j=0; j<10; j++)
		{
			waypoints[i][j] = new Waypoint(D3DXVECTOR3(i*10, 0, j*10));
			waypoints[i][j]->setContainer(NONE);
			//wayLine[i][j].init(&inactiveLine, 1.0f, D3DXVECTOR3(waypoints[i][j]->getPosition().x, 2, waypoints[i][j]->getPosition().z), D3DXVECTOR3(0,0,0), 0.0f, 0.125f);
		}
	}
	for(int i=0; i<10; i++)
	{
		for(int j=0; j<10; j++)
		{
			//Currently just waypoints in the cardinal directions
			if(i-1 >= 0) waypoints[i][j]->addNeighbor(waypoints[i-1][j]);
			if(i+1 < 10) waypoints[i][j]->addNeighbor(waypoints[i+1][j]);
			if(j-1 >= 0) waypoints[i][j]->addNeighbor(waypoints[i][j-1]);
			if(j+1 < 10) waypoints[i][j]->addNeighbor(waypoints[i][j+1]);
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