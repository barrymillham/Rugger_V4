//=============================================================================
// Color Cube App.cpp by Frank Luna (C) 2008 All Rights Reserved.
//
// Demonstrates coloring.
//
// Controls:
//		'A'/'D'/'W'/'S' - Rotate 
//=============================================================================


/*
Things to reinit
Lamps
pickups
Walls
Buildings

*/

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

using std::string;
using std::time;

namespace gameNS {
	const float DAYLEN = 40;
	const int NUM_WALLS = 28;
	const int NUM_BUILDINGS = 39;
	const int NUM_BARRELS = 24;
	const int PERIMETER = 4;
	const int NUM_BULLETS = 100;
	const int NUM_LIGHTS = 15;
	const int NUM_FIRES = 12;
	const float TRANSITIONTIME = 10;
	const D3DXCOLOR NIGHT_SKY_COLOR = D3DXCOLOR(0.049f, 0.049f, 0.2195f, 1.0f);
	const D3DXCOLOR DAY_SKY_COLOR = D3DXCOLOR(0.529f, 0.808f, 0.98f, 1.0f);
	const int MAX_NUM_ENEMIES = 20;
	bool PLAY_MUSIC = true;
	const float FOOTSTEP_GAP = 0.45f;
	int GRASSY_AREA_WIDTH = 110;
	const int FLASHLIGHT_NUM = 2;
	const int NUM_NIGHTS_TO_ADVANCE = 2;
	const float FAR_CLIP = 10000.0f;
	const int PLAYER_SPEED = 40;
	const int ROAD_LENGTH = 4000;
	const int ROAD_WIDTH = 170;
	const D3DXCOLOR DARKGREEN(0.0f, 0.4f, 0.0f, 1.0f);
}

class ColoredCubeApp : public D3DApp
{
public:
	ColoredCubeApp(HINSTANCE hInstance);
	~ColoredCubeApp();

	void initApp();
	void initOrigin();
	void initPickups();
	void initBarrels();
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
	void initShaderResources();
	void initFire();

	void updateScene(float dt);
	void updatePickups(float dt);
	void placePickups();
	void updateOrigin(float dt);
	void updateWalls(float dt);
	void updateBuildings(float dt);
	void updateUniqueObjects(float dt);
	void updatePlayer(float dt);
	void updateEnemies(float dt);
	void updateDayNight();
	void updateLamps(float dt);
	void updateDebugMode();
	void updateMusic();
	void updateHUD(float dt);
	void updateGameState();

	void handleUserInput();
	void handleBuildingCollisions(Vector3 pos);
	void handleWallCollisions(Vector3 pos);
	void handleLampCollisions(Vector3 pos);
	void handleEnemyCollisions(float dt);

	void collisionSlide(GameObject* mobile, GameObject* still);

	void drawScene(); 
	void drawLine(LineObject*);
	void drawOrigin();
	void drawPickups();
	void drawWalls();
	void drawLamps();
	void drawBuildings();
	void drawHUD();
	void drawBarrels();
	void drawFloor();

	void onResize();
	Vector3 moveRuggerDirection();
	void printText(DebugText text);
	void printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, D3DXCOLOR color, int value = -1);
	void printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, D3DXCOLOR color, string value="");
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
	Box mBox, redBox, brick, bulletBox, eBulletBox, yellowGreenBox, goldBox, blueBox, greenBox, tealBox, maroonBox, clearBox, whiteBox;
	Box testBox;
	Player player;
	LineObject xLine, yLine, zLine;
	
	Barrel barrels[gameNS::NUM_BARRELS];
	Wall floor;	//Walls?
	Wall floor2; //Walls?
	vector<Building> buildings;
	vector<Wall> walls;
	vector<LampPost> lamps;
	vector<Pickup> dayPickups;
	vector<Pickup> nightPickups;
	vector<HudObject> hudObjects;
	vector<Bullet*> pBullets;
	Wall menu;

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


	Vector3 startingLevelPosition;
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
	ID3D10ShaderResourceView* mDiffuseMapRVIntroMenu;
	ID3D10ShaderResourceView* mSpecMapRVIntroMenu;
	ID3D10ShaderResourceView* mDiffuseMapRVInstructionsMenu;
	ID3D10ShaderResourceView* mSpecMapRVInstructionsMenu;
	ID3D10ShaderResourceView* mDiffuseMapRVYouWinMenu;
	ID3D10ShaderResourceView* mSpecMapRVYouWinMenu;
	ID3D10ShaderResourceView* mDiffuseMapRVIWinMenu;
	ID3D10ShaderResourceView* mSpecMapRVIWinMenu;
	ID3D10ShaderResourceView* mDiffuseMapRVLevel2;
	ID3D10ShaderResourceView* mSpecMapRVLevel2;

	ID3D10EffectShaderResourceVariable* mfxDiffuseMapVar;
	ID3D10EffectShaderResourceVariable* mfxSpecMapVar;
	ID3D10EffectMatrixVariable* mfxTexMtxVar;
	D3DXMATRIX mCompCubeWorld;

	int level;
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
	bool startScreen, endScreen;
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
	GameState gameState;

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
	srand(static_cast<unsigned int>(time(0)));
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
	gameState = GameState::INTROSCREEN;

	SetCursorPos(0,0);
	ShowCursor(false);
	audio->playCue(INTROMUSIC);
	startScreen = true;

	initBasicVariables(); //Like shotTimer, etc
	initBasicGeometry(); //must happen before init player
	initTextStrings(); //Like start/end screen text
	initUniqueObjects(); //Like the floor
	initBarrels();
	initOrigin();
	initPickups();
	initWallPositions();
	initBuildingPositions();
	initLamps();
	initLights();
	initEnemies();
	initHUD();
	
	mClearColor = gameNS::DAY_SKY_COLOR;
	player.init(&bulletBox, &pBullets, &mBox, sqrt(2.0f), Vector3(3,4,0), Vector3(0,0,0), 200, audio, 1, 1, 1, 5);

	mWallMesh.init(md3dDevice, 1.0f, mFX);
	mBuildingMesh.init(md3dDevice, 1.0f, mFX);
	mEnemyMesh.init(md3dDevice, 1.0f, mFX);
	mPoleMesh.init(md3dDevice, 1.0f, mFX);
	mStreetMesh.init(md3dDevice, 1.0f, mFX);
	mBulletMesh.init(md3dDevice, 1.0f, mFX);
	mBlueMesh.init(md3dDevice, 1.0f, mFX);
	mRedMesh.init(md3dDevice, 1.0f, mFX);
	mYellowMesh.init(md3dDevice, 1.0f, mFX);

	initShaderResources();
	camera.init(input, position, Vector3(1, 0, 0), player.getPosition() + Vector3(1, 0, 0));
	camera.transformToWorld(player.getPosition());
	initFire();

	//mFire.setEmitPos(D3DXVECTOR3(10, 10, 10));
}

void ColoredCubeApp::initLamps() {
	lamps.clear();
	if (level == 1) 
		for (int i = 0; i < 4; i++)
			lamps.push_back(LampPost());
	else if (level == 2) 
		for (int i = 0; i < 4; i++) 
			lamps.push_back(LampPost());


	if (level == 1) {
		lamps[0].init(&brick, Vector3(58.5f,0.1f,58.5f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 2.3456f);
		lamps[1].init(&brick, Vector3(-58.5f,0.1f,-58.5f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 5.49f);
		lamps[2].init(&brick, Vector3(58.5f,0.1f,-58.5f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 3.9359f);
		lamps[3].init(&brick, Vector3(-58.5f,0.1f,58.5f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 0.7944f);
	} else if (level == 2) {
		lamps[0].init(&brick, Vector3(-100.0f,0.1f,100.0f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 6.3f);
		lamps[1].init(&brick, Vector3(100.0f,0.1f,100.0f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 3.2f);
		lamps[2].init(&brick, Vector3(-100.0f,0.1f,-100.0f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 6.3f);
		lamps[3].init(&brick, Vector3(100.0f,0.1f,-100.0f), 1.0f, 1.0f, 1, 1, 1, 0.0f, 3.2f);
	}
}

void ColoredCubeApp::initPickups() {
	//define the pickups
	dayPickups.clear();
	nightPickups.clear();
	if (level == 1) {
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 0, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 1, RELOAD, audio, level));
		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 2, RELOAD, audio, level));
		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 3, RELOAD, audio, level));
		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 15, 4, RELOAD, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 5, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 6, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 7, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 8, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 9, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 10, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 11, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 12, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 13, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 14, WHOOSH, audio, level));
		dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 15, WHOOSH, audio, level));
		dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 16, WHOOSH, audio, level));
		dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 5, 17, WHOOSH, audio, level));

	
		nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 13, ZIPPER, audio, level));
		nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 14, ZIPPER, audio, level));
		nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 15, ZIPPER, audio, level));
		nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 16, ZIPPER, audio, level));
		nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 50, 13, RELOAD, audio, level));
		nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 50, 14, RELOAD, audio, level));
		nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 50, 15, RELOAD, audio, level));
		nightPickups.push_back(Pickup(&greenBox, &player.currentGun, INCREASE, 1, 16, NEW_GUN, audio, level));

	} else if (level == 2) {
		dayPickups.push_back(Pickup(&greenBox, &player.currentGun, INCREASE, 1, 0, NEW_GUN, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 1, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 2, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 3, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 4, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 5, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 6, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 7, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 8, RELOAD, audio, level));

		dayPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 20, 9, RELOAD, audio, level));
		dayPickups.push_back(Pickup(&greenBox, &player.currentGun, INCREASE, 1, 10, NEW_GUN, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 11, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 12, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 13, ZIPPER, audio, level));
		dayPickups.push_back(Pickup(&greenBox, &player.currentGun, INCREASE, 1, 14, NEW_GUN, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 15, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 16, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 17, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 18, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 19, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 15, 20, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 10, 21, WHOOSH, audio, level));

		dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 10, 22, WHOOSH, audio, level));

		dayPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 30, 23, ZIPPER, audio, level));

		dayPickups.push_back(Pickup(&goldBox, &player.speed, INCREASE, 10, 24, WHOOSH, audio, level));


		nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 30, 9, RELOAD, audio, level));
		nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 12, ZIPPER, audio, level));
		nightPickups.push_back(Pickup(&blueBox, &player.ammo, INCREASE, 30, 10, RELOAD, audio, level));
		nightPickups.push_back(Pickup(&redBox, &player.health, INCREASE, 50, 16, ZIPPER, audio, level));
	}


	for (unsigned int i = 0; i < dayPickups.size(); i++) {
	//	dayPickups[i].startGlowing();
		dayPickups[i].setInActive();
	}
	for (unsigned int i = 0; i < nightPickups.size(); i++) {
	//	nightPickups[i].startGlowing();
		nightPickups[i].setInActive();
	}
}

void ColoredCubeApp::initBasicGeometry() {	
	mBox.init(md3dDevice, 2.0f, D3DXCOLOR(0,0,0,0), mFX);
	whiteBox.init(md3dDevice, 1.0f, D3DXCOLOR(1,1,1,0), mFX);
	tealBox.init(md3dDevice, 1.0f, colorNS::TEAL, mFX);
	clearBox.init(md3dDevice, 1.0f, D3DXCOLOR(0,0,0,0), mFX);
	redBox.init(md3dDevice, 1.0f, colorNS::RED, mFX);
	greenBox.init(md3dDevice, 1.0f, colorNS::GREEN, mFX);
	brick.init(md3dDevice, 1.0f, DARKBROWN, mFX);
	bulletBox.init(md3dDevice, 0.25f, D3DXCOLOR(0,0,0,0), mFX);
	eBulletBox.init(md3dDevice, 0.5f, RED, mFX);
	maroonBox.init(md3dDevice, 10000, colorNS::MAROON, mFX);
	yellowGreenBox.init(md3dDevice, 1.f,D3DXCOLOR(0,0,0,0), mFX);
	goldBox.init(md3dDevice, 1.0f, YELLOW, mFX);
	blueBox.init(md3dDevice, 1.0f, BLUE, mFX);
	rLine.init(md3dDevice, 10.0f, RED);
	bLine.init(md3dDevice, 10.0f, BLACK);
	gLine.init(md3dDevice, 10.0f, GREEN);
	activeLine.init(md3dDevice, 2.0f, RED, mFX);
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

	level = 1;
	score = 0;
	firstpass = true;
	startScreen = true;
	endScreen = false;
	night = false;
	found = false;
	timect = 0.0f;
	timeOfDay = "Day";
	srand(static_cast<unsigned int>(time(0)));
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
	startingLevelPosition = Vector3(0.0f,4.0f,0.0f);
}

void ColoredCubeApp::initBuildingPositions() {
	//clear old buildings if any and make new ones.
	buildings.clear();
	if (level == 1) {
		for (int i = 0; i < 12; i++) 
			buildings.push_back(Building());
	} else if (level == 2) {
		for (int i = 0; i < 27; i++) 
			buildings.push_back(Building());
	}
	
	if (level == 1) {
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
	} else if (level == 2) {
		buildings[0].init(&brick, 2.0f, Vector3(700, 0, 1300),1,	190,	50,  190);//Left Side Building 1
		buildings[1].init(&brick, 2.0f, Vector3(370, 0, 1020),1,	50,		50,  95);//Left Side Building 2
		buildings[2].init(&brick, 2.0f, Vector3(300, 0, 1350),1,	95,		50,  95);//Left Side Building 3
		buildings[3].init(&brick, 2.0f, Vector3(700, 0, 925),1,	95,		50,  95);//Left Side Building 4

		buildings[4].init(&brick, 2.0f, Vector3(300, 0, 700),	1,	95,		70,  90);//Left Side Building 5
		buildings[5].init(&brick, 2.0f, Vector3(700, 0, 500),	1,	120,	50,  95);//Left Side Building 6
		buildings[6].init(&brick, 2.0f, Vector3(700, 0, 150),	1,	120,	50,  108);//Left Side Building 7
		buildings[7].init(&brick, 2.0f, Vector3(350, 0, 250),	1,	72,		50,  120);//Left Side Building 8

		buildings[8].init(&brick, 2.0f, Vector3(750, 0, -200),	1,	90,		80,  95);//Left Side Building 9
		buildings[9].init(&brick, 2.0f, Vector3(350, 0, -300),	1,	75,		50,  95);//Left Side Building 10
		buildings[10].init(&brick, 2.0f, Vector3(750, 0, -550),	1,	120,		50,  95);//Left Side Building 11
		buildings[11].init(&brick, 2.0f, Vector3(325, 0, -650),	1,	100,	50,  95);//Left Side Building 12
		buildings[12].init(&brick, 2.0f, Vector3(800, 0, -1200),1,	120,	50,  250);//Left Side Building 13
		buildings[13].init(&brick, 2.0f, Vector3(350, 0, -1300),1,	72,		50,  72);//Left Side Building 14

		buildings[14].init(&brick, 2.0f, Vector3(-700, 0, 1150),1,	95,		50,  320);//Right Side Building 15
		buildings[15].init(&brick, 2.0f, Vector3(-300, 0, 1100),1,	110,	50,  130);//Right Side Building 16
		buildings[16].init(&brick, 2.0f, Vector3(-600, 0, 450),	1,	100,	50,  90);//Right Side Building 17
		buildings[17].init(&brick, 2.0f, Vector3(-300, 0, 550),	1,	50,		30,  150);//Right Side Building 18
		buildings[18].init(&brick, 2.0f, Vector3(-370, 0, 195),1,	95,		50,  95);//Right Side Building 19

		buildings[19].init(&brick, 2.0f, Vector3(-615, 0, -10),	1,	150,	50,  110);//Right Side Building 20
		buildings[20].init(&brick, 2.0f, Vector3(-370, 0, -215),1,	95,		50,  95);//Right Side Building 21
		buildings[21].init(&brick, 2.0f, Vector3(-700, 0, -275),1,	95,		50,  95);//Right Side Building 22
		buildings[22].init(&brick, 2.0f, Vector3(-250, 0, -700),1,	50,		90,  50);//Right Side Building 23

		buildings[23].init(&brick, 2.0f, Vector3(-650, 0, -800), 1,	110,	50,  200);//Right Side Building 24
		buildings[24].init(&brick, 2.0f, Vector3(-225, 0, -1000),1,	50,		60,  50);//Right Side Building 25
		buildings[25].init(&brick, 2.0f, Vector3(-650, 0, -1300),1,	110,	50,  200);//Right Side Building 26
		buildings[26].init(&brick, 2.0f, Vector3(-200, 0, -1300),1,	50,		30,  50);//Right Side Building 27
	}
}

void ColoredCubeApp::initWallPositions() {
	walls.clear();
	//create number of walls per level.
	if (level == 1) 
		for (int i = 0; i < 16; i++) 
			walls.push_back(Wall());
	else if (level == 2) 
		for (int i = 0; i < 12; i++) 
			walls.push_back(Wall());
	
	if (level == 1)  {
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
	} else if (level == 2) {
		//Level 2
		walls[0].init(&brick, 2.0f, Vector3(0, 0, -1625),	Vector3(0,0,0), 1,	1,	980,	20,	10);// Far Wall
		walls[1].init(&brick, 2.0f, Vector3(0, 0, 1625),	Vector3(0,0,0), 1,	1,	980,	20,	10);// Back Wall
		walls[2].init(&brick, 2.0f, Vector3(-980, 0, 0),	Vector3(0,0,0), 1,	1,	10,		20,	1625);// Back Wall
		walls[3].init(&brick, 2.0f, Vector3(980, 0, 0),		Vector3(0,0,0), 1,	1,	10,		20,	1625);// Back Wall

		//Safe zone - level 2
		walls[4].init(&brick, 2.0f, Vector3(482.5, 0, 50),	Vector3(0, 0, 0), 1, 1, 17.5,	2.5, 1);
		walls[5].init(&brick, 2.0f, Vector3(417.5, 0, 50),	Vector3(0, 0, 0), 1, 1, 17.5,	2.5, 1);
		walls[6].init(&brick, 2.0f, Vector3(500, 0, 32.5),	Vector3(0, 0, 0), 1, 1, 1,		2.5, 17.5);
		walls[7].init(&brick, 2.0f, Vector3(400, 0, 32.5),	Vector3(0, 0, 0), 1, 1, 1,		2.5, 17.5);

		walls[8].init(&brick, 2.0f, Vector3(482.5, 0, -50),Vector3(0, 0, 0), 1, 1, 17.5,	2.5, 1);
		walls[9].init(&brick, 2.0f, Vector3(417.5, 0, -50),Vector3(0, 0, 0), 1, 1, 17.5,	2.5, 1);
		walls[10].init(&brick, 2.0f, Vector3(500, 0, -32.5),Vector3(0, 0, 0), 1, 1, 1,		2.5, 17.5);
		walls[11].init(&brick, 2.0f, Vector3(400, 0, -32.5),Vector3(0, 0, 0), 1, 1, 1,		2.5, 17.5);
	}
}

void ColoredCubeApp::initUniqueObjects() {
	floor.init(&yellowGreenBox, 2.0f, Vector3(0,-1000.0f,0), Vector3(0,0,0), 1, 1.0f, 250, 500, 250);
	floor2.init(&yellowGreenBox, 2.0f, Vector3(0,-1000.0f,0), Vector3(0,0,0), 1, 1.0f, 975, 500, 1625);
	menu.init(&clearBox, 2.0f, Vector3(-998.0f,-1100.0f,-727.1f), Vector3(0,0,0), 1, 1.0f, 75, 100, 100);
}

void ColoredCubeApp::initBarrels() {
	barrels[0].init(&brick, 2.0f, Vector3(-85, 0, 1500),	1.0f,	1,		3,  1);
	barrels[1].init(&brick, 2.0f, Vector3(85, 0, 1475),		1.0f,	1,		3,  1);
	barrels[2].init(&brick, 2.0f, Vector3(-85, 0, 1300),	1.0f,	1,		3,  1);
	barrels[3].init(&brick, 2.0f, Vector3(85, 0, 1275),		1.0f,	1,		3,  1);
	barrels[4].init(&brick, 2.0f, Vector3(-85, 0, 1100),	1.0f,	1,		3,  1);
	barrels[5].init(&brick, 2.0f, Vector3(85, 0, 1075),		1.0f,	1,		3,  1);
	barrels[6].init(&brick, 2.0f, Vector3(-85, 0, 800),		1.0f,	1,		3,  1);
	barrels[7].init(&brick, 2.0f, Vector3(85, 0, 775),		1.0f,	1,		3,  1);
	barrels[8].init(&brick, 2.0f, Vector3(-85, 0, 500),		1.0f,	1,		3,  1);
	barrels[9].init(&brick, 2.0f, Vector3(85, 0, 475),		1.0f,	1,		3,  1);
	barrels[10].init(&brick, 2.0f, Vector3(-85, 0, 300),	1.0f,	1,		3,  1);
	barrels[11].init(&brick, 2.0f, Vector3(85, 0, 275),		1.0f,	1,		3,  1);

	barrels[12].init(&brick, 2.0f, Vector3(-85, 0, -275),	1.0f,	1,		3,  1);
	barrels[13].init(&brick, 2.0f, Vector3(85, 0, -300),	1.0f,	1,		3,  1);
	barrels[14].init(&brick, 2.0f, Vector3(-85, 0, -475),	1.0f,	1,		3,  1);
	barrels[15].init(&brick, 2.0f, Vector3(85, 0, -500),	1.0f,	1,		3,  1);
	barrels[16].init(&brick, 2.0f, Vector3(-85, 0, -775),	1.0f,	1,		3,  1);
	barrels[17].init(&brick, 2.0f, Vector3(85, 0, -800),	1.0f,	1,		3,  1);
	barrels[18].init(&brick, 2.0f, Vector3(-85, 0, -1075),	1.0f,	1,		3,  1);
	barrels[19].init(&brick, 2.0f, Vector3(85, 0, -1100),	1.0f,	1,		3,  1);
	barrels[20].init(&brick, 2.0f, Vector3(-85, 0, -1275),	1.0f,	1,		3,  1);
	barrels[21].init(&brick, 2.0f, Vector3(85, 0, -1300),	1.0f,	1,		3,  1);
	barrels[22].init(&brick, 2.0f, Vector3(-85, 0, -1475),	1.0f,	1,		3,  1);
	barrels[23].init(&brick, 2.0f, Vector3(85, 0, -1500),	1.0f,	1,		3,  1);
}

void ColoredCubeApp::initEnemies() {
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++) {
		enemy[i].init(&mBox, 2.0f, Vector3((float)(rand()%50),0.f,(float)(rand()%50)), Vector3(0.f,0.f,0.f), 1.f, 1.f, 1, 2, 1);
		enemy[i].faceObject(&player);
	}
}

void ColoredCubeApp::initOrigin() {
	xLine.init(&rLine, Vector3(0,0,0), 5);
	xLine.setPosition(Vector3(0,0,0));
	yLine.init(&bLine, Vector3(0,0,0), 5);
	yLine.setPosition(Vector3(0,0,0));
	yLine.setRotationZ(static_cast<float>(ToRadian(90)));
	zLine.init(&gLine, Vector3(0,0,0), 5);
	zLine.setPosition(Vector3(0,0,0));
	zLine.setRotationY(static_cast<float>(ToRadian(90)));
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
	
	if(level == 1)
	{
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
		mLights[7].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[7].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[7].att.x    = 0.0f;
		mLights[7].att.y    = 0.55f;
		mLights[7].att.z    = 0.0f;
		mLights[7].range    = 90.0f;
		mLights[7].pos = D3DXVECTOR3(200, 10, 0);

		mLights[8].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[8].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[8].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[8].att.x    = 0.0f;
		mLights[8].att.y    = 0.55f;
		mLights[8].att.z    = 0.0f;
		mLights[8].range    = 90.0f;
		mLights[8].pos = D3DXVECTOR3(-200, 10, 0);

		mLights[9].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[9].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[9].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[9].att.x    = 0.0f;
		mLights[9].att.y    = 0.55f;
		mLights[9].att.z    = 0.0f;
		mLights[9].range    = 90.0f;
		mLights[9].pos = D3DXVECTOR3(0, 10, -200);

		mLights[10].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[10].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[10].specular = D3DXCOLOR(1.0f, 0.55f, 1.0f, 1.0f);
		mLights[10].att.x    = 0.0f;
		mLights[10].att.y    = 0.55f;
		mLights[10].att.z    = 0.0f;
		mLights[10].range    = 35.0f;
		mLights[10].pos = D3DXVECTOR3(0, 10, 200);

		mLights[11].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[11].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[11].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[11].att.x    = 0.0f;
		mLights[11].att.y    = 0.55f;
		mLights[11].att.z    = 0.0f;
		mLights[11].range    = 90.0f;
		mLights[11].pos = D3DXVECTOR3(85, 0, 275);

		for(int i=12; i<gameNS::NUM_LIGHTS; i++)
		{
			mLights[i].ambient	= D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			mLights[i].diffuse	= D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			mLights[i].specular	= D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
			mLights[i].range	= 0.0f;
			mLights[i].pos		= D3DXVECTOR3(0, 100, 0);
		}
	}

	else if (level == 2)
	{
		//Lamppost lights
		mLights[3].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[3].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[3].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[3].att.x    = 0.0f;
		mLights[3].att.y    = 0.55f;
		mLights[3].att.z    = 0.0f;
		mLights[3].range    = 90.0f;
		mLights[3].pos = D3DXVECTOR3(-100, 10, -100);

		mLights[4].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[4].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[4].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[4].att.x    = 0.0f;
		mLights[4].att.y    = 0.55f;
		mLights[4].att.z    = 0.0f;
		mLights[4].range    = 90.0f;
		mLights[4].pos = D3DXVECTOR3(-100, 10, 100);

		mLights[5].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[5].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[5].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[5].att.x    = 0.0f;
		mLights[5].att.y    = 0.55f;
		mLights[5].att.z    = 0.0f;
		mLights[5].range    = 90.0f;
		mLights[5].pos = D3DXVECTOR3(100, 10, -100);

		mLights[6].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[6].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[6].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[6].att.x    = 0.0f;
		mLights[6].att.y    = 0.55f;
		mLights[6].att.z    = 0.0f;
		mLights[6].range    = 90.0f;
		mLights[6].pos = D3DXVECTOR3(100, 10, 100);

		//Safe zone lights
		mLights[7].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[7].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[7].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[7].att.x    = 0.0f;
		mLights[7].att.y    = 0.55f;
		mLights[7].att.z    = 0.0f;
		mLights[7].range    = 90.0f;
		mLights[7].pos = D3DXVECTOR3(400 + 25, 10, 50 - 25);

		mLights[8].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[8].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[8].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[8].att.x    = 0.0f;
		mLights[8].att.y    = 0.55f;
		mLights[8].att.z    = 0.0f;
		mLights[8].range    = 90.0f;
		mLights[8].pos = D3DXVECTOR3(400 + 25, 10, -50 + 25);

		mLights[9].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[9].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[9].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[9].att.x    = 0.0f;
		mLights[9].att.y    = 0.55f;
		mLights[9].att.z    = 0.0f;
		mLights[9].range    = 90.0f;
		mLights[9].pos = D3DXVECTOR3(500 - 25, 10, 50 - 25);

		mLights[10].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[10].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[10].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[10].att.x    = 0.0f;
		mLights[10].att.y    = 0.55f;
		mLights[10].att.z    = 0.0f;
		mLights[10].range    = 90.0f;
		mLights[10].pos = D3DXVECTOR3(500 - 25, 10, -50 + 25);

		//End of road lights
		mLights[11].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[11].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[11].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[11].att.x    = 0.0f;
		mLights[11].att.y    = 0.55f;
		mLights[11].att.z    = 0.0f;
		mLights[11].range    = 200.0f;
		mLights[11].pos = D3DXVECTOR3(0, 10, -1500);

		mLights[12].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[12].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[12].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[12].att.x    = 0.0f;
		mLights[12].att.y    = 0.55f;
		mLights[12].att.z    = 0.0f;
		mLights[12].range    = 200.0f;
		mLights[12].pos = D3DXVECTOR3(-25, 10, -1400);

		mLights[13].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[13].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[13].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[13].att.x    = 0.0f;
		mLights[13].att.y    = 0.55f;
		mLights[13].att.z    = 0.0f;
		mLights[13].range    = 200.0f;
		mLights[13].pos = D3DXVECTOR3(25, 10, -1400);

		mLights[14].ambient  = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);
		mLights[14].diffuse  = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);
		mLights[14].specular = D3DXCOLOR(1.0f, 0.55f, 0.0f, 1.0f);
		mLights[14].att.x    = 0.0f;
		mLights[14].att.y    = 0.55f;
		mLights[14].att.z    = 0.0f;
		mLights[14].range    = 200.0f;
		mLights[14].pos = D3DXVECTOR3(0, 10, -1300);
	}
}

void ColoredCubeApp::initHUD() {
	hudObjects.push_back(HudObject());

	hudObjects[0].init(&blueBox, 1.0f, Vector3(0,0,0), Vector3(0,0,0), 1, 1);

	for (unsigned int i = 0; i < hudObjects.size(); i++) {
		hudObjects[i].faceObject(&player);
		//hudObjects[i].setActive();
	}
}

void ColoredCubeApp::initShaderResources() {
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
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"introMenu.png", 0, 0, &mDiffuseMapRVIntroMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVIntroMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"instructions.png", 0, 0, &mDiffuseMapRVInstructionsMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVInstructionsMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"youWin.png", 0, 0, &mDiffuseMapRVYouWinMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVYouWinMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"iWin.png", 0, 0, &mDiffuseMapRVIWinMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVIWinMenu, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"onToLevel2.png", 0, 0, &mDiffuseMapRVLevel2, 0 ));
	HR(D3DX10CreateShaderResourceViewFromFile(md3dDevice, L"defaultspec.dds", 0, 0, &mSpecMapRVLevel2, 0 ));
}

void ColoredCubeApp::initFire() {
	vector<std::wstring> flares;
	flares.push_back(L"flare0.dds"); 
	ID3D10ShaderResourceView* texArray = GetTextureMgr().createTexArray(L"flares", flares);
	
	mFire[0].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 1500), &camera); 
	mFire[1].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 1475), &camera); 
	//mFire[2].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 1300), &camera); 
	//mFire[3].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 1275), &camera); 
	mFire[2].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 1100), &camera); 
	mFire[3].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 1075), &camera); 
	//mFire[6].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 800), &camera); 
	//mFire[7].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 775), &camera); 
	mFire[4].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 500), &camera); 
	mFire[5].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 475), &camera); 
	//mFire[10].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, 300), &camera); 
	//mFire[11].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, 275), &camera); 

	mFire[6].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -275), &camera); 
	mFire[7].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -300), &camera); 
	//mFire[14].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -475), &camera); 
	//mFire[15].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -500), &camera); 
	mFire[8].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -775), &camera); 
	mFire[9].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -800), &camera); 
	//mFire[18].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -1075), &camera); 
	//mFire[19].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -1100), &camera); 
	mFire[10].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -1275), &camera); 
	mFire[11].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -1300), &camera); 
	//mFire[22].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(-85, 0, -1475), &camera); 
	//mFire[23].init(md3dDevice, fx::FireFX, texArray, 125, D3DXVECTOR3(85, 0, -1500), &camera);
}


void ColoredCubeApp::updateScene(float dt)
{
	ColoredCubeApp::dt = dt;
	gameTime += dt;
	bool playing = (!endScreen && !startScreen);
	//bool playing = (!endScreen && !startScreen);
	

	if(input->isKeyDown(VK_ESCAPE)) 
		PostQuitMessage(0);

	if (gameState == GameState::INTROSCREEN){
		camera.transformToMenu();
		menu.update(dt);
		if(input->isKeyDown(VK_SPACE)){
			gameState = GameState::INSTRUCTIONS;
			input->keyUp(VK_SPACE);
		}
	}
	if (gameState == GameState::INSTRUCTIONS) {
		if(input->isKeyDown(VK_SPACE)){
			menu.update(dt);
			audio->stopCue(INTROMUSIC);
			gameState = GameState::PLAYING;
			camera.transformToWorld(player.getPosition());
			input->keyUp(VK_SPACE);
		}
	}
	if(gameState == GameState::PLAYING){
		//Restricting the mouse movement
		RECT restrict = {639, 399, 640, 400};
		ClipCursor(&restrict);
		Vector3 oldPos = camera.getPosition();
		timect += dt;
		updateGameState(); //Checks for win/lose/levelTransition conditions
		if(input->isKeyDown(VK_ESCAPE))  PostQuitMessage(0);
		D3DApp::updateScene(dt);
		menu.update(dt);
		updateDebugMode();
		updateMusic();
		menu.update(dt);
		updateDayNight();

		camera.update(dt, static_cast<float>(gameNS::PLAYER_SPEED), &walking);
		

		updateOrigin(dt);
		handleUserInput();
		updatePlayer(dt);
		updateEnemies(dt);
		updatePickups(dt);
		updateLamps(dt);
		updateWalls(dt);
		updateBuildings(dt);
		updateUniqueObjects(dt);
		//updateHUD(dt);
		placePickups();
		
		//Handle Collisions
		handleWallCollisions(oldPos);
		handleBuildingCollisions(oldPos);
		handleEnemyCollisions(dt);

		mLights[0].ambient.r = 0.1f;
		for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)
		{
			if(enemy[i].getAttacking()) mLights[0].ambient.r = 0.5f; 
		}

		for(int i=0; i<gameNS::NUM_FIRES; i++)
			mFire[i].update(dt, gameTime);
	}
	if (gameState == GameState::BEATLV1) {
		if (level == 1) {
			menu.update(dt);
			camera.transformToMenu();
			level = 2;
			ColoredCubeApp::initLamps();
			ColoredCubeApp::initPickups();
			ColoredCubeApp::initWallPositions();
			ColoredCubeApp::initBuildingPositions();
			ColoredCubeApp::initLights();
			/*timeOfDay = "Day";
			night = false;
			timect = 0.0f;*/

			for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)
			{
				enemy[i].setInActive();
				enemy[i].initWaypoints2();
				
			}
		}
		//lock the screen at a certain spot and render the cube with the transition graphic and then...
		if(input->isKeyDown(VK_SPACE)) {
			camera.transformToWorld(startingLevelPosition);
			gameState = GameState::PLAYING;
		}
	}
	if(gameState == GameState::LOSE){
		camera.transformToMenu();
		menu.update(dt);
		//lock the screen at a certain spot and render the cube with the transition graphic and then...
		if(input->isKeyDown(VK_SPACE))
			PostQuitMessage(0);
	}
	if (gameState == GameState::WIN) {
		camera.transformToMenu();
		menu.update(dt);
		//Lock the screen at a certain spot and render the cube with the transition graphic and then...
		if (input->isKeyDown(VK_SPACE))
			PostQuitMessage(0);
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
		if (gameNS::PLAY_MUSIC) {
			audio->stopCue(MUSIC);
			audio->playCue(MUSIC);
		}
	}
}

void ColoredCubeApp::updateDebugMode() {
	if(input->wasKeyPressed(KEY_K)) {
		debugMode = true;
		input->clear(KEY_K);
		player.setSpeed(500);
	} 
	if (input->wasKeyPressed(KEY_L)) {
		camera.setPosition(D3DXVECTOR3(camera.getPosition().x, 5, camera.getPosition().z));
		debugMode = false;
		input->clear(KEY_L);
		player.setSpeed(200);
	}
	if (input->wasKeyPressed(KEY_M)) {
		gameNS::PLAY_MUSIC = false;
		audio->stopCue(MUSIC);
	}
}

void ColoredCubeApp::doEndScreen() {
	
}

void ColoredCubeApp::updateUniqueObjects(float dt) {
	floor.update(dt);
	floor2.update(dt);
	//big floor update must also be added here for it to show.

	for(int i=0; i <gameNS::NUM_BARRELS; i++)
		barrels[i].update(dt);
}

void ColoredCubeApp::updateWalls(float dt) {
	for(int i=0; i<walls.size(); i++)
		walls[i].update(dt);
}

void ColoredCubeApp::updateLamps(float dt) {
	for (int i = 0; i < lamps.size(); i++) {
		lamps[i].update(dt);
	}
}

void ColoredCubeApp::updateBuildings(float dt) {
	for(int i=0; i<buildings.size(); i++)
		buildings[i].update(dt);
}

void ColoredCubeApp::updatePlayer(float dt) {
	player.setPosition(camera.getPosition());
	player.setVelocity(camera.getDirection());
	D3DXVECTOR3 pos = player.getPosition();
	
	player.update(dt, camera.getLookatDirection(), &bulletBox, &pBullets); //bullet should follow camera lookat vector

	//Update shooting
	if(input->getMouseLButton())
	{
		if(player.canShoot()) 
			player.fired = true; //this player value being set dictates whether the player shoots the next time player.update() is called
		else player.fired = false;
	} else {
		player.firedLastFrame = false;
		player.fired = false;
	}


	//Update walking noises
	if (walking) {
		stepTime += 1;
		if (stepTime*dt > gameNS::FOOTSTEP_GAP) {
			if (level == 1) {
				if (pos.x < gameNS::GRASSY_AREA_WIDTH/2.0f && pos.x > -gameNS::GRASSY_AREA_WIDTH/2.0f  && pos.z > -gameNS::GRASSY_AREA_WIDTH/2.0f && pos.z < gameNS::GRASSY_AREA_WIDTH/2.0f) { //in grassy area 
					if (step1) audio->playCue(FOOTSTEP3);
					else audio->playCue(FOOTSTEP4);
				} else {
					if (step1) audio->playCue(FOOTSTEP1);
					else audio->playCue(FOOTSTEP2);
				}
			} else if (level == 2) {
				if (pos.x < gameNS::ROAD_WIDTH/2.0f && pos.x > -gameNS::ROAD_WIDTH/2.0f && pos.z > -gameNS::ROAD_LENGTH/2.0f && pos.z < gameNS::ROAD_LENGTH/2.0f) {
					if (step1) audio->playCue(FOOTSTEP1);
					else audio->playCue(FOOTSTEP2);
				} else {
					if (step1) audio->playCue(FOOTSTEP3);
					else audio->playCue(FOOTSTEP4);
				}
			}
			step1 = !step1;
			stepTime = 0.0f;
		}
	}

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
				if(level == 1){
					if(D3DXVec3LengthSq(&(enemy[i].getPosition() - D3DXVECTOR3(0,0,0))) < 55*55)
					{
						enemy[i].setSpeed(enemyNS::DAY_SPEED);
					}
					else enemy[i].setSpeed(enemyNS::NIGHT_SPEED);
				}
				else if (level == 2)
				{
					if(D3DXVec3LengthSq(&(enemy[i].getPosition() - D3DXVECTOR3(450,0,0))) < 55*55)
					{
						enemy[i].setSpeed(enemyNS::DAY_SPEED);
					}
					else enemy[i].setSpeed(enemyNS::NIGHT_SPEED);
				}
				
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
	for(int i=0; i<walls.size(); i++)
	{
		if(player.collided(&walls[i]))
		{
			//Player position before collision
			D3DXVECTOR3 pPos = pos;
			D3DXVECTOR3 pVel = player.getVelocity();
			//D3DXVec3Normalize(&pVel, &pVel);
			D3DXVECTOR3 wallPos = walls[i].getPosition();
			D3DXVECTOR3 wNormal(0, 0, 0); //normal of the wall surface where the player intersects
			D3DXVECTOR3 refPoint = camera.getLookatDirection();

			float depth = walls[i].getDepth();
			float width = walls[i].getWidth();
			float t = 0;

			//D1-4 represent points on each of the planes we want to check for intersection with
			//We are just donig ray-plane intersection here, with the ray origin being at the 
			D3DXVECTOR3 D[4] = {D3DXVECTOR3(wallPos.x + width + 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x - width - 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z+depth + 2.1), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z-depth - 2.1)};
			D3DXVECTOR3 N[4] = {D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(-1, 0, 0), D3DXVECTOR3(0, 0, 1), D3DXVECTOR3(0, 0, -1)};

			//find the minimum intersection time and the normal of the surface that we intersect with
			for(int l=0; l<4; l++)
			{
				float denom = D3DXVec3Dot(&N[l], &pVel);
				if(denom != 0)
				{
					float time = D3DXVec3Dot(&N[l], &(D[l]-pPos))/denom;
					if(time > 0 ){
						if(t <= 0 || time < t && t < 1){
							t = time;
							wNormal = N[l];
						}
					}
				}
			}

			D3DXVECTOR3 npv;
			D3DXVec3Normalize(&npv, &pVel);

			float deg = acos(D3DXVec3Dot(&npv, &wNormal));
			deg *= 180/PI;
			if(deg <= 210.f && deg >= 150.f ){
				camera.setPosition(pos);
			}
			else
			{
				D3DXVec3Normalize(&pVel, &pVel);
				camera.setPosition(camera.getPosition() - (D3DXVec3Dot(&(camera.getPosition()-(pPos + t*(pVel*gameNS::PLAYER_SPEED))), &wNormal)*wNormal));
			}
			camera.setLookAt(camera.getPosition() + refPoint);
		}

		for (unsigned int j = 0; j < pBullets.size(); j++) {
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
	for(int i=0; i<buildings.size(); i++)
	{
		if (buildings[i].getActiveState() == false) continue;
		if(player.collided(&buildings[i])){
			//Player position before collision
			D3DXVECTOR3 pPos = pos;
			D3DXVECTOR3 pVel = player.getVelocity();
			//D3DXVec3Normalize(&pVel, &pVel);
			D3DXVECTOR3 wallPos = buildings[i].getPosition();
			D3DXVECTOR3 wNormal(0, 0, 0); //normal of the wall surface where the player intersects
			D3DXVECTOR3 refPoint = camera.getLookatDirection();

			float depth = buildings[i].getDepth();
			float width = buildings[i].getWidth();
			float t = 0;

			//D1-4 represent points on each of the planes we want to check for intersection with
			//We are just donig ray-plane intersection here, with the ray origin being at the 
			D3DXVECTOR3 D[4] = {D3DXVECTOR3(wallPos.x + width + 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x - width - 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z+depth + 2.1), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z-depth - 2.1)};
			D3DXVECTOR3 N[4] = {D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(-1, 0, 0), D3DXVECTOR3(0, 0, 1), D3DXVECTOR3(0, 0, -1)};

			//find the minimum intersection time and the normal of the surface that we intersect with
			for(int l=0; l<4; l++)
			{
				float denom = D3DXVec3Dot(&N[l], &pVel);
				if(denom != 0)
				{
					float time = D3DXVec3Dot(&N[l], &(D[l]-pPos))/denom;
					if(time > 0 ){
						if(t <= 0 || time < t && t < 1){
							t = time;
							wNormal = N[l];
						}
					}
				}
			}

			D3DXVECTOR3 npv;
			D3DXVec3Normalize(&npv, &pVel);
			float deg = acos(D3DXVec3Dot(&npv, &wNormal));
			deg *= 180/PI;
			if(deg <= 210.f && deg >= 150.f ){
				camera.setPosition(pos);
			}
			else
			{
				D3DXVec3Normalize(&pVel, &pVel);
				camera.setPosition(camera.getPosition() - (D3DXVec3Dot(&(camera.getPosition()-(pPos + t*(pVel*gameNS::PLAYER_SPEED))), &wNormal)*wNormal));
			}
			camera.setLookAt(camera.getPosition() + refPoint);

		}
		for (unsigned int j = 0; j < pBullets.size(); j++) {
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
		for(int i=0; i<walls.size(); i++)
	{
		if(player.collided(&lamps[i]))
			player.setPosition(pos);

		for (unsigned int j = 0; j < pBullets.size(); j++) {
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

void ColoredCubeApp::handleEnemyCollisions(float dt)
{
	
	for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)
	{
		for(unsigned int j=0; j<pBullets.size(); j++)
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
		for(int j=0; j<walls.size(); j++)
		{
			if(enemy[i].collided(&walls[j]))
			{
				////Player position before collision
				//D3DXVECTOR3 pPos = enemy[i].getOldPos();
				//D3DXVECTOR3 pVel = enemy[i].getVelocity();
				////D3DXVec3Normalize(&pVel, &pVel);
				//D3DXVECTOR3 wallPos = walls[j].getPosition();
				//D3DXVECTOR3 wNormal(0, 0, 0); //normal of the wall surface where the player intersects
				////D3DXVECTOR3 refPoint = camera.getLookatDirection();
				//float depth = walls[j].getDepth();
				//float width = walls[j].getWidth();
				//float t = 0;
				////D1-4 represent points on each of the planes we want to check for intersection with
				////We are just donig ray-plane intersection here, with the ray origin being at the 
				//D3DXVECTOR3 D[4] = {D3DXVECTOR3(wallPos.x + width + 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x - width - 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z+depth + 2.1), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z-depth - 2.1)};
				//D3DXVECTOR3 N[4] = {D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(-1, 0, 0), D3DXVECTOR3(0, 0, 1), D3DXVECTOR3(0, 0, -1)};
				////find the minimum intersection time and the normal of the surface that we intersect with
				//for(int l=0; l<4; l++)
				//{
				//	float denom = D3DXVec3Dot(&N[l], &pVel);
				//	if(denom != 0)
				//	{
				//		float time = D3DXVec3Dot(&N[l], &(D[l]-pPos))/denom;
				//		if(time > 0 ){
				//			if(t <= 0 || time < t && t < 1){
				//				t = time;
				//				wNormal = N[l];
				//			}
				//		}
				//	}
				//}
				//D3DXVec3Normalize(&pVel, &pVel);
				//enemy[i].setPosition(enemy[i].getPosition() - (D3DXVec3Dot(&(enemy[i].getPosition()-(pPos + t*(pVel*enemy[i].getSpeed()))), &wNormal)*wNormal));
				enemy[i].setPosition(enemy[i].getOldPos());
			}
		}
		for(int j=0; j<buildings.size(); j++)
		{
			if(enemy[i].collided(&buildings[j])) 
			{
				////Player position before collision
				//D3DXVECTOR3 pPos = enemy[i].getOldPos();
				//D3DXVECTOR3 pVel = enemy[i].getVelocity();
				////D3DXVec3Normalize(&pVel, &pVel);
				//D3DXVECTOR3 wallPos = buildings[j].getPosition();
				//D3DXVECTOR3 wNormal(0, 0, 0); //normal of the wall surface where the player intersects
				////D3DXVECTOR3 refPoint = camera.getLookatDirection();
				//float depth = buildings[j].getDepth();
				//float width = buildings[j].getWidth();
				//float t = 0;
				////D1-4 represent points on each of the planes we want to check for intersection with
				////We are just donig ray-plane intersection here, with the ray origin being at the 
				//D3DXVECTOR3 D[4] = {D3DXVECTOR3(wallPos.x + width + 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x - width - 2.1, wallPos.y, wallPos.z), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z+depth + 2.1), D3DXVECTOR3(wallPos.x, wallPos.y, wallPos.z-depth - 2.1)};
				//D3DXVECTOR3 N[4] = {D3DXVECTOR3(1, 0, 0), D3DXVECTOR3(-1, 0, 0), D3DXVECTOR3(0, 0, 1), D3DXVECTOR3(0, 0, -1)};
				////find the minimum intersection time and the normal of the surface that we intersect with
				//for(int l=0; l<4; l++)
				//{
				//	float denom = D3DXVec3Dot(&N[l], &pVel);
				//	if(denom != 0)
				//	{
				//		float time = D3DXVec3Dot(&N[l], &(D[l]-pPos))/denom;
				//		if(time > 0 ){
				//			if(t <= 0 || time < t && t < 1){
				//				t = time;
				//				wNormal = N[l];
				//			}
				//		}
				//	}
				//}
				//D3DXVec3Normalize(&pVel, &pVel);
				//enemy[i].setPosition(enemy[i].getPosition() - (D3DXVec3Dot(&(enemy[i].getPosition()-(pPos + t*(pVel*enemy[i].getSpeed()))), &wNormal)*wNormal));
				enemy[i].setPosition(enemy[i].getOldPos());
			}
		}
	}
}

void ColoredCubeApp::placePickups() {
	if (!placedPickups) {
			int maxNightPickups;
			int maxDayPickups;
		if(level == 1){
			maxNightPickups = 8;
			maxDayPickups = 18;
		}
		else if(level == 2){
			maxNightPickups = 4;
			maxDayPickups = 25;
		}
			vector<int> choices;
			bool day = !night;
			if (day) {
				if (dayPickups.size() > 0) { //otherwise divide by zero when I mod by size
					vector<int> tempUsedIndices;
					for (int i = 0; i < maxDayPickups; i++) {
						bool add = true;
						int choice = rand()%dayPickups.size();
						for (unsigned int j = 0; j < tempUsedIndices.size(); j++) { //check that chosen dayPickup mapIndex isn't in the usedMapIndices
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
						for (unsigned int j = 0; j < tempUsedIndices.size(); j++) { //check that chosen dayPickup mapIndex isn't in the usedMapIndices
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
	
			for (unsigned int i = 0; i < nightPickups.size(); i++)
				nightPickups[i].setInActive();
			for (unsigned int i = 0; i < dayPickups.size(); i++)
				dayPickups[i].setInActive();

			if (day)
				for (unsigned int i = 0; i < choices.size(); i++) 
					dayPickups[choices[i]].setActive();
			else if (night) 
				for (unsigned int i = 0; i < choices.size(); i++) 
					nightPickups[choices[i]].setActive();


		

		placedPickups = true;
	}
}

void ColoredCubeApp::updatePickups(float dt) {
	
		for (unsigned int i = 0; i < dayPickups.size(); i++) {
			if (player.collided(&dayPickups[i])) {
				dayPickups[i].activate();
			}
			dayPickups[i].update(dt);
		}
		for (unsigned int i = 0; i < nightPickups.size(); i++) {
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
				
				placedPickups = false;

				int x = 0;
				for(int i=0; i<gameNS::MAX_NUM_ENEMIES && x < 4+(4*nightCount); i++)
				{
					if(!enemy[i].getActiveState())
					{
						enemy[i].setActive();
						enemy[i].setHealth(100);
						enemy[i].setPosition(enemy[i].waypointPositions()[rand()%enemy[i].waypointPositions().size()]);
						x++;
					}
				}

			}
			timeOfDay = "Night";
			mClearColor = gameNS::NIGHT_SKY_COLOR;
			mLights[0].diffuse  = D3DXCOLOR(0.1f, 0.1f, 0.1f, 1.0f);
			for(int i=3; i<gameNS::NUM_LIGHTS; i++)
			{
				mLights[i].att.y	= 0.05f;
			}
		}
		else
		{
			if (timeOfDay == "Dawn")
			{
				nightCount++;
				placedPickups = false;
				nightDayTrans = true;
			}
			timeOfDay = "Day";
			mClearColor = gameNS::DAY_SKY_COLOR;
			mLights[0].diffuse  = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			for(int i=3; i<gameNS::NUM_LIGHTS; i++)
			{
				mLights[i].att.y	= 0.55f;
			}
			dayCount++;
		}
	}
	if(timect >= gameNS::DAYLEN - gameNS::TRANSITIONTIME)
	{
		if(night) 
		{
			timeOfDay = "Dawn";
			mClearColor += D3DXCOLOR(((gameNS::DAY_SKY_COLOR.r-gameNS::NIGHT_SKY_COLOR.r)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.g-gameNS::NIGHT_SKY_COLOR.g)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.b-gameNS::NIGHT_SKY_COLOR.b)/(gameNS::TRANSITIONTIME))*dt, 1.0f);
			
			mLights[0].diffuse += D3DXCOLOR(((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0 -0.1)/(gameNS::TRANSITIONTIME))*dt, 0.0f);
			for(int i=3; i<gameNS::NUM_LIGHTS; i++)
			{
				mLights[i].att.y	+= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			}
		}
		else 
		{
			timeOfDay = "Evening";
			mClearColor -= D3DXCOLOR(((gameNS::DAY_SKY_COLOR.r-gameNS::NIGHT_SKY_COLOR.r)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.g-gameNS::NIGHT_SKY_COLOR.g)/(gameNS::TRANSITIONTIME))*dt, ((gameNS::DAY_SKY_COLOR.b-gameNS::NIGHT_SKY_COLOR.b)/(gameNS::TRANSITIONTIME))*dt, 1.0f);
			
			mLights[0].diffuse -= D3DXCOLOR(((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, ((1.0-0.1)/(gameNS::TRANSITIONTIME))*dt, 0.0f);
			for(int i=3; i<gameNS::NUM_LIGHTS; i++)
			{
				mLights[i].att.y	-= ((0.55-0.05f)/(gameNS::TRANSITIONTIME))*dt;
			}
		}
	}
}

void ColoredCubeApp::updateHUD(float dt) {
	for (unsigned int i = 0; i < hudObjects.size(); i++) {
		//hudObjects[i].setPosition(camera.getPosition() + camera.getLookatDirection());
		hudObjects[i].update(dt);
	}
}

void ColoredCubeApp::updateGameState() {
	//Handle possible transitions from the PLAYING state
	if (gameState == GameState::PLAYING) {
		if (player.getHealth() <= 0) {
			Sleep(2000);
			gameState = GameState::LOSE;
			input->keyUp(KEY_SPACE);
		}
		if (dayCount > gameNS::NUM_NIGHTS_TO_ADVANCE && level == 1) {
			Sleep(2000);
			gameState = GameState::BEATLV1;
			input->keyUp(KEY_SPACE);
			input->keyUp(KEY_0);
			nightCount = 0;
		}
		if (dayCount == gameNS::NUM_NIGHTS_TO_ADVANCE && level == 2) {
			Sleep(2000);
			gameState = GameState::WIN;
			input->keyUp(KEY_SPACE);
		}
		if (input->isKeyDown(KEY_0)) {
			nightCount = gameNS::NUM_NIGHTS_TO_ADVANCE;
			input->keyUp(KEY_SPACE);
			input->keyUp(KEY_0);
		}
	}

}


void ColoredCubeApp::drawScene()
{
	D3DApp::drawScene();
	incrementedYMargin = 5;
	lineHeight = 20;

	setDeviceAndShaderInformation();

	if(gameState == PLAYING) {	
		if(debugMode) for(int i=0; i<WAYPOINT_SIZE*WAYPOINT_SIZE; i++) wayLine[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVEnemy);
		mfxSpecMapVar->SetResource(mSpecMapRVEnemy);
		for(int i=0; i<gameNS::MAX_NUM_ENEMIES; i++)enemy[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);


		mVP = camera.getViewMatrix()*camera.getProjectionMatrix();

		drawBarrels();
		//drawHUD();
		drawFloor();
		drawBuildings();
		drawPickups();
		drawWalls();
		drawLamps();
		
		//Commenting this out because I think it does nothing. I think it's supposed to be drawing a menu.
		//D3D10_TECHNIQUE_DESC techDesc;
		//mTech->GetDesc( &techDesc );
		//for(UINT p = 0; p < techDesc.Passes; ++p) mWallMesh.draw();
		//Matrix mWVP = menu.getWorld() * (mVP);
		//mfxWVPVar->SetMatrix((float*)&mWVP);
		//mfxWorldVar->SetMatrix((float*)&menu.getWorld());
		//	
		//mTech->GetDesc( &techDesc );
		//for(UINT p = 0; p < techDesc.Passes; ++p) {
		//	mTech->GetPassByIndex( p )->Apply(0);
		//	//menu.draw();
		//}

		
		//Draw particle systems last besides text
		if (level == 2) {	

			md3dDevice->OMSetDepthStencilState(0, 0);
			float blendFactor[] = {0.0f, 0.0f, 0.0f, 0.0f};
			md3dDevice->OMSetBlendState(0, blendFactor, 0xffffffff);
			for(int i=0; i<gameNS::NUM_FIRES; i++)
			{
				mFire[i].setEyePos(camera.getPosition());
				mFire[i].draw();
			}
		}

		mfxDiffuseMapVar->SetResource(mDiffuseMapRVBullet);
		mfxSpecMapVar->SetResource(mSpecMapRVBullet);
		player.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);

		printText("Score: ", 20, 5, 0, 0, WHITE, player.getScore()); //This has to be the last thing in the draw function.
		printText("Health: ", 20, 25, 0, 0, RED, player.getHealth());
		printText("Ammo: ", 20, 45, 0, 0, BLUE, player.getAmmo());
		printText("Gun: ", 20, 65, 0, 0, gameNS::DARKGREEN, player.getGunName());
		printText(timeOfDay + " ", 670, 20, 0, 0, WHITE, dayCount);
		if(debugMode)printText("playerX = ", 20, 65, 0, 0, WHITE, player.getPosition().x);
		if(debugMode)printText("playerZ = ", 20, 85, 0, 0, WHITE, player.getPosition().z);
		printText("+", mClientWidth/2 - 2, mClientHeight/2-16, 0, 0, WHITE, "");
	}
	else if(gameState == INTROSCREEN)
	{
		mVP = camera.getViewMatrix()*camera.getProjectionMatrix();
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVIntroMenu);
		mfxSpecMapVar->SetResource(mSpecMapRVIntroMenu);
		menu.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
	else if (gameState == INSTRUCTIONS) {
		mVP = camera.getViewMatrix()*camera.getProjectionMatrix();
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVInstructionsMenu);
		mfxSpecMapVar->SetResource(mSpecMapRVInstructionsMenu);
		menu.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
	else if (gameState == BEATLV1) {
		mVP = camera.getViewMatrix()*camera.getProjectionMatrix();
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVLevel2);
		mfxSpecMapVar->SetResource(mSpecMapRVLevel2);
		menu.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
	else if (gameState == LOSE) { // End Screen 
		mVP = camera.getViewMatrix()*camera.getProjectionMatrix();
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVIWinMenu);
		mfxSpecMapVar->SetResource(mSpecMapRVIWinMenu);
		menu.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		printText("Score: ", 350, 280, 0, 0, WHITE, player.getScore());
	}
	else if (gameState == WIN) {
		mVP = camera.getViewMatrix()*camera.getProjectionMatrix();
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVYouWinMenu);
		mfxSpecMapVar->SetResource(mSpecMapRVYouWinMenu);
		menu.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
		printText("Score: ", 350, 280, 0, 0, WHITE, player.getScore());
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
	for(int i=0; i<walls.size(); i++)
		walls[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}

void ColoredCubeApp::drawBuildings() {
	//Set correct shader stuff
	if(level == 1) {
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVBuilding);
		mfxSpecMapVar->SetResource(mSpecMapRVBuilding);
	} else if (level == 2) {
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVBuilding2);
		mfxSpecMapVar->SetResource(mSpecMapRVBuilding2);
	}
	//draw
	for(int i=0; i<buildings.size(); i++)
		buildings[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
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
			printText(text.lines[i].s, xMargin, yMargin, 0, 0, WHITE, "");
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

void ColoredCubeApp::printText(string text, int rectPosX, int rectPosY, int rectWidth, int rectHeight, D3DXCOLOR color, string value) {
	
	RECT POS = {rectPosX, rectPosY, rectWidth, rectHeight};
	std::wostringstream outs;   
	outs.precision(6);
	if (value != "")
		outs << text.c_str() << value.c_str();
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
//#if defined(DEBUG) | defined(_DEBUG)
//	_CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
//#endif


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
	
	for (unsigned int i = 0; i < dayPickups.size(); i++)
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
	for (unsigned int i = 0; i < nightPickups.size(); i++) 
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
	for (int i = 0; i < lamps.size(); i++)
		lamps[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}

void ColoredCubeApp::drawHUD() {
	mfxDiffuseMapVar->SetResource(mDiffuseMapRV);
	mfxSpecMapVar->SetResource(mSpecMapRV);
	
	for(unsigned int i=0; i<hudObjects.size(); i++)
		hudObjects[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
}

void ColoredCubeApp::drawBarrels()
{
	if (level == 2) {
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVBarrel);
		mfxSpecMapVar->SetResource(mSpecMapRVBarrel);
		for(int i = 0; i < gameNS::NUM_BARRELS; i++)
			barrels[i].draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
}

void ColoredCubeApp::drawFloor() {
	if (level == 1){
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVStreet);
		mfxSpecMapVar->SetResource(mSpecMapRVStreet);
		floor.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
	else if (level == 2){
		mfxDiffuseMapVar->SetResource(mDiffuseMapRVTheRoad);
		mfxSpecMapVar->SetResource(mSpecMapRVTheRoad);
		floor2.draw(mfxWVPVar, mfxWorldVar, mTech, &mVP);
	}
	
}