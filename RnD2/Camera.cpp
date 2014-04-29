#include "Camera.h"

Camera::Camera()
{
	speed = 10;
	FoV = 0.25*PI;
	aspectRatio = 480.0f/640.0f;
	nearClippingPlane = 1.0f;
	farClippingPlane = 10000.0f;
	up = Vector3(0.0f, 1.0f, 0.0f);
	position = Vector3(-10,0,0);
	lookAt = Vector3(0.0f, 0.0f, 0.0f);
	yaw = 0;
	roll = 0;
	pitch = 0;
}

Camera::~Camera()
{
	//nothing to deallocate
}

void Camera::init(Input* input, Vector3 position, Vector3 direction, Vector3 _lookAt)
{
	Camera::position = position;
	Camera::input = input;
	setPerspective();
	//Generate an initial view matrix
	D3DXMatrixLookAtLH(&mView, &position, &lookAt, &up);
}

void Camera::setPerspective()
{
	D3DXMatrixPerspectiveFovLH(&mProj, FoV, aspectRatio, nearClippingPlane,farClippingPlane); 
}

void Camera::update(float dt)
{
	int dx = input->getMouseRawX();
	int dy = input->getMouseRawY();

	bool rotated = false;
	//float _speed = 100;
	float _speed = 6.0f;
	float deltaYaw = 0;
	float deltaPitch = 0;

	direction = Vector3(0,0,0);
	transformedReference = Vector3(1, 0, 0);

	Matrix yawR;
	Matrix pitchR;
	Matrix rollR;
	
	Identity(&yawR);
	Identity(&pitchR);
	Identity(&rollR);

	//if(input->isKeyDown(VK_RIGHT))
	if(dx > 0)
	{
		deltaYaw += _speed*dt;
		yaw += deltaYaw;
		rotated = true;
	}

	//if(input->isKeyDown(VK_LEFT))
	if(dx < 0)
	{
		rotated = true;
		deltaYaw -= _speed*dt;
		yaw += deltaYaw;
	}
	//if (input->isKeyDown(VK_UP))
	if(dy < 0)
	{
		rotated = true;
		deltaPitch += _speed*dt;
		if (deltaPitch > 1) 
			deltaPitch = 1;
		pitch += deltaPitch;
	}
	//if (input->isKeyDown(VK_DOWN))
	if(dy > 0)
	{
		rotated = true;
		deltaPitch -= _speed*dt;
		if (deltaPitch < -1) 
			deltaPitch = -1;
		pitch += deltaPitch;

	}

	if(input->isKeyDown(KEY_A))
			direction.z = 1;
	if(input->isKeyDown(KEY_D))
			direction.z = -1;
	if(input->isKeyDown(KEY_S))
			direction.x = -1;
	if(input->isKeyDown(KEY_W))
			direction.x = 1;
	
	//Generate rotation matrices
	if( pitch < -(PI/2.0f) + 0.01f)		pitch = -(PI/2.0f) + 0.01f;
	if( pitch > PI/2.0f - 0.01f)		pitch = (PI/2.0f) - 0.01f;
	D3DXMatrixRotationY(&yawR, yaw);
	D3DXMatrixRotationZ(&pitchR, pitch);
	
	//Update position
	if(direction != Vector3(0,0,0))
	{
		Transform(&direction, &direction, &(yawR));
		//Transform(&direction, &direction, &(yawR));
		D3DXVec3Normalize(&direction, &direction);
		position += direction * speed * dt;
	}

	//Update LookAt
	if (rotated)
	{
		Transform(&transformedReference, &transformedReference, &(pitchR * yawR));
		D3DXVec3Normalize(&transformedReference, &transformedReference);
		lookAt = transformedReference * speed ;
		lookAt += position;
		rotated = false;
	}
	else{
		lookAt += direction * speed * dt;
	}

	//Generate new matrix
	D3DXMatrixLookAtLH(&mView, &position, &lookAt, &up);
}
