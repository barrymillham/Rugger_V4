#ifndef _CAMERA_H                // Prevent multiple definitions if this 
#define _CAMERA_H 

#include "constants.h"
#include "d3dUtil.h"
#include "d3dApp.h"
#include <d3dx9math.h>
#include "input.h"

class Camera
{
public:
	Camera();
	~Camera();
	void init(Input* input, Vector3 position, Vector3 direction, Vector3 _lookAt);
	void update(float dt, float playerSpeeed, bool* walking);

	Matrix getViewMatrix() {return mView;}
	Matrix getProjectionMatrix() {return mProj;}
	
	void setPosition(Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setDirection(Vector3 dir) {direction = dir;}
	Vector3 getDirection() {return direction;}
	void setSpeed(float s) {speed = s;}
	float getSpeed () {return speed;}
	Vector3 getLookatDirection(){return lookAt - position;}
	Vector3 getRightVec() {return right;}
	void setFoV(float fov){FoV = fov;}
	float getFoV() {return FoV;}
	void flying(bool b){fly = b;}
	void falling(bool b){fall = b;}
	void transformToWorld(Vector3 position = Vector3(-1.0f, -1.0f, -1.0f), Vector3 axis = Vector3(0.0f,0.0f,1.0f));
	void transformToMenu();
	void setPerspective();
	void setPitch(float p) {pitch = p;}

private:
	Matrix mView;
	Matrix mProj;
	Vector3 position;
	Vector3 direction; //direction to walk (not look)
	float speed;
	float aspectRatio;
	float FoV;
	float nearClippingPlane;
	float farClippingPlane;
	Vector3 lookAt;
	Vector3 up;
	Vector3 right;
	float yaw;
	float pitch;
	float roll;
	Vector3 transformedReference; //direction to look
	bool fly;
	bool fall;

	Input* input;
};
#endif