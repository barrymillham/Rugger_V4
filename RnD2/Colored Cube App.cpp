//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//=============================================================================


#include "d3dApp.h"
#include "Box.h"
#include "GameObject.h"
#include "Line.h"
#include "Quad.h"
#include <d3dx9math.h>
#include "LineObject.h"
#include "Wall.h"
#include "Building.h"
#include "Barrel.h"
#include "gameError.h"
#include "Player.h"
#include "Bullet.h"
#include "debugText.h"
#include "Audio.h"
#include "namespaces.h"
#include "pickup.h"
#include <ctime>
#include "Light.h"
#include "LampPost.h"
#include "Enemy.h"
#include "Camera.h"
#include "HudObject.h"
#include "TextureMgr.h"
#include "InputLayouts.h"
#include "Effects.h"
#include "PSystem.h"
using std::time;

namespace gameNS {
	const float DAYLEN = 40;
	const int NUM_WALLS = 20;
	const int NUM_BUILDINGS = 39;
	const int NUM_BARRELS = 24;
	const int PERIMETER = 4;
	const int NUM_BULLETS = 50;
	const int NUM_LIGHTS = 11;
	const int NUM_FIRES = 24;
	const int NUM_LAMPS = 8;
	const float TRANSITIONTIME = 10;
	const D3DXCOLOR NIGHT_SKY_COLOR = D3DXCOLOR(0.049f, 0.049f, 0.2195f, 1.0f);
	const D3DXCOLOR DAY_SKY_COLOR = D3DXCOLOR(0.529f, 0.808f, 0.98f, 1.0f);
	const int MAX_NUM_ENEMIES = 20;
	bool PLAY_MUSIC = true;
	const float FOOTSTEP_GAP = 0.45;
	const int GRASSY_AREA_WIDTH = 110;
	const int FLASHLIGHT_NUM = 2;
	const int NUM_NIGHTS_TO_WIN = 4;
	const float FAR_CLIP = 10000.0f;
	const int PLAYER_SPEED = 40;
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
	void initBuildingPositions();
	void initUniqueObjects();
	void initLights();
	void initLamps();
	void initEnemies();
	void initHUD();

	void updateScene(float dt);
	void updatePickups(float dt);
	void placePickups();
	void updateOrigin(float dt);
	void updateWalls(float dt);
	void updateBuildings(float dt);
	void updateUniqueObjects(float dt);
	void updatePlayer(float dt);
	void updateEnemies(float dt);
	void updateCamera();
	void updateDayNight();
	void updateLamps(float dt);
	void updateDebugMode();
	void updateMusic();
	void updateHUD(float dt);

	void handleUserInput();
	void handleBuildingCollisions(Vector3 pos);
	void handleWallCollisions(Vector3 pos);
	void handleLampCollisions(Vector3 pos);
	void handlePickupCollisions(float dt);
	void handleEnemyCollisions(float dt);

	void drawScene(); 
	void drawLine(LineObject*);
	void drawOrigin();
	void drawPickups();
	void drawWalls();
	void drawLamps();
	void drawBuildings();
	void drawHUD();
	void drawBarrels();

	void onResize();
	Vector3 moveRuggerDirection();
	void printText(DebugText text);
	void printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, D3DXCOLOR color, int value = -1);
	void setDeviceAndShaderInformation();
	void doEndScreen();
	
private:
	void buildFX();
	void buildVertexLayouts();
 
private:
	Box mWallMesh;
	Box mBuildingMesh;
	Box mEnemyMesh;
	Box mPoleMesh;
	Box mStreetMesh;
	Box mBulletMesh;
	Box mRedMesh;
	Box mBlueMesh;
	Box mYellowMesh;

	Line rLine, bLine, gLine;
	Box mBox, redBox, brick, bulletBox, eBulletBox, yellowGreenBox, goldBox, blueBox, tealBox, maroonBox, clearBox, whiteBox;
	Box testBox;
	Player player;
	vector<Bullet*> pBullets;
	LineObject xLine, yLine, zLine;
	Wall walls[gameNS::NUM_WALLS];
	Building buildings[gameNS::NUM_BUILDINGS];
	Barrel barrels[gameNS::NUM_BARRELS];
	Wall floor;
	Wall floor2;
	vector<LampPost> lamps;
	vector<Pickup> dayPickups;
	vector<Pickup> nightPickups;
	vector<HudObject> hudObjects;
	Quad menu;

	//Lighting and Camera-specific declarations
	Light mLights[gameNS::NUM_LIGHTS];
	int mLightType; // 0 (parallel), 1 (point), 2 (spot)
	Light sun;
	int mLightNum;
	ID3D10EffectScalarVariable* mfxLightNum;
	D3DXVECTOR3 position;
	D3DXVECTOR3 lookAt;
	D3DXVECTOR3 perpAxis;
	D3DXVECTOR3 moveAxis;

	//Pathfinding stuff
	Box inactiveLine;
	Box activeLine;
	//GameObject wayLine[100][100];
	GameObject wayLine[WAYPOINT_SIZE*WAYPOINT_SIZE];
	
	Waypoint* dest;
	Waypoint* src;
	
	bool found;
	bool debugMode;
	bool hasntPlayedYet;
	bool nightDayTrans;
	bool placedPickups;
	int dayCount;
	bool won;
	bool walking;

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
	ID3D10ShaderResourceView* mDiffuseMapRVBuilding;
	ID3D10ShaderResourceView* mSpecMapRVBuilding;
	ID3D10ShaderResourceView* mDiffuseMapRVEnemy;
	ID3D10ShaderResourceView* mSpecMapRVEnemy;
	ID3D10ShaderResourceView* mDiffuseMapRVPole;
	ID3D10ShaderResourceView* mSpecMapRVPole;
	ID3D10ShaderResourceView* mDiffuseMapRVStreet;
	ID3D10ShaderResourceView* mSpecMapRVStreet;
	ID3D10ShaderResourceView* mDiffuseMapRVTheRoad;
	ID3D10ShaderResourceView* mSpecMapRVTheRoad;
	ID3D10ShaderResourceView* mDiffuseMapRVBuilding2;
	ID3D10ShaderResourceView* mSpecMapRVBuilding2;
	ID3D10ShaderResourceView* mDiffuseMapRVBullet;
	ID3D10ShaderResourceView* mSpecMapRVBullet;
	ID3D10ShaderResourceView* mDiffuseMapRVBarrel;
	ID3D10ShaderResourceView* mSpecMapRVBarrel;
	ID3D10ShaderResourceView* mDiffuseMapRVBlue;
	ID3D10ShaderResourceView* mSpecMapRVBlue;
	ID3D10ShaderResourceView* mDiffuseMapRVRed;
	ID3D10ShaderResourceView* mSpecMapRVRed;
	ID3D10ShaderResourceView* mDiffuseMapRVYellow;
	ID3D10ShaderResourceView* mSpecMapRVYellow;
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

	float pitch;
	float yaw; //Rotation around Y axis
	int incrementedYMargin;

	int score;
	int lineHeight;
	bool firstpass;
	bool startScreen, endScreen, level1, level2;
	float dt;
	DebugText sText, lText, wText;

	float timect;
	string timeOfDay;
	Enemy enemy[gameNS::MAX_NUM_ENEMIES];
	int nightCount;
	float stepTime;
	bool step1;
	bool flashChanged, flashOn;
	float flashChangeTime;

	Camera camera;

	//PARTICLES
	PSystem mFire[gameNS::NUM_FIRES];
	float gameTime;
};

ColoredCubeApp::ColoredCubeApp(HINSTANCE hInstance)
: D3DApp(hInstance), mFX(0), mTech(0), mVertexLayout(0),
  mfxWVPVar(0), pitch(0.0f), yaw(0.0f), mfxWorldVar(0), 
  mfxEyePosVar(0), mfxLightVar(0), mfxLightType(0)
{
	srand(time(0));
	D3DXMatrixIdentity(&mView);
	D3DXMatrixIdentity(&mProj);
	D3DXMatrixIdentity(&mWVP); 
	D3DXMatrixIdentity(&mVP); 
	gameTime = 0.0f;
}

ColoredCubeApp::~ColoredCubeApp()
{
	if( md3dDevice )
		md3dDevice->ClearState();

	fx::DestroyAll();
	InputLayout::DestroyAll();

	for(int i=0; i<4; i++)
	{
		//delete [] wayLine[i];
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

	fx::InitAll(md3dDevice);
	InputLayout::InitAll(md3dDevice);
	GetTextureMgr().init(md3dDevice);

	buildFX();
	buildVertexLayouts();

	SetCursorPos(0,0);
	ShowCursor(false);
	initBasicVariables(); //Like shotTimer, etc.
	audio->playCue(INTROMUSIC);
	startScreen = true;
	level1 = false;
	level2 = true;
	position = D3DXVECTOR3(0, 5, 0); 
	initBasicGeometry();
	initTextStrings(); //Like start/end screen text
	initUniqueObjects(); //Like the floor

	initOrigin();
	initBullets();
	initPickups();
	initWallPositions();
	initBuildingPositions();
	initLights();
	initLamps();
	initEnemies();
	initHUD();

	menu.init(md3dDevice, 1.0f, WHITE);
	menu.setPosition(position);
	menu.setRotYAngle(ToRadian(90));
	menu.setRotZAngle(ToRadian(0));
	menu.setRotXAngle(ToRadian(-90));

	//mClearColor = D3DXCOLOR(0.15f, 0.15f, 0.15f, 1.0f);
	//mClearColor = D3DXCOLOR(0.529f, 0.808f, 0.98f, 1.0f);
	mClearColor = gameNS::DAY_SKY_COLOR;

	player.init(&mBox, pBullets, sqrt(2.0f), Vector3(3,4,0), Vector3(0,0,0), 200, audio, 1, 1, 1, 5);

	mWallMesh.init(md3dDevice, 1.0f, mFX);
	mBuildingMesh.init(md3dDevice, 1.0f, mFX);
	mEnemyMesh.init(md3dDevice, 1.0f, mFX);
	mPoleMesh.init(md3dDevice, 1.0f, mFX);
	mStreetMesh.init(md3dDevice, 1.0f, mFX);
	mBulletMesh.init(md3dDevice, 1.0f, mFX);
	mBlueMesh.init(md3dDevice, 1.0f, mFX);
	mRedMesh.init(md3dDevice, 1.0f, mFX);
	mYellowMesh.init(md3dDevice, 1.0f, mFX);

	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"bricks.png", 0, 0, &mDiffuseMapRV, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRV, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"skyscraper.jpg", 0, 0, &mDiffuseMapRVBuilding, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVBuilding, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"Robot.png", 0, 0, &mDiffuseMapRVEnemy, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVEnemy, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"pole.png", 0, 0, &mDiffuseMapRVPole, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVPole, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"street.png", 0, 0, &mDiffuseMapRVStreet, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVStreet, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"theroad.png", 0, 0, &mDiffuseMapRVTheRoad, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVTheRoad, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"building2.jpg", 0, 0, &mDiffuseMapRVBuilding2, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVBuilding2, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"bullet.png", 0, 0, &mDiffuseMapRVBullet, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVBullet, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"barrel.png", 0, 0, &mDiffuseMapRVBarrel, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVBarrel, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"blue.png", 0, 0, &mDiffuseMapRVBlue, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVBlue, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"red.png", 0, 0, &mDiffuseMapRVRed, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVRed, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"yellow.png", 0, 0, &mDiffuseMapRVYellow, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVYellow, 0 ));

	camera.init(input, position, Vector3(1, 0, 0), player.getPosition() + Vector3(1, 0, 0));

	vector<std::wstring> flares;
	flares.push_back(L"flare0.dds"); 
	ID3D10ShaderResourceView* texArray = GetTextureMgr().createTexArray(L"flares", flares);
 
	mFire[0].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 1500), &camera); 
	mFire[1].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 1475), &camera); 
	mFire[2].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 1300), &camera); 
	mFire[3].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 1275), &camera); 
	mFire[4].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 1100), &camera); 
	mFire[5].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 1075), &camera); 
	mFire[6].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 800), &camera); 
	mFire[7].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 775), &camera); 
	mFire[8].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 500), &camera); 
	mFire[9].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 475), &camera); 
	mFire[10].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 300), &camera); 
	mFire[11].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 275), &camera); 

	mFire[12].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -275), &camera); 
	mFire[13].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -300), &camera); 
	mFire[14].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -475), &camera); 
	mFire[15].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -500), &camera); 
	mFire[16].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -775), &camera); 
	mFire[17].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -800), &camera); 
	mFire[18].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -1075), &camera); 
	mFire[19].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -1100), &camera); 
	mFire[20].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -1275), &camera); 
	mFire[21].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -1300), &camera); 
	mFire[22].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -1475), &camera); 
	mFire[23].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -1500), &camera);
	//mFire.setEmitPos(D3DXVECTOR3(10, 10, 10));
}

void ColoredCubeApp::initLamps() {
	for (int i = 0; i < gameNS::NUM_LAMPS; i++)
		lamps.push_back(LampPost());
	
	lamps[0].init(&brick, Vector3(58.5,0.1,58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 2.3456f);
	lamps[1].init(&brick, Vector3(-58.5,0.1,-58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 5.49f);
	lamps[2].init(&brick, Vector3(58.5,0.1,-58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 3.9359f);
	lamps[3].init(&brick, Vector3(-58.5,0.1,58.5), 1.0f, 1.0f, 1, 1, 1, 0.0f, 0.7944f);
	lamps[4].init(&brick, Vector3(-100,0.1,100), 1.0f, 1.0f, 1, 1, 1, 0.0f, 6.3f);
	lamps[5].init(&brick, Vector3(100,0.1,100), 1.0f, 1.0f, 1, 1, 1, 0.0f, 3.2f);
	lamps[6].init(&brick, Vector3(-100,0.1,-100), 1.0f, 1.0f, 1, 1, 1, 0.0f, 6.3f);
	lamps[7].init(&brick, Vector3(100,0.1,-100), 1.0f, 1.0f, 1, 1, 1, 0.0f, 3.2f);
}

void ColoredCubeApp::initPickups() {
	//define the pickups

	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 0, ZIPPER, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 1, RELOAD, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 2, RELOAD, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 3, RELOAD, audio));
	dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 4, RELOAD, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 5, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 6, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 7, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 8, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 8, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 9, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 10, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 11, ZIPPER, audio));
	dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 12, ZIPPER, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 13, WHOOSH, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 14, WHOOSH, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 15, WHOOSH, audio));
	dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 16, WHOOSH, audio));

	
	nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 13, ZIPPER, audio));
	nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 14, ZIPPER, audio));
	nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 15, ZIPPER, audio));
	nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 16, ZIPPER, audio));
	nightPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 7, 13, WHOOSH, audio));
	nightPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 7, 14, WHOOSH, audio));
	nightPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 7, 15, WHOOSH, audio));
	nightPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 7, 16, WHOOSH, audio));
	nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 50, 13, RELOAD, audio));
	nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 50, 14, RELOAD, audio));
	nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 50, 15, RELOAD, audio));
	nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 50, 16, RELOAD, audio));

	for (int i = 0; i < dayPickups.size(); i++) {
	//	dayPickups[i].startGlowing();
		dayPickups[i].setInActive();
	}
	for (int i = 0; i < nightPickups.size(); i++) {
	//	nightPickups[i].startGlowing();
		nightPickups[i].setInActive();
	}



}

void ColoredCubeApp::initBullets() {
	for (int i = 0; i < gameNS::NUM_BULLETS; i++) {
		pBullets.push_back(new Bullet());
		pBullets[i]->init(&bulletBox, 2.0f, Vector3(0, 0, 0), Vector3(0,0,0), 0, 1);
	}
	
}

void ColoredCubeApp::initBasicGeometry() {	
	mBox.init(md3dDevice, 2.0f, D3DXCOLOR(0,0,0,0), mFX);
	whiteBox.init(md3dDevice, 1.0f, D3DXCOLOR(1,1,1,0), mFX);
	tealBox.init(md3dDevice, 1.0f, colorNS::TEAL, mFX);
	clearBox.init(md3dDevice, 1.0f, D3DXCOLOR(0,0,0,0), mFX);
	redBox.init(md3dDevice, 1.0f, colorNS::RED, mFX);
	brick.init(md3dDevice, 1.0f, DARKBROWN, mFX);
	bulletBox.init(md3dDevice, 0.25f, D3DXCOLOR(0,0,0,0), mFX);
	eBulletBox.init(md3dDevice, 0.5f, RED, mFX);
	maroonBox.init(md3dDevice, 10000, colorNS::MAROON, mFX);
	yellowGreenBox.init(md3dDevice, 1.f,D3DXCOLOR(0,0,0,0), mFX);
	goldBox.init(md3dDevice, 1.0f, YELLOW, mFX);
	blueBox.init(md3dDevice, 2.0f, BLUE, mFX);
	rLine.init(md3dDevice, 10.0f, RED);
	bLine.init(md3dDevice, 10.0f, BLACK);
	gLine.init(md3dDevice, 10.0f, GREEN);
	activeLine.init(md3dDevice, 5.0f, RED, mFX);
}

void ColoredCubeApp::initTextStrings() {
	

	sText.addLine("WELCOME RUGGER !", 290, 120);
	sText.addLine("Controls: ", 30, 210);
	sText.addLine("Movement: W A S D", 80, 240);
	sText.addLine("Shoot: Left-Click", 80, 265);
	sText.addLine("Flashlight: 'F' to toggle", 80, 290);
	sText.addLine("Objective:", 30, 360);
	sText.addLine("Collect items and shoot enemies to survive as long as you can!", 80, 390);
	sText.addLine("PRESS SPACE BAR TO BEGIN !", 250, 500);
	
	

	lText.addLine("TOO BAD, RUGGER, I WON!", 260, 180);
	lText.addLine("Press SPACEBAR to exit", 276, 500);

	wText.addLine("RUGGER YOU FIEND! YOU WON!", 250, 180);
	wText.addLine("THOSE MINIONS WERE EXPENSIVE!", 235, 250);
	wText.addLine("Press SPACEBAR to exit", 300, 500);
}

void ColoredCubeApp::initBasicVariables() {
	startScreen = true;
	shotTimer = 0;
	hasntPlayedYet = true;
	nightDayTrans = false;
	walking = false;

	score = 0;
	firstpass = true;
	startScreen = true;
	endScreen = false;
	night = false;
	found = false;
	timect = 0.0f;
	timeOfDay = "Day";
	srand(time(0));
	debugMode = false;
	nightCount = 0;
	stepTime = 0.0f;
	step1 = true;
	flashChanged = false;
	flashChangeTime = 0.0f;
	flashOn = false;
	placedPickups = false;
	dayCount = 1;
	won = false;
}

void ColoredCubeApp::initBuildingPositions() {
//					geom,  rad,  position,					sc,	w,		h,	d    //Level 1 Buildings
	buildings[0].init(&brick, 2.0f, Vector3(150, 0, -150),	1,	20,		50,  20);// Front right corner buildings
	buildings[1].init(&brick, 2.0f, Vector3(150, 0, -50),	1,	20,		50,  20);
	buildings[2].init(&brick, 2.0f, Vector3(50, 0, -150),	1,	20,		50,  20);

	buildings[3].init(&brick, 2.0f, Vector3(150, 0, 150),	1,	20,		50,  20);// Front left corner buildings
	buildings[4].init(&brick, 2.0f, Vector3(150, 0, 50),	1,	20,		50,  20);
	buildings[5].init(&brick, 2.0f, Vector3(50, 0, 150),	1,	20,		50,  20);

	buildings[6].init(&brick, 2.0f, Vector3(-150, 0, -150), 1,	20,		50,  20);// Back right corner buildings
	buildings[7].init(&brick, 2.0f, Vector3(-150, 0, -50),	1,	20,		50,  20);
	buildings[8].init(&brick, 2.0f, Vector3(-50, 0, -150),	1,	20,		50,  20);

	buildings[9].init(&brick, 2.0f, Vector3(-150, 0, 150),	1,	20,		50,  20);// Back left corner buildings
	buildings[10].init(&brick, 2.0f, Vector3(-150, 0, 50),	1,	20,		50,  20);
	buildings[11].init(&brick, 2.0f, Vector3(-50, 0, 150),	1,	20,		50,  20);

																				 //Level 2 Buildings
	buildings[12].init(&brick, 2.0f, Vector3(700, 0, 1300),1,	190,	50,  190);//Left Side Building 1
	buildings[13].init(&brick, 2.0f, Vector3(420, 0, 1020),1,	95,		50,  95);//Left Side Building 2
	buildings[14].init(&brick, 2.0f, Vector3(300, 0, 1350),1,	95,		50,  95);//Left Side Building 3
	buildings[15].init(&brick, 2.0f, Vector3(700, 0, 925),1,	95,		50,  95);//Left Side Building 4

	buildings[16].init(&brick, 2.0f, Vector3(300, 0, 700),	1,	95,		70,  120);//Left Side Building 5
	buildings[17].init(&brick, 2.0f, Vector3(700, 0, 500),	1,	120,	50,  95);//Left Side Building 6
	buildings[18].init(&brick, 2.0f, Vector3(700, 0, 150),	1,	120,	50,  108);//Left Side Building 7
	buildings[19].init(&brick, 2.0f, Vector3(350, 0, 250),	1,	72,		50,  120);//Left Side Building 8

	buildings[20].init(&brick, 2.0f, Vector3(800, 0, -200),	1,	95,		80,  95);//Left Side Building 9
	buildings[21].init(&brick, 2.0f, Vector3(375, 0, -300),	1,	95,		50,  95);//Left Side Building 10
	buildings[22].init(&brick, 2.0f, Vector3(850, 0, -550),	1,	95,		50,  95);//Left Side Building 11
	buildings[23].init(&brick, 2.0f, Vector3(350, 0, -650),	1,	130,	50,  95);//Left Side Building 12
	buildings[24].init(&brick, 2.0f, Vector3(800, 0, -1200),1,	120,	50,  250);//Left Side Building 13
	buildings[25].init(&brick, 2.0f, Vector3(350, 0, -1300),1,	72,		50,  72);//Left Side Building 14

	buildings[26].init(&brick, 2.0f, Vector3(-700, 0, 1350),1,	95,		50,  120);//Right Side Building 15
	buildings[27].init(&brick, 2.0f, Vector3(-300, 0, 1100),1,	110,	50,  130);//Right Side Building 16
	buildings[28].init(&brick, 2.0f, Vector3(-600, 0, 750),	1,	100,	50,  90);//Right Side Building 17
	buildings[29].init(&brick, 2.0f, Vector3(-300, 0, 550),	1,	50,		30,  150);//Right Side Building 18
	buildings[30].init(&brick, 2.0f, Vector3(-250, 0, 195),1,	95,		50,  95);//Right Side Building 19

	buildings[31].init(&brick, 2.0f, Vector3(-495, 0, -10),	1,	150,	50,  110);//Right Side Building 20
	buildings[32].init(&brick, 2.0f, Vector3(-250, 0, -215),1,	95,		50,  95);//Right Side Building 21
	buildings[33].init(&brick, 2.0f, Vector3(-700, 0, -275),1,	95,		50,  95);//Right Side Building 22
	buildings[34].init(&brick, 2.0f, Vector3(-250, 0, -700),1,	50,		90,  50);//Right Side Building 23

	buildings[35].init(&brick, 2.0f, Vector3(-650, 0, -800),	1,	110,	50,  200);//Right Side Building 24
	buildings[36].init(&brick, 2.0f, Vector3(-225, 0, -1000),1,	50,		60,  50);//Right Side Building 25
	buildings[37].init(&brick, 2.0f, Vector3(-650, 0, -1300),1,	110,	50,  200);//Right Side Building 26
	buildings[38].init(&brick, 2.0f, Vector3(-200, 0, -1300),1,	50,		30,  50);//Right Side Building 27

}

void ColoredCubeApp::initWallPositions() {
	
//				   geom,  rad,  position,								sc,	w,			h,	d
	walls[0].init(&brick, 2.0f, Vector3(125, 0, 250), Vector3(0,0,0),	1, 	1,	125,	10,		10);//	Left/Front wall 
	walls[1].init(&brick, 2.0f, Vector3(-125, 0, -250), Vector3(0,0,0), 1,	1,	125,	10,		10);//	Right/back wall
	walls[2].init(&brick, 2.0f, Vector3(250, 0, 125),	 Vector3(0,0,0), 1,	1,	10,		10,		125);//	Front/Left wall
	walls[3].init(&brick, 2.0f, Vector3(-250, 0, -125),	 Vector3(0,0,0), 1,	1,	10,		10,		125);//	Back/Right wall

	walls[4].init(&brick, 2.0f, Vector3(-125, 0, 250),	 Vector3(0,0,0), 1,	1,	125,	10,		10);//	Left/Back wall 
	walls[5].init(&brick, 2.0f, Vector3(125, 0, -250),	 Vector3(0,0,0), 1,	1,	125,	10,		10);//	Right/Front wall
	walls[6].init(&brick, 2.0f, Vector3(250, 0, -125),	 Vector3(0,0,0), 1,	1,	10,		10,		125);//	Front/Right wall
	walls[7].init(&brick, 2.0f, Vector3(-250, 0, 125),	 Vector3(0,0,0), 1,	1,	10,		10,		125);//	Back/Left wall

	walls[8].init(&brick, 2.0f, Vector3(36, 0, 55),		 Vector3(0,0,0), 1,	1,	20,		2.5,	1);//	Left/Front inner wall 
	walls[9].init(&brick, 2.0f, Vector3(-36, 0, -55),	 Vector3(0,0,0), 1,	1,	20,		2.5,	1);//	Right/Back inner wall
	walls[10].init(&brick, 2.0f, Vector3(55, 0, 36),	 Vector3(0,0,0), 1,	1,	1,		2.5,	20);//	Front/Left inner wall
	walls[11].init(&brick, 2.0f, Vector3(-55, 0, -36),	 Vector3(0,0,0), 1,	1,	1,		2.5,	20);//	Back/Right inner wall

	walls[12].init(&brick, 2.0f, Vector3(-36, 0, 55),	 Vector3(0,0,0), 1,	1,	20,		2.5,	1);//	Left/Back inner wall 
	walls[13].init(&brick, 2.0f, Vector3(36, 0, -55),	 Vector3(0,0,0), 1,	1,	20,		2.5,	1);//	Right/Front inner wall
	walls[14].init(&brick, 2.0f, Vector3(55, 0, -36),	 Vector3(0,0,0), 1,	1,	1,		2.5,	20);//	Front/Right inner wall
	walls[15].init(&brick, 2.0f, Vector3(-55, 0, 36),	 Vector3(0,0,0), 1,	1,	1,		2.5,	20);//	Back/Left inner wall

	walls[16].init(&brick, 2.0f, Vector3(0, 0, -1625),	 Vector3(0,0,0), 1,	1,	980,	20,	10);// Far Wall
	walls[17].init(&brick, 2.0f, Vector3(0, 0, 1625),	 Vector3(0,0,0), 1,	1,	980,	20,	10);// Back Wall
	walls[18].init(&brick, 2.0f, Vector3(-980, 0, 0),	 Vector3(0,0,0), 1,	1,	10,		20,	1625);// Back Wall
	walls[19].init(&brick, 2.0f, Vector3(980, 0, 0),	 Vector3(0,0,0), 1,	1,	10,		20,	1625);// Back Wall


}

void ColoredCubeApp::initUniqueObjects() {
	floor.init(&yellowGreenBox, 2.0f, Vector3(0,-1000.0f,0), Vector3(0,0,0), 1, 1.0f, 1625, 500, 1625);
	floor2.init(&yellowGreenBox, 2.0f, Vector3(0,-1000.0f,0), Vector3(0,0,0), 1, 1.0f, 975, 500, 1625);
	
	barrels[0].init(&brick, 2.0f, Vector3(-85, 0, 1500),	1,	1,		3.5,  1);
	barrels[1].init(&brick, 2.0f, Vector3(85, 0, 1475),		1,	1,		3.5,  1);
	barrels[2].init(&brick, 2.0f, Vector3(-85, 0, 1300),	1,	1,		3.5,  1);
	barrels[3].init(&brick, 2.0f, Vector3(85, 0, 1275),		1,	1,		3.5,  1);
	barrels[4].init(&brick, 2.0f, Vector3(-85, 0, 1100),	1,	1,		3.5,  1);
	barrels[5].init(&brick, 2.0f, Vector3(85, 0, 1075),		1,	1,		3.5,  1);
	barrels[6].init(&brick, 2.0f, Vector3(-85, 0, 800),		1,	1,		3.5,  1);
	barrels[7].init(&brick, 2.0f, Vector3(85, 0, 775),		1,	1,		3.5,  1);
	barrels[8].init(&brick, 2.0f, Vector3(-85, 0, 500),		1,	1,		3.5,  1);
	barrels[9].init(&brick, 2.0f, Vector3(85, 0, 475),		1,	1,		3.5,  1);
	barrels[10].init(&brick, 2.0f, Vector3(-85, 0, 300),	1,	1,		3.5,  1);
	barrels[11].init(&brick, 2.0f, Vector3(85, 0, 275),		1,	1,		3.5,  1);

	barrels[12].init(&brick, 2.0f, Vector3(-85, 0, -275),	1,	1,		3.5,  1);
	barrels[13].init(&brick, 2.0f, Vector3(85, 0, -300),	1,	1,		3.5,  1);
	barrels[14].init(&brick, 2.0f, Vector3(-85, 0, -475),	1,	1,		3.5,  1);
	barrels[15].init(&brick, 2.0f, Vector3(85, 0, -500),	1,	1,		3.5,  1);
	barrels[16].init(&brick, 2.0f, Vector3(-85, 0, -775),	1,	1,		3.5,  1);
	barrels[17].init(&brick, 2.0f, Vector3(85, 0, -800),	1,	1,		3.5,  1);
	barrels[18].init(&brick, 2.0f, Vector3(-85, 0, -1075),	1,	1,		3.5,  1);
	barrels[19].init(&brick, 2.0f, Vector3(85, 0, -1100),	1,	1,		3.5,  1);
	barrels[20].init(&brick, 2.0f, Vector3(-85, 0, -1275),	1,	1,		3.5,  1);
	barrels[21].init(&brick, 2.0f, Vector3(85, 0, -1300),	1,	1,		3.5,  1);
	barrels[22].init(&brick, 2.0f, Vector3(-85, 0, -1475),	1,	1,		3.5,  1);
	barrels[23].init(&brick, 2.0f, Vector3(85, 0, -1500),	1,	1,		3.5,  1);
}

void ColoredCubeApp::initEnemies() {
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++) {
		enemy[i].init(&mBox, 2.0f, Vector3(rand()%50,0,rand()%50), Vector3(0,0,0), 1, 1, 1, 2, 1);
		enemy[i].faceObject(&player);
	}
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

void ColoredCubeApp::initLights()
{
	mLightType = 1;
	mLightNum = gameNS::NUM_LIGHTS;
 
	// Parallel light.
	mLights[0].dir      = D3DXVECTOR3(0.57735f, -0.57735f, 0.57735f);
	mLights[0].ambient  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	//mLights[0].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLights[0].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[0].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLights[0].specular = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
	//mLights[0].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[0].pos = D3DXVECTOR3(0, 500, 0);
	mLights[0].range = 1000;
	mLights[0].att.x = 0.0f;
	mLights[0].att.y = 0.01f;
	mLights[0].att.z = 0.0f;
 
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
	mLights[2].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//mLights[2].ambient  = D3DXCOLOR(0.0f, 0.0f, 1.0f, 1.0f);
	mLights[2].diffuse  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	//mLights[2].diffuse  = D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f);
	mLights[2].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	//mLights[2].specular = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
	mLights[2].att.x    = 1.0f;
	mLights[2].att.y    = 0.0f;
	mLights[2].att.z    = 0.0f;
	mLights[2].spotPow  = 128.0f;
	mLights[2].range    = 0.0f;

	//Inner corner lights
	mLights[3].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[3].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[3].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[3].att.x    = 0.0f;
	mLights[3].att.y    = 0.55f;
	mLights[3].att.z    = 0.0f;
	mLights[3].range    = 90.0f;
	mLights[3].pos = D3DXVECTOR3(30, 10, 30);
	
	mLights[4].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[4].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[4].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[4].att.x    = 0.0f;
	mLights[4].att.y    = 0.55f;
	mLights[4].att.z    = 0.0f;
	mLights[4].range    = 90.0f;
	mLights[4].pos = D3DXVECTOR3(-30, 10, 30);

	mLights[5].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[5].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[5].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[5].att.x    = 0.0f;
	mLights[5].att.y    = 0.55f;
	mLights[5].att.z    = 0.0f;
	mLights[5].range    = 90.0f;
	mLights[5].pos = D3DXVECTOR3(30, 10, -30);

	mLights[6].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[6].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
	mLights[6].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
	mLights[6].att.x    = 0.0f;
	mLights[6].att.y    = 0.55f;
	mLights[6].att.z    = 0.0f;
	mLights[6].range    = 90.0f;
	mLights[6].pos = D3DXVECTOR3(-30, 10, -30);

	//Enemy entry vectors
	mLights[7].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[7].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[7].specular = D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f);
	mLights[7].att.x    = 0.0f;
	mLights[7].att.y    = 0.45f;
	mLights[7].att.z    = 0.0f;
	mLights[7].range    = 35.0f;
	mLights[7].pos = D3DXVECTOR3(200, 10, 0);

	mLights[8].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[8].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[8].specular = D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f);
	mLights[8].att.x    = 0.0f;
	mLights[8].att.y    = 0.45f;
	mLights[8].att.z    = 0.0f;
	mLights[8].range    = 35.0f;
	mLights[8].pos = D3DXVECTOR3(-200, 10, 0);

	mLights[9].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[9].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[9].specular = D3DXCOLOR(1.0f, 0.5f, 0.0f, 1.0f);
	mLights[9].att.x    = 0.0f;
	mLights[9].att.y    = 0.45f;
	mLights[9].att.z    = 0.0f;
	mLights[9].range    = 35.0f;
	mLights[9].pos = D3DXVECTOR3(0, 10, -200);

	mLights[10].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
	mLights[10].diffuse  = D3DXCOLOR(0.9f, 0.5f, 0.5f, 1.0f);
	mLights[10].specular = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	mLights[10].att.x    = 0.0f;
	mLights[10].att.y    = 0.45f;
	mLights[10].att.z    = 0.0f;
	mLights[10].range    = 35.0f;
	mLights[10].pos = D3DXVECTOR3(0, 10, 200);
}

void ColoredCubeApp::initHUD() {
	hudObjects.push_back(HudObject());

	hudObjects[0].init(&blueBox, 1.0f, Vector3(0,0,0), Vector3(0,0,0), 1, 1);

	for (int i = 0; i < hudObjects.size(); i++) {
		hudObjects[i].faceObject(&player);
		hudObjects[i].setActive();
	}
}




void ColoredCubeApp::updateScene(float dt)
{
	ColoredCubeApp::dt = dt;
	gameTime += dt;
	bool playing = (!endScreen && !startScreen);
	Vector3 oldPos = position;
	
	//Restricting the mouse movement
	RECT restrict = {639, 399, 640, 400};
	ClipCursor(&restrict);

	if(input->isKeyDown(VK_ESCAPE)) PostQuitMessage(0);

	if (startScreen){
		if(input->isKeyDown(VK_SPACE)){
			startScreen = false;
			playing = true;
			audio->stopCue(INTROMUSIC);
		}
	}
	if(playing){
		timect += dt;
		D3DApp::updateScene(dt);
		menu.update(dt);
		updateDebugMode();
		updateMusic();
		menu.update(dt);
		updateDayNight();
		camera.update(dt, gameNS::PLAYER_SPEED);
		player.setPosition(camera.getPosition());
		updateOrigin(dt);
		handleUserInput();
		updatePlayer(dt);
		updateEnemies(dt);
		updatePickups(dt);
		updateLamps(dt);
		updateWalls(dt);
		updateBuildings(dt);
		updateUniqueObjects(dt);
		updateHUD(dt);


		if(level1)
		{
			placePickups();

		}
		else if(level2)
		{

		}

		//Handle Collisions
		handleWallCollisions(oldPos);
		handleBuildingCollisions(oldPos);
		handlePickupCollisions(dt);
		handleEnemyCollisions(dt);

		for(int i=0; i<gameNS::NUM_FIRES; i++)
			mFire[i].update(dt, gameTime);
	}
	if(endScreen){
		doEndScreen();
	}
	
	// The spotlight takes on the camera position and is aimed in the
	// same direction the camera is looking.  In this way, it looks
	// like we are holding a flashlight.
	mLights[2].pos = position;
	D3DXVec3Normalize(&mLights[2].dir, &(lookAt-position));
}

void ColoredCubeApp::updateMusic() {
	if (hasntPlayedYet || nightDayTrans) {
		hasntPlayedYet = false;
		nightDayTrans = false;
		if (gameNS::PLAY_MUSIC) audio->playCue(MUSIC);
	}
}

void ColoredCubeApp::updateDebugMode() {
	if(input->wasKeyPressed(KEY_K)) {
		debugMode = true;
		input->clear(KEY_K);
	} 
	if (input->wasKeyPressed(KEY_L)) {
		position = D3DXVECTOR3(position.x, 5, position.z);
		debugMode = false;
		input->clear(KEY_L);
	}
	if (input->wasKeyPressed(KEY_M)) {
		gameNS::PLAY_MUSIC = false;
		audio->stopCue(MUSIC);
	}
}

void ColoredCubeApp::updateCamera() {
	int dx = input->getMouseRawX();
	int dy = input->getMouseRawY();
	float _speed = 6.0;

	D3DXVECTOR3 transformRef(1, 0, 0); 
	D3DXVECTOR3 up(0.0f, 1.0f, 0.0f);
	Vector3 direction = Vector3(0,0,0);

	bool yawUpdate = (dx != 0)?true:false;
	bool pitchUpdate = (dy != 0)?true:false;
	
	if(dx < 0) yaw -= _speed*dt;
	if(dx > 0) yaw += _speed*dt;
	if(dy < 0) pitch += (_speed/2)*dt;
	if(dy > 0) pitch -= (_speed/2)*dt;
	// Restrict the angle pitch and radius mRadius.
	if( pitch < -(PI/2.0f) + 0.01f)		pitch = -(PI/2.0f) + 0.01f;
	if( pitch > PI/2.0f - 0.01f)		pitch = (PI/2.0f) - 0.01f;

	walking = false;
	if(GetAsyncKeyState('A') & 0x8000) direction.z = 1;
	if(GetAsyncKeyState('D') & 0x8000) direction.z = -1;
	if(GetAsyncKeyState('S') & 0x8000) {direction.x = -1; moveAxis.y = 0;}
	if(GetAsyncKeyState('W') & 0x8000) {direction.x = 1;  moveAxis.y = 0;}

	//Generate transformation matrices
	Matrix yawR = *D3DXMatrixRotationY(&yawR, yaw);
	Matrix pitchR = *D3DXMatrixRotationZ(&pitchR, pitch);
	Matrix temp = pitchR * yawR; 

	if (direction != Vector3(0,0,0)) {
		walking = true;
		Transform(&direction, &direction, &yawR);
		position += direction * player.getSpeed() * dt;
	}
	
	Transform(&transformRef, &transformRef, &temp);
	Normalize(&transformRef, &transformRef);
	lookAt = transformRef * player.getSpeed() * dt;
	lookAt += position;

	D3DXMatrixLookAtLH(&mView, &position, &lookAt, &up);
}

void ColoredCubeApp::doEndScreen() {
	if(input->isKeyDown(VK_SPACE))
	{
		endScreen = false;
		PostQuitMessage(0);
	}
}

void ColoredCubeApp::updateUniqueObjects(float dt) {
	floor.update(dt);
	floor2.update(dt);
	//big floor update must also be added here for it to show.

	for(int i=0; i <gameNS::NUM_BARRELS; i++)
		barrels[i].update(dt);
}

void ColoredCubeApp::updateWalls(float dt) {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
		walls[i].update(dt);
}

void ColoredCubeApp::updateLamps(float dt) {
	for (int i = 0; i < gameNS::NUM_LAMPS; i++) {
		lamps[i].update(dt);
	}
}

void ColoredCubeApp::updateBuildings(float dt) {
	for(int i=0; i<gameNS::NUM_BUILDINGS; i++)
		buildings[i].update(dt);
}

void ColoredCubeApp::updatePlayer(float dt) {
	
	D3DXVECTOR3 pos = player.getPosition();
	//player.setPosition(Vector3(position.x, position.y-2, position.z));
	player.setPosition(camera.getPosition());
	player.update(dt, camera.getLookatDirection()); //moveAxis is passed to the bullet
	if (player.getHealth() <= 0) {
		Sleep(2000);
		endScreen = true;
		input->clearKeyPress(KEY_SPACE);
	}


	//Update shooting
	if(input->getMouseLButton())
	{
		if(!player.firedLastFrame){
			player.fired = true;
			if(player.getAmmo() > 0) audio->playCue(FIRE);
			//else audio->playCue(OUT_OF_AMMO);
		}
		player.firedLastFrame = true; 
	} else {
		player.firedLastFrame = false;
		player.fired = false;
	}
	if(input->getMouseRButton())
	{
		//Do something
	}


	
	//Update walking noises
	if (walking) {
		stepTime += 1;
		if (stepTime*dt > gameNS::FOOTSTEP_GAP) {
			if (pos.x < gameNS::GRASSY_AREA_WIDTH/2.0f && pos.x > -gameNS::GRASSY_AREA_WIDTH/2.0f  && pos.z > -gameNS::GRASSY_AREA_WIDTH/2.0f && pos.z < gameNS::GRASSY_AREA_WIDTH/2.0f) { //in grassy area 
				if (step1) audio->playCue(FOOTSTEP3);
				else audio->playCue(FOOTSTEP4);
			} else {
				if (step1) audio->playCue(FOOTSTEP1);
				else audio->playCue(FOOTSTEP2);
			}
			step1 = !step1;
			stepTime = 0.0f;
		}
	}


	//update camera a bit
	if (debugMode) { //Allow flying with space and shift
		if(input->isKeyDown(VK_SPACE)) camera.flying(true);
		else camera.flying(false);
		if(input->isKeyDown(VK_SHIFT)) camera.falling(true);
		else camera.falling(false);
	}
}

void ColoredCubeApp::updateEnemies(float dt)
{
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)
	{
		if(enemy[i].getActiveState())
		{
			if(night)
			{
				if(D3DXVec3LengthSq(&(enemy[i].getPosition() - D3DXVECTOR3(0,0,0))) < 55*55)
				{
					enemy[i].setSpeed(enemyNS::DAY_SPEED);
				}
				else enemy[i].setSpeed(enemyNS::NIGHT_SPEED);
			}
			else enemy[i].setSpeed(enemyNS::DAY_SPEED);		
			
			enemy[i].update(dt, &player);
		}
	}

	vector<D3DXVECTOR3> wp = enemy[0].waypointPositions();
	
	for(int i=0; i<WAYPOINT_SIZE*WAYPOINT_SIZE; i++)
	{
		wayLine[i].init(&activeLine, 1.0f, wp[i], D3DXVECTOR3(0,0,0), 0.0f, 1.0f);
		wayLine[i].update(dt);
	}
}

void ColoredCubeApp::handleUserInput() {

	if (flashChanged)
		flashChangeTime++;

	if (flashChangeTime*dt >= 0.30f) {
		flashChangeTime = 0;
		flashChanged = false;
	}
	
	if (input->isKeyDown(KEY_F) && flashChanged == false) {
		flashChanged = true;	
		flashOn = !flashOn;
		if (flashOn)
			mLights[gameNS::FLASHLIGHT_NUM].range = 100;
		else 
			mLights[gameNS::FLASHLIGHT_NUM].range = 0;
	}
}

void ColoredCubeApp::handleWallCollisions(Vector3 pos) {
	for(int i=0; i<gameNS::NUM_WALLS; i++)
	{
		if(player.collided(&walls[i]))
		{
			position = pos;
		}

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

void ColoredCubeApp::handleBuildingCollisions(Vector3 pos) {
	for(int i=0; i<gameNS::NUM_BUILDINGS; i++)
	{
		if (buildings[i].getActiveState() == false) continue;
		if(player.collided(&buildings[i]))
			position = pos;
		for (int j = 0; j < pBullets.size(); j++) {
			if (pBullets[j]->collided(&buildings[i])) {
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
			}		
		}
	}
}

void ColoredCubeApp::handleLampCollisions(Vector3 pos) {
		for(int i=0; i<gameNS::NUM_WALLS; i++)
	{
		if(player.collided(&lamps[i]))
			player.setPosition(pos);

		for (int j = 0; j < pBullets.size(); j++) {
			if (pBullets[j]->collided(&lamps[i])) {
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
				player.fired = false;
			}		
		}
	}
}

void ColoredCubeApp::handlePickupCollisions(float dt) {
	
}

void ColoredCubeApp::handleEnemyCollisions(float dt)
{
	
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)
	{
		for(int j=0; j<pBullets.size(); j++)
		{
			if(pBullets[j]->collided(&enemy[i]))
			{
				pBullets[j]->setInActive();
				pBullets[j]->setVelocity(D3DXVECTOR3(0,0,0));
				pBullets[j]->setPosition(D3DXVECTOR3(0,0,0));
				shotTimer = 0;
				enemy[i].damage(50);
			}
		}
		for(int j=0; j<gameNS::NUM_WALLS; j++)
		{
			if(enemy[i].collided(&walls[j]) && D3DXVec3LengthSq(&(enemy[i].getPosition() - player.getPosition())) < 55*55)
			{
				enemy[i].setPosition(enemy[i].getOldPos());
			}
		}
		for(int j=0; j<gameNS::NUM_BUILDINGS; j++)
		{
			if(enemy[i].collided(&buildings[j])) enemy[i].setPosition(enemy[i].getOldPos());
		}
		//for(int j=0; j<gameNS::MAX_NUM_ENEMIES; j++)
		//{
		//	if(enemy[j].getActiveState())
		//		if(enemy[i].collided(&enemy[j])) enemy[i].setPosition(enemy[i].getOldPos());
		//}
	}
}

void ColoredCubeApp::placePickups() {
	if (!placedPickups) {
		int maxNightPickups = 5; //Locations: 0, 13, 14, 15, 16
		int maxDayPickups = 17; //All locations
		vector<int> choices;
		bool day = !night;
		if (day) {
			if (dayPickups.size() > 0) { //otherwise divide by zero when I mod by size
				vector<int> tempUsedIndices;
				for (int i = 0; i < maxDayPickups; i++) {
					bool add = true;
					int choice = rand()%dayPickups.size();
					for (int j = 0; j < tempUsedIndices.size(); j++) { //check that chosen dayPickup mapIndex isn't in the usedMapIndices
						if (tempUsedIndices[j] == dayPickups[choice].getMapIndex())
							add = false; //there is already a pickup in the spot of the chosen day pickup
					}
					if (add) {
						choices.push_back(choice); //add that to displayed pickups
						tempUsedIndices.push_back(dayPickups[choice].getMapIndex()); //record that mapIndex as used
					}
				}
			}
		} else {
			if (nightPickups.size() > 0) {
				vector<int> tempUsedIndices;
				for (int i = 0; i < maxNightPickups; i++) {
					bool add = true;
					int choice = rand()%nightPickups.size();
					for (int j = 0; j < tempUsedIndices.size(); j++) { //check that chosen dayPickup mapIndex isn't in the usedMapIndices
						if (tempUsedIndices[j] == nightPickups[choice].getMapIndex())
							add = false; //there is already a pickup in the spot of the chosen night pickup
					}
					if (add) {
							choices.push_back(choice); //add that to displayed pickups
							tempUsedIndices.push_back(nightPickups[choice].getMapIndex()); //record that mapIndex as used
					}
				}
			}
		}
	
		for (int i = 0; i < nightPickups.size(); i++)
			nightPickups[i].setInActive();
		for (int i = 0; i < dayPickups.size(); i++)
			dayPickups[i].setInActive();

		if (day)
			for (int i = 0; i < choices.size(); i++) 
				dayPickups[choices[i]].setActive();
		else if (night) 
			for (int i = 0; i < choices.size(); i++) 
				nightPickups[choices[i]].setActive();

		placedPickups = true;
	}
}

void ColoredCubeApp::updatePickups(float dt) {
	for (int i = 0; i < dayPickups.size(); i++) {
			if (player.collided(&dayPickups[i])) {
				dayPickups[i].activate();
			}
			dayPickups[i].update(dt);
	}
	for (int i = 0; i < nightPickups.size(); i++) {
			if (player.collided(&nightPickups[i])) {
				nightPickups[i].activate();
			}
			nightPickups[i].update(dt);
	}
}

void ColoredCubeApp::updateOrigin(float dt) {
	xLine.update(dt);
	yLine.update(dt);
	zLine.update(dt);
}

void ColoredCubeApp::updateDayNight() {
	if(timect >= gameNS::DAYLEN)
	{
		timect = 0;
		night = !night;
		if(night)
		{
			if(timeOfDay == "Evening")
			{
				nightCount++;
				placedPickups = false;

				int x = 0;
				for(int i=0; i<gameNS::MAX_NUM_ENEMIES && x < 4*nightCount; i++)
				{
					if(!enemy[i].getActiveState())
					{
						enemy[i].setActive();
						enemy[i].setHealth(100);
						switch(x%4)
						{
						case 0:
							enemy[i].setPosition(D3DXVECTOR3(50-rand()%100,0,-250));
							break;
						case 1:
							enemy[i].setPosition(D3DXVECTOR3(50-rand()%100,0,250));
							break;
						case 2:
							enemy[i].setPosition(D3DXVECTOR3(250, 0, 50-rand()%100));
							break;
						case 3:
							enemy[i].setPosition(D3DXVECTOR3(-250, 0, 50-rand()%100));
							break;
						}
						x++;
					}
				}

			}
			timeOfDay = "Night";
			mClearColor = gameNS::NIGHT_SKY_COLOR;
			mLights[0].diffuse  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
			mLights[3].att.y    = 0.05f;
			mLights[4].att.y    = 0.05f;
			mLights[5].att.y    = 0.05f;
			mLights[6].att.y    = 0.05f;
		}
		else
		{
			if (timeOfDay == "Dawn")
			{
				placedPickups = false;
				nightDayTrans = true;
			}
			timeOfDay = "Day";
			mClearColor = gameNS::DAY_SKY_COLOR;
			mLights[0].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			mLights[3].att.y    = 0.55f;
			mLights[4].att.y    = 0.55f;
			mLights[5].att.y    = 0.55f;
			mLights[6].att.y    = 0.55f;
			dayCount++;
		}
	}
	if(timect >= gameNS::DAYLEN - gameNS::TRANSITIONTIME)
	{
		//mLights[0].diffuse  = D3DXCOLOR(0.45f, 0.45f, 0.45f, 1.0f);
		if(night) 
		{
			timeOfDay = "Dawn";
			mClearColor += D3DXCOLOR(((gameNS::DAY_SKY_COLOR.r-gameNS::NIGHT_SKY_COLOR.r)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.g-gameNS::NIGHT_SKY_COLOR.g)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.b-gameNS::NIGHT_SKY_COLOR.b)/(gameNS::TRANSITIONTIME))*dt, 1.0f);
			//mClearColor += (gameNS::DAY_SKY_COLOR - gameNS::NIGHT_SKY_COLOR)/((gameNS::TRANSITIONTIME)*dt);
			mLights[0].diffuse += D3DXCOLOR(((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, 0.0f);
			mLights[3].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[4].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[5].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[6].att.y    += ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
		}
		else 
		{
			timeOfDay = "Evening";
			mClearColor -= D3DXCOLOR(((gameNS::DAY_SKY_COLOR.r-gameNS::NIGHT_SKY_COLOR.r)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.g-gameNS::NIGHT_SKY_COLOR.g)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.b-gameNS::NIGHT_SKY_COLOR.b)/(gameNS::TRANSITIONTIME))*dt, 1.0f);
			//mClearColor -= (gameNS::DAY_SKY_COLOR - gameNS::NIGHT_SKY_COLOR)/((gameNS::TRANSITIONTIME)*dt);
			mLights[0].diffuse -= D3DXCOLOR(((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, 0.0f);
			mLights[3].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[4].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[5].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			mLights[6].att.y    -= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
		}
	}
	
}

void ColoredCubeApp::updateHUD(float dt) {
	for (int i = 0; i < hudObjects.size(); i++) 
		hudObjects[i].update(dt);
}


void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();
	incrementedYMargin = 5;
	lineHeight = 20;
	bool playing = (!startScreen && !endScreen);

	setDeviceAndShaderInformation();

	if(playing) {	
		if(debugMode) for(int i=0; i<WAYPOINT_SIZE*WAYPOINT_SIZE; i++) wayLine[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)enemy[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);

		if(level1)
		{
			//mVP = mView*mProj;
			mVP = camera.getViewMatrix()*camera.getProjectionMatrix();

			mfxDiffuseMapVar->SetResource(mDiffuseMapRVEnemy);
			mfxSpecMapVar->SetResource(mSpecMapRVEnemy);
			
			
			//for(int i=0; i<gameNS::WAYPT_SIZE; i++) for(int j=0; j<gameNS::WAYPT_SIZE; j++) if(waypoints[i][j]->isActive())wayLine[i][j].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
			//drawOrigin();
			mfxDiffuseMapVar->SetResource(mDiffuseMapRVStreet);
			mfxSpecMapVar->SetResource(mSpecMapRVStreet);
			floor.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
			drawWalls();
			drawBuildings();
			drawPickups();
			drawLamps();

			D3D10_TECHNIQUE_DESC techDesc;
			mTech->GetDesc( &techDesc );
			for(UINT p = 0; p < techDesc.Passes; ++p)
			{
				mWallMesh.draw();
			}

			Matrix mWVP = menu.getWorld() * (mVP);
			mfxWVPVar->SetMatrix((float*)&mWVP);
			mfxWorldVar->SetMatrix((float*)&menu.getWorld());
			//D3D10_TECHNIQUE_DESC techDesc;
			mTech->GetDesc( &techDesc );
			for(UINT p = 0; p < techDesc.Passes; ++p)
			{
				mTech->GetPassByIndex( p )->Apply(0);
				//menu.draw();
			}

			mfxDiffuseMapVar->SetResource(mDiffuseMapRVBullet);
			mfxSpecMapVar->SetResource(mSpecMapRVBullet);
			player.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
			printText("Score: ", 20, 5, 0, 0, WHITE, player.getScore()); //This has to be the last thing in the draw function.
			printText("Health: ", 20, 25, 0, 0, RED, player.getHealth());
			printText("Ammo: ", 20, 45, 0, 0, BLUE, player.getAmmo());
			printText(timeOfDay + " ", 670, 20, 0, 0, WHITE, dayCount);
		}
		else if(level2)
		{
			drawHUD();

			mVP = camera.getViewMatrix()*camera.getProjectionMatrix();

			mfxDiffuseMapVar->SetResource(mDiffuseMapRVTheRoad);
			mfxSpecMapVar->SetResource(mSpecMapRVTheRoad);
			floor2.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
			mfxDiffuseMapVar->SetResource(mDiffuseMapRVBuilding2);
			mfxSpecMapVar->SetResource(mSpecMapRVBuilding2);
			drawBuildings();
			drawWalls();
			drawBarrels();
			drawLamps();
			//Draw particle systems last
			md3dDevice->OMSetDepthStencilState(0, 0);
			float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
			md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff);
		
			for(int i=0; i<gameNS::NUM_FIRES; i++)
			{
				mFire[i].setEyePos(camera.getPosition());
				mFire[i].draw();
			}
		}
	}
	else if(startScreen)
	{
		D3D10_TECHNIQUE_DESC techDesc;
		mTech->GetDesc( &techDesc );
		for(UINT p = 0; p < techDesc.Passes; ++p)
		{
			menu.draw();
		}
		printText(sText);
	}
	else { // End Screen 
		if(!won){
			printText(lText);
			printText("Score: ", 350, 280, 0, 0, WHITE, player.getScore());
		}
		else {
			printText(wText);
			printText("Score: ", 350, 280, 0, 0, WHITE, player.getScore());
		}
	}
	
	// We specify DT_NOCLIP, so we do not care about width/height of the rect.
	RECT R = {5, 5, 0, 0};
	md3dDevice->RSSetState(0);
	mFont->DrawText(0, mFrameStats.c_str(), -1, &R, DT_NOCLIP, BLACK);

	mSwapChain->Present(0, 0); //Comment this out for expert mode
}

void ColoredCubeApp::drawWalls() {
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);
	if(level1)
		for(int i=0; i<17; i++)
			walls[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	else if(level2)
		for(int i=16; i<gameNS::NUM_WALLS; i++)
			walls[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}

void ColoredCubeApp::drawBuildings() {
	if(level1){
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVBuilding);
		mfxSpecMapVar->SetResource(mSpecMapRVBuilding);
		for(int i=0; i<13; i++)
			buildings[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
	else if(level2){
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVBuilding2);
		mfxSpecMapVar->SetResource(mSpecMapRVBuilding2);
		for(int i=12; i<gameNS::NUM_BUILDINGS; i++)
			buildings[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
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
			printText(text.lines[i].s, xMargin, yMargin, 0, 0, WHITE);
		}
}

void ColoredCubeApp::printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, D3DXCOLOR color, int value) {
	
	RECT POS = {rectPosX, rectPosY, rectWidth, rectHeight};
	std::wostringstream outs;   
	outs.precision(6);
	if (value != -1)
		outs << text.c_str() << value;
	else 
		outs << text.c_str();
	std::wstring sc = outs.str();
	mFont->DrawText(0, sc.c_str(), -1, &POS, DT_NOCLIP, color);
}

void ColoredCubeApp::drawLine(LineObject* line) {
	//mWVP = line->getWorldMatrix()*mView*mProj;
	mWVP = line->getWorldMatrix()*camera.getViewMatrix()*camera.getProjectionMatrix();
	mfxWVPVar->SetMatrix((float*)&mWVP);
	line->setMTech(mTech);
	line->draw();
}

void ColoredCubeApp::buildFX()
{
		DWORD shaderFlags = D3D10_SHADER_ENABLE_STRICTNESS;
//#if defined( DEBUG ) || defined( _DEBUG )
//    shaderFlags |= D3D10_SHADER_DEBUG;
//	shaderFlags |= D3D10_SHADER_SKIP_OPTIMIZATION;
//#endif

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
	
	mfxWVPVar		= mFX->GetVariableByName("gWVP")->AsMatrix();
	mfxWorldVar		= mFX->GetVariableByName("gWorld")->AsMatrix();
	mfxEyePosVar	= mFX->GetVariableByName("gEyePosW");
	mfxLightVar		= mFX->GetVariableByName("gLight");
	mfxLightType	= mFX->GetVariableByName("gLightType")->AsScalar();
	mfxDiffuseMapVar= mFX->GetVariableByName("gDiffuseMap")->AsShaderResource();
	mfxSpecMapVar	= mFX->GetVariableByName("gSpecMap")->AsShaderResource();
	mfxTexMtxVar	= mFX->GetVariableByName("gTexMtx")->AsMatrix();
	mfxLightNum		= mFX->GetVariableByName("gLightNum")->AsScalar();
}

void ColoredCubeApp::buildVertexLayouts()
{
	// Create the vertex input layout.
	D3D10_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,  D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"DIFFUSE",  0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 24, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"SPECULAR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 40, D3D10_INPUT_PER_VERTEX_DATA, 0},
		{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 56, D3D10_INPUT_PER_VERTEX_DATA, 0},
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
	mfxEyePosVar->SetRawValue(&position, 0, sizeof(D3DXVECTOR3));
	mfxLightVar->SetRawValue(&mLights, 0, mLightNum*sizeof(Light));
	mfxLightType->SetInt(mLightType);
	mfxWVPVar->SetMatrix((float*)&mWVP);
	mfxWorldVar->SetMatrix((float*)&mCompCubeWorld);

	mfxLightNum->SetInt(mLightNum);
	D3DXMATRIX tm;
	Identity(&tm);
	mfxTexMtxVar->SetMatrix((float*)&tm);
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
	D3DXMatrixPerspectiveFovLH(&mProj, 0.25f*PI, aspect, 1.0f, gameNS::FAR_CLIP);
}

void ColoredCubeApp::drawOrigin() {
	//draw the lines
	drawLine(&xLine);
	drawLine(&yLine);
	drawLine(&zLine);
}

void ColoredCubeApp::drawPickups() {
	//Set mVP to be view*projection, so we can pass that into GO::draw(..)
	
	for (int i = 0; i < dayPickups.size(); i++)

		if (dayPickups[i].getActiveState()) {
			if(dayPickups[i].getColor() == Vector3(RED.r, RED.g, RED.b)){
				mfxDiffuseMapVar->SetResource(mDiffuseMapRVRed);
				mfxSpecMapVar->SetResource(mSpecMapRVRed);
			}
			else if(dayPickups[i].getColor() == Vector3(BLUE.r, BLUE.g, BLUE.b)){
				mfxDiffuseMapVar->SetResource(mDiffuseMapRVBlue);
				mfxSpecMapVar->SetResource(mSpecMapRVBlue);
			}
			else if(dayPickups[i].getColor() == Vector3(YELLOW.r, YELLOW.g, YELLOW.b)){
				mfxDiffuseMapVar->SetResource(mDiffuseMapRVYellow);
				mfxSpecMapVar->SetResource(mSpecMapRVYellow);
			}
			dayPickups[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		}	
	for (int i = 0; i < nightPickups.size(); i++) 
		if (nightPickups[i].getActiveState()) {
			if(nightPickups[i].getColor() == Vector3(RED.r, RED.g, RED.b)){
				mfxDiffuseMapVar->SetResource(mDiffuseMapRVRed);
				mfxSpecMapVar->SetResource(mSpecMapRVRed);
			}
			else if(nightPickups[i].getColor() == Vector3(BLUE.r, BLUE.g, BLUE.b)){
				mfxDiffuseMapVar->SetResource(mDiffuseMapRVBlue);
				mfxSpecMapVar->SetResource(mSpecMapRVBlue);
			}
			else if(nightPickups[i].getColor() == Vector3(YELLOW.r, YELLOW.g, YELLOW.b)){
				mfxDiffuseMapVar->SetResource(mDiffuseMapRVYellow);
				mfxSpecMapVar->SetResource(mSpecMapRVYellow);
			}
			nightPickups[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		}

}

void ColoredCubeApp::drawLamps() {
	mfxDiffuseMapVar->SetResource(mDiffuseMapRVPole);
	mfxSpecMapVar->SetResource(mSpecMapRVPole);
	if(level1)
	{
		for (int i = 0; i < 4; i++)
		lamps[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
	else if(level2)
	{
		for (int i = 4; i < 8; i++)
		lamps[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
}

void ColoredCubeApp::drawHUD() {
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);
	
	for(int i=0; i<hudObjects.size(); i++)
		hudObjects[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}

void ColoredCubeApp::drawBarrels()
{
	mfxDiffuseMapVar->SetResource(mDiffuseMapRVBarrel);
	mfxSpecMapVar->SetResource(mSpecMapRVBarrel);
	for(int i = 0; i < gameNS::NUM_BARRELS; i++)
		barrels[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);

}