//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//
//=============================================================================


#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include <d3dx9math.h>
#include "LineObject.h"
#include "Wall.h"
#include "gameError.h"
#include "Player.h"
#include "Bullet.h"
#include "debugText.h"
#include "Audio.h"
#include "namespaces.h"
#include "pickup.h"
#include <ctime>
using std::time;
#include "Light.h"
#include "Enemy.h"

#include "Waypoint.h"

#include <queue>
using std::priority_queue;

namespace gameNS {
	const int NUM_WALLS = 28;
	const int PERIMETER = 4;
	const int NUM_BULLETS = 5;
	const int NUM_PICKUPS = 4;
}

class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void initOrigin();
	void initPickups();
	void initBullets();
	void initBasicGeometry();
	void initTextStrings();
	void initBasicVariables();
	void initWallPositions();
	void initUniqueObjects();

	void updateScene(float dt);
	void updatePickups(float dt);
	void updateOrigin(float dt);
	void updateWalls(float dt);
	void updateUniqueObjects(float dt);
	void updatePlayer(float dt);
	void updateCamera();
	
	void handleUserInput();
	void handleWallCollisions(Vector3 pos);
	void handlePickupCollisions(float dt);

	void drawScene(); 
	void drawLine(LineObject*);
	void drawOrigin();
	void drawPickups();
	void drawWalls();

	void onResize();
	Vector3 moveRuggerDirection();
	void printText(DebugText text);
	void printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, int value = -1);
	void setDeviceAndShaderInformation();
	void doEndScreen();
	void firstPassCleanup();

	//This will need to be relocated into the enemies, so they will individually calculate their paths per frame
	list<Waypoint*> pathfindAStar(Waypoint* source, Waypoint* target);

private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Box mWallMesh;

	Line rLine, bLine, gLine;
	Box mBox, redBox, brick, bulletBox, eBulletBox, yellowGreenBox, goldBox, blueBox, tealBox, maroonBox;
	Player player;
	vector<Bullet*> pBullets;
	LineObject xLine, yLine, zLine;
	Wall walls[gameNS::NUM_WALLS];
	Wall floor;
	vector<Pickup> pickups;
	GameObject superLowFloorOffInTheDistanceUnderTheScene;

	//Lighting and Camera-specific declarations
	Light mLights[3];
	int mLightType; // 0 (parallel), 1 (point), 2 (spot)
	D3DXVECTOR3 mEyePos;
	D3DXVECTOR3 target;
	D3DXVECTOR3 perpAxis;
	D3DXVECTOR3 moveAxis;

	//Pathfinding stuff
	Box wLine;
	//GameObject wayLine[100][100];
	GameObject** wayLine;
	Waypoint* waypoints[100][100];
	priority_queue<Waypoint*> openWay;
	priority_queue<Waypoint*> closedWay;
	Waypoint* dest;

	float spinAmount;
	int shotTimer;
	ID3D10Effect* mFX;
	ID3D10EffectTechnique* mTech;
	ID3D10InputLayout* mVertexLayout;
	ID3D10EffectMatrixVariable* mfxWVPVar;
	ID3D10EffectMatrixVariable* mfxWorldVar;
	ID3D10EffectVariable* mfxEyePosVar;
	ID3D10EffectVariable* mfxLightVar;
	ID3D10EffectScalarVariable* mfxLightType;
	ID3D10ShaderResourceView* mDiffuseMapRV;
	ID3D10ShaderResourceView* mSpecMapRV;
	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;
	D3DXMATRIX mCompCubeWorld;
	bool night;
	//my addition
	ID3D10EffectVariable* mfxFLIPVar;

	D3DXMATRIX mView;
	D3DXMATRIX mProj;
	D3DXMATRIX mWVP;
	D3DXMATRIX mVP;

	//my edits
	D3DXMATRIX worldBox1, worldBox2;

	//Theta represents vertical rotation (z-axis rotation)
	float mTheta;
	//Phi represents horizontal rotation (y-axis rotation)
	float mPhi;
	int incrementedYMargin;

	int score;
	int lineHeight;
	bool firstpass;
	bool startScreen, endScreen;
	float dt;
	DebugText sText, eText;
};

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), mTheta(0.0f), mPhi(0.0f), mfxWorldVar(0), 
  mfxEyePosVar(0), mfxLightVar(0), mfxLightType(0)
{
	srand(time(0));
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
	D3DXMatrixIdentity(&mVP); 
	score = 0;
	firstpass = true;
	startScreen = true;
	endScreen = false;
	night = false;
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	for(int i=0; i<100; i++)
	{
		delete [] wayLine[i];
	}

	ReleaseCOM(mFX);
	ReleaseCOM(mVertexLayout);

	//CHANGE if audio needs to be released in d3dApp instead
	safeDelete(audio);
  // 
}



void ColoredCubeApp::initApp()
{
	D3DApp::initApp();
	//pos will eventually be player.x, player.height, player.z)
	mEyePos = D3DXVECTOR3(0, 5, 0);
	initBasicGeometry();
	initBasicVariables(); //Like shotTimer, etc.
	initTextStrings(); //Like start/end screen text
	initUniqueObjects(); //Like the floor

	initOrigin();
	initBullets();
	initPickups();
	initWallPositions();

	mClearColor = D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f);
	mLightType = 0;
 
	// Parallel light.
	mLights[0].dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mLights[0].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//mLights[0].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLights[0].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[0].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLights[0].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[0].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[0].pos = D3DXVECTOR3(0, 50, 0);
 
	// Pointlight--position is changed every frame to animate.
	//mLights[1].ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	mLights[1].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	//mLights[1].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	//mLights[1].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[1].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[1].att.x    = 0.0f;
	mLights[1].att.y    = 0.1f;
	mLights[1].att.z    = 0.0f;
	mLights[1].range    = 50.0f;

	// Spotlight--position and direction changed every frame to animate.
	mLights[2].ambient  = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	//mLights[2].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLights[2].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[2].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLights[2].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[2].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[2].att.x    = 1.0f;
	mLights[2].att.y    = 0.0f;
	mLights[2].att.z    = 0.0f;
	mLights[2].spotPow  = 64.0f;
	mLights[2].range    = 10000.0f;
		
	//Pathfinding testing
	wLine.init(md3dDevice, 1.0f, WHITE);
	wayLine = new GameObject*[100];
	for(int i=0; i<100; i++) wayLine[i] = new GameObject[100];

	for(int i=0; i<100; i++){
		for(int j=0; j<100; j++)
		{
			waypoints[i][j] = new Waypoint(D3DXVECTOR3(i, 0, j));
			wayLine[i][j].init(&wLine, 1.0f, waypoints[i][j]->getPosition(), D3DXVECTOR3(0,0,0), 0.0f, 0.125f);
		}
	}
	for(int i=0; i<100; i++)
	{
		for(int j=0; j<100; j++)
		{
			//Currently just waypoints in the cardinal directions
			if(i-1 >= 0) waypoints[i][j]->addNeighbor(waypoints[i-1][j]);
			if(i+1 < 100) waypoints[i][j]->addNeighbor(waypoints[i+1][j]);
			if(j-1 >= 0) waypoints[i][j]->addNeighbor(waypoints[i][j-1]);
			if(j+1 < 100) waypoints[i][j]->addNeighbor(waypoints[i][j+1]);
		}
	}
	dest = waypoints[98][98];

	player.init(&mBox, pBullets, sqrt(2.0f), Vector3(5,5,0), Vector3(0,0,0), 0, 1);
	buildFX();
	buildVertexLayouts();

	mWallMesh.init(md3dDevice, 1.0f);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"Companion Cube.jpg", 0, 0, &mDiffuseMapRV, 0 ));

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, 
		L"defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));

	//pls no more
	//audio->playCue(MUSIC);
}

void ColoredCubeApp::initPickups() {
	for (int i = 0; i < gameNS::NUM_PICKUPS; i++)
		pickups.push_back(Pickup());

	pickups[0].init(&tealBox, Vector3(-35,0,95));
	pickups[1].init(&tealBox, Vector3(-6,0,-80));
	pickups[2].init(&tealBox, Vector3(-84,0,-56));
	pickups[3].init(&tealBox, Vector3(10,0,80));
}

void ColoredCubeApp::initBullets() {
	for (int i = 0; i < gameNS::NUM_BULLETS; i++) {
		pBullets.push_back(new Bullet());
		pBullets[i]->init(&bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1);
	}
}

void ColoredCubeApp::initBasicGeometry() {
	mBox.init(md3dDevice, 1.0f, WHITE);
	tealBox.init(md3dDevice, 1.0f, colorNS::TEAL);
	brick.init(md3dDevice, 1.0f, DARKBROWN);
	bulletBox .init(md3dDevice, 0.5f, BEACH_SAND);
	eBulletBox.init(md3dDevice, 0.5f, RED);
	maroonBox.init(md3dDevice, 10000, colorNS::MAROON);
	//redBox.init(md3dDevice, 0.00001f, RED);
	yellowGreenBox.init(md3dDevice, 1.f, LIGHT_YELLOW_GREEN);
	goldBox.init(md3dDevice, 1.0f, YELLOW);
	blueBox.init(md3dDevice, 2.0f, BLUE);
	rLine.init(md3dDevice, 10.0f, RED);
	bLine.init(md3dDevice, 10.0f, BLACK);
	gLine.init(md3dDevice, 10.0f, GREEN);
}

void ColoredCubeApp::initTextStrings() {
	sText.addLine("WELCOME RUGGER !", 10, 10);
	sText.addLine("WASD TO MOVE", 10, 30);
	sText.addLine("ARROW KEYS TO SHOOT", 10, 50);
	sText.addLine("HOLD SHIFT TO SPRINT!", 10, 70);
	sText.addLine("GO KILL DUNSTAN FOR ME!", 10, 90);
	sText.addLine("PRESS ANY KEY TO BEGIN !", 250, 300);
	eText.addLine("CONGRATS RUGGER I WON!", 250, 300);
}

void ColoredCubeApp::initBasicVariables() {
	startScreen = false;
	shotTimer = 0;
}

void ColoredCubeApp::initWallPositions() {
	
//				   geom,  rad,  position,				sc,	w,		h,	d
	walls[0].init(&brick, 2.0f, Vector3(155, 0, 250), 	1,	115,	10, 10);//	Left/Front wall 
	walls[1].init(&brick, 2.0f, Vector3(-155, 0, -250),	1,	115,	10, 10);//	Right/back wall
	walls[2].init(&brick, 2.0f, Vector3(250, 0, 155),	1,	10,		10, 95);//	Front/Left wall
	walls[3].init(&brick, 2.0f, Vector3(-250, 0, -155),	1,	10,		10, 95);//	Back/Right wall

	walls[4].init(&brick, 2.0f, Vector3(-155, 0, 250),	1,	115,	10, 10);//	Left/Back wall 
	walls[5].init(&brick, 2.0f, Vector3(155, 0, -250),	1,	115,	10, 10);//	Right/Front wall
	walls[6].init(&brick, 2.0f, Vector3(250, 0, -155),	1,	10,		10, 95);//	Front/Right wall
	walls[7].init(&brick, 2.0f, Vector3(-250, 0, 155),	1,	10,		10, 95);//	Back/Left wall

	walls[8].init(&brick, 2.0f, Vector3(36, 0, 55),		1,	20,		5,	1);//	Left/Front inner wall 
	walls[9].init(&brick, 2.0f, Vector3(-36, 0, -55),	1,	20,		5,	1);//	Right/Back inner wall
	walls[10].init(&brick, 2.0f, Vector3(55, 0, 36),	1,	1,		5,	20);//	Front/Left inner wall
	walls[11].init(&brick, 2.0f, Vector3(-55, 0, -36),	1,	1,		5,	20);//	Back/Right inner wall

	walls[12].init(&brick, 2.0f, Vector3(-36, 0, 55),	1,	20,		5,	1);//	Left/Back inner wall 
	walls[13].init(&brick, 2.0f, Vector3(36, 0, -55),	1,	20,		5,	1);//	Right/Front inner wall
	walls[14].init(&brick, 2.0f, Vector3(55, 0, -36),	1,	1,		5,	20);//	Front/Right inner wall
	walls[15].init(&brick, 2.0f, Vector3(-55, 0, 36),	1,	1,		5,	20);//	Back/Left inner wall

	walls[16].init(&brick, 2.0f, Vector3(150, 0, -150),	1,	20,		20,  20);// Front right corner buildings
	walls[17].init(&brick, 2.0f, Vector3(150, 0, -50),	1,	20,		20,  20);
	walls[18].init(&brick, 2.0f, Vector3(50, 0, -150),	1,	20,		20,  20);

	walls[19].init(&brick, 2.0f, Vector3(150, 0, 150),	1,	20,		20,  20);// Front left corner buildings
	walls[20].init(&brick, 2.0f, Vector3(150, 0, 50),	1,	20,		20,  20);
	walls[21].init(&brick, 2.0f, Vector3(50, 0, 150),	1,	20,		20,  20);

	walls[22].init(&brick, 2.0f, Vector3(-150, 0, -150),1,	20,		20,  20);// Back right corner buildings
	walls[23].init(&brick, 2.0f, Vector3(-150, 0, -50),	1,	20,		20,  20);
	walls[24].init(&brick, 2.0f, Vector3(-50, 0, -150),	1,	20,		20,  20);

	walls[25].init(&brick, 2.0f, Vector3(-150, 0, 150),	1,	20,		20,  20);// Back left corner buildings
	walls[26].init(&brick, 2.0f, Vector3(-150, 0, 50),	1,	20,		20,  20);
	walls[27].init(&brick, 2.0f, Vector3(-50, 0, 150),	1,	20,		20,  20);

}

void ColoredCubeApp::initUniqueObjects() {
	floor.init(&yellowGreenBox, 2.0f, Vector3(0,-1.5f,0), 1.0f, 250, 1, 250);
	superLowFloorOffInTheDistanceUnderTheScene.init(&maroonBox, 2.0f, Vector3(0,-10.0f,0), Vector3(0,0,0), 0, 100000);
}

void ColoredCubeApp::initOrigin() {
	xLine.init(&rLine, Vector3(0,0,0), 5);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&bLine, Vector3(0,0,0), 5);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(ToRadian(90));
	zLine.init(&gLine, Vector3(0,0,0), 5);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(ToRadian(90));
}



void ColoredCubeApp::updateScene(float dt)
{
	ColoredCubeApp::dt = dt;
	bool playing = (!endScreen && !startScreen);
	Vector3 oldPos = player.getPosition();
	if(input->isKeyDown(VK_ESCAPE)) PostQuitMessage(0);

	//Going to need to do this to invalidate some of the waypoints
	firstPassCleanup(); //I don't think we will need this, since money isn't being randomly placed. Or is it?

	//if(!night)
	//{
	//	mLights[0].diffuse.r += 0.1*dt;
	//	mLights[0].diffuse.g += 0.1*dt;
	//	mLights[0].diffuse.b += 0.1*dt;
	//	if(mLights[0].diffuse.r >= 1 || mLights[0].diffuse.g >= 1 || mLights[0].diffuse.b >= 1) night = true;
	//}
	//else if (night)
	//{
	//	mLights[0].diffuse.r -= 0.1f * dt;
	//	mLights[0].diffuse.g -= 0.1f * dt;
	//	mLights[0].diffuse.b -= 0.1f * dt;
	//	if(mLights[0].diffuse.r <= 1 || mLights[0].diffuse.g <= 1 || mLights[0].diffuse.b <= 1) night = false;
	//}
	
	if(playing)
	{	
		//waypoints
		for(int i=0; i<100; i++)for(int j=0; j<100; j++)if(waypoints[i][j]->isActive())wayLine[i][j].update(dt);
		//find path
		


		//General Update
		D3DApp::updateScene(dt);
		updateOrigin(dt);
		handleUserInput();
		updatePlayer(dt);
		updatePickups(dt);
		updateWalls(dt);
		updateUniqueObjects(dt); //Like floor


		//Handle Collisions
		handleWallCollisions(oldPos);
		handlePickupCollisions(dt);
	}

	else if (startScreen) if(input->anyKeyPressed()) startScreen = false;
	else doEndScreen();
	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mLights[2].pos = D3DXVECTOR3(0, 10, 0);
	D3DXVec3Normalize(&mLights[2].dir, &D3DXVECTOR3(0, -1, 0));

	// The point light circles the scene as a function of time, 
	// staying 7 units above the land's or water's surface.
	mLights[1].pos.x = 50.0f*cosf( mTimer.getGameTime() );
	mLights[1].pos.z = 50.0f*sinf( mTimer.getGameTime() );
	mLights[1].pos.y = 7.0f;


	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mLights[2].pos = mEyePos;
	D3DXVec3Normalize(&mLights[2].dir, &(target-mEyePos));

	updateCamera();
}

void ColoredCubeApp::firstPassCleanup() {
	//For the first update pass, we want to remove any money that is colliding with cameras or walls
	if(firstpass)
	{
		bool found = false;
		firstpass = false;
		for(int i=0; i<gameNS::NUM_WALLS; i++)
		{
			for(int j=0; j<100; j++)
			{
				for(int k=0; k<100; k++)
				{
					//if(walls[i].collided(&wayLine[j][k])) waypoints[j][k]->setActive(false);
					if(wayLine[j][k].collided(&walls[i])) waypoints[j][k]->setActive(false);
					//just for now, find a starting node
					else if(!found)
					{
						found = true;
						openWay.push(waypoints[j][k]);
					}
				}
			}
		}
	}
}

void ColoredCubeApp::updateCamera() {
	int dx = input->getMouseRawX();
	int dy = input->getMouseRawY();

	if(dx < 0)
	//if(input->isKeyDown(KEY_A))
	{
		mPhi -= 4.0f*dt;
	}
	if(dx > 0)
	//if(input->isKeyDown(KEY_D))
	{
		mPhi += 4.0f*dt;
	}
	if(dy < 0)
	//if(input->isKeyDown(KEY_W))
	{
		mTheta += 3.0f*dt;
	}
	if(dy > 0)
	//if(input->isKeyDown(KEY_S))
	{
		mTheta -= 3.0f*dt;
	}
	//Restricting the mouse movement
	RECT restrict = {639, 399, 640, 400};
	ClipCursor(&restrict);

		if(input->isKeyDown(KEY_W))
	{
		moveAxis.y = 0;
		D3DXVec3Normalize(&moveAxis, &moveAxis);
		mEyePos += moveAxis * dt * 20;
	}
	if(input->isKeyDown(KEY_S))
	{
		moveAxis.y = 0;
		D3DXVec3Normalize(&moveAxis, &moveAxis);
	
		mEyePos -= moveAxis * dt * 20;
	}
	if(input->isKeyDown(KEY_D))
	{
		mEyePos -= perpAxis * dt * 20;
	}
	if(input->isKeyDown(KEY_A))
	{
		mEyePos += perpAxis * dt * 20;
	}
	if(input->isKeyDown(VK_SPACE))
	{
		mEyePos.y += 20 * dt;
	}
	if(input->isKeyDown(VK_SHIFT))
	{
		mEyePos.y -= 20 * dt;
	}

	// Restrict the angle mPhi and radius mRadius.
	if( mTheta < -(PI/2.0f) + 0.01f)	mTheta = -(PI/2.0f) + 0.01f;
	if( mTheta > PI/2.0f - 0.01f)	mTheta = (PI/2.0f) - 0.01f;
	//target will start pointing in the +x direction and will be rotated according to the camera's net rotations
	//Should eventually be pos.x+1, pos.y, pos.z to make the camera rotate according to its own axis
	//D3DXVECTOR3 target(1, 0, 0);
	//Up remains unchanged.
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);

	//Big changes to lookat vector
	D3DXVECTOR4 t4(1, 0, 0, 1);
	Matrix rotZ, rotY, trans;
	D3DXMatrixRotationZ(&rotZ, mTheta);
	D3DXMatrixRotationY(&rotY, mPhi);
	D3DXMatrixTranslation(&trans, mEyePos.x, mEyePos.y, mEyePos.z);
	//Rotate around the Z-axis first, then the Y-axis, then translate to the camera position
	Matrix transform = rotZ * rotY/* * trans*/;
	
	//t4 now holds the final position of the camera target point
	D3DXVec4Transform(&t4, &t4, &transform);
	moveAxis = D3DXVECTOR3(t4.x, t4.y, t4.z);
	D3DXVec3Normalize(&moveAxis, &moveAxis);
	D3DXVec3Cross(&perpAxis, &(D3DXVECTOR3(t4.x, t4.y, t4.z)), &up);
	D3DXVec3Normalize(&perpAxis, &perpAxis);
	D3DXVec4Transform(&t4, &t4, &trans);
	//Assign it into a Vec3 and we should be good to go
	target = D3DXVECTOR3(t4.x, t4.y, t4.z);
	//D3DXVec3Normalize(&target, &target);

	

	D3DXMatrixLookAtLH(&mView, &mEyePos, &target, &up);
}

void ColoredCubeApp::doEndScreen() {
	Sleep(2000);
	if(input->anyKeyPressed())
	{
		endScreen = false;
		PostQuitMessage(0);
	}
}

void ColoredCubeApp::updateUniqueObjects(float dt) {
	floor.update(dt);
	//big floor update must also be added here for it to show.
}

void ColoredCubeApp::updateWalls(float dt) {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
		walls[i].update(dt);
}

void ColoredCubeApp::updatePlayer(float dt) {
	player.setVelocity(moveRuggerDirection() * player.getSpeed());
	player.update(dt);
}

void ColoredCubeApp::handleUserInput() {

	if(input->isKeyDown(VK_SHIFT)) player.setSpeed(40);
	else player.setSpeed(20);

}

void ColoredCubeApp::handleWallCollisions(Vector3 pos) {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
	{
		if(player.collided(&walls[i]))
			player.setPosition(pos);

		for (int j = 0; j < pBullets.size(); j++) {
			if (pBullets[j]->collided(&walls[i])) {
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
			}		
		}
	}
}

void ColoredCubeApp::handlePickupCollisions(float dt) {
	
}

void ColoredCubeApp::updatePickups(float dt) {
	for (int i = 0; i < pickups.size(); i++) {
			if (player.collided(&pickups[i])) {
				pickups[i].setInActive();
				player.charge();
			}
			pickups[i].update(dt);
	}
}

void ColoredCubeApp::updateOrigin(float dt) {
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);
}



void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();
	incrementedYMargin = 5;
	lineHeight = 20;
	bool playing = (!startScreen && !endScreen);

	setDeviceAndShaderInformation();

	if(playing) {		
		mVP = mView*mProj;

		//for(int i=0; i<100; i++) for(int j=0; j<100; j++) if(waypoints[i][j]->isActive())wayLine[i][j].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		//drawOrigin();
		floor.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		drawWalls();
		drawPickups();
		mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
		D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			mWallMesh.draw();
		}
		player.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);

		printText("Score: ", 5, 5, 0, 0, score); //This has to be the last thing in the draw function.
	}
	else if(startScreen)
		printText(sText);
	else { // End Screen 
		printText(eText);
		printText("Score: ", 300, 350, 0, 0, score);
	}
	
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDevice->RSSetState(0);
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0); //Comment this out for expert mode
}

void ColoredCubeApp::drawWalls() {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
		walls[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}

void ColoredCubeApp::printText(DebugText text) {
	for (int i = 0; i < text.getSize(); i++)
		{
			int xMargin = text.lines[i].x;
			int yMargin = text.lines[i].y;

			if (xMargin == -1)
				xMargin = 5;
			if (yMargin == -1) {
				yMargin = incrementedYMargin;
				incrementedYMargin += lineHeight;
			}
			printText(text.lines[i].s, xMargin, yMargin, 0, 0);
		}
}

void ColoredCubeApp::printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, int value) {
	
	RECT POS = {rectPosX, rectPosY, rectWidth, rectHeight};
	std::wostringstream outs;   
	outs.precision(6);
	if (value != -1)
		outs << text.c_str() << score;
	else 
		outs << text.c_str();
	std::wstring sc = outs.str();
	mFont->DrawText(0, sc.c_str(), -1, &POS, DT_NOCLIP, WHITE);
}

void ColoredCubeApp::drawLine(LineObject* line) {
	mWVP = line->getWorldMatrix()*mView*mProj;
	mfxWVPVar->SetMatrix((float*)&mWVP);
	line->setMTech(mTech);
	line->draw();
}

void ColoredCubeApp::buildFX()
{
		DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
    shaderFlags |= D3D10_SHADER_DEBUG;
	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
#endif
 
	ID3D10Blob* compilationErrors = 0;
	HRESULT hr = 0;
	hr = D3DX10CreateEffectFromFile(L"lighting.fx", 0, 0, 
		"fx_4_0", shaderFlags, 0, md3dDevice, 0, 0, &mFX, &compilationErrors, 0);
	if(FAILED(hr))
	{
		if( compilationErrors )
		{
			MessageBoxA(0, (char*)compilationErrors->GetBufferPointer(), 0, 0);
			ReleaseCOM(compilationErrors);
		}
		DXTrace(__FILE__, (DWORD)__LINE__, hr, L"D3DX10CreateEffectFromFile", true);
	} 

	mTech = mFX->GetTechniqueByName("LightTech");
	
	mfxWVPVar    = mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar  = mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar = mFX->GetVariableByName("gEyePosW");
	mfxLightVar  = mFX->GetVariableByName("gLight");
	mfxLightType = mFX->GetVariableByName("gLightType")->AsScalar();
	mfxDiffuseMapVar = mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar    = mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar     = mFX->GetVariableByName("gTexMtx")->AsMatrix();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 36, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, 48, D3D10_INPUT_PER_VERTEX_DATA, 0},
	};

	// Create the input layout
    D3D10_PASS_DESC PassDesc;
    mTech->GetPassByIndex(0)->GetDesc(&PassDesc);
    HR(md3dDevice->CreateInputLayout(vertexDesc, 5, PassDesc.pIAInputSignature,
		PassDesc.IAInputSignatureSize, &mVertexLayout));
}

Vector3 ColoredCubeApp::moveRuggerDirection()
{
	Vector3 dir = Vector3(0,0,0);

	return dir;
}

void ColoredCubeApp::setDeviceAndShaderInformation() {
	// Restore default states, input layout and primitive topology 
	// because mFont->DrawText changes them.  Note that we can 
	// restore the default states by passing null.
	md3dDevice->OMSetDepthStencilState(0, 0);
	float blendFactors[] = {0.0f, 0.0f, 0.0f, 0.0f};
	md3dDevice->OMSetBlendState(0, blendFactors, 0xffffffff);

    md3dDevice->IASetInputLayout(mVertexLayout);
    md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set per frame constants.
	mfxEyePosVar->SetRawValue(&mEyePos, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mLights[mLightType], 0, sizeof(Light));
	mfxLightType->SetInt(mLightType);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mCompCubeWorld);
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	// Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif


	ColoredCubeApp theApp(hInstance);
	
	theApp.initApp();

	return theApp.run();
}

void ColoredCubeApp::onResize()
{
	D3DApp::onResize();

	float aspect = (float)mClientWidth/mClientHeight;
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, 1000.0f);
}

void ColoredCubeApp::drawOrigin() {
	//draw the lines
	drawLine(&xLine);
	drawLine(&yLine);
	drawLine(&zLine);
}

void ColoredCubeApp::drawPickups() {
	//Set mVP to be view*projection, so we can pass that into GO::draw(..)
	
	for (int i = 0; i < pickups.size(); i++)
	pickups[i].draw(mfxWVPVar, mTech, &mVP);
}
