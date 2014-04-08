
#ifndef GameObject_H
#define GameObject_H

//#include "d3dUtil.h"
#include "d3dUtil.h"
#include "Box.h"
#include "constants.h"

class GameObject
{
public:

	GameObject();
	~GameObject();
	//				   geom,  rad,  position,				sc,	w,		h,	d
	//walls[0].init(&brick, 2.0f, Vector3(155, 0, 250), 	1,	115,	10, 10);//	Left/Front wall 
	void init(Box *b, float r, Vector3 pos, Vector3 vel, float sp, float s = 1.0f, float w = 1.0f, float h = 1.0f, float d = 1.0f);

	virtual void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	virtual void drawWithWorld(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP, Matrix transformation);
	virtual void update(float dt);

	void setPosition (Vector3 pos) {position = pos;}
	Vector3 getPosition() {return position;}
	void setVelocity (Vector3 vel) {velocity = vel;}
	Vector3 getVelocity() {return velocity;}
	void setSpeed(float s) {speed = s;}
	float getSpeed() {return speed;}
	void setRadius(float r) {radius = r; radiusSquared = (scale*r)*(scale*r);}
	float getRadiusSquare() {return radiusSquared;}
	float getRadius() {return radius;}
	Matrix getWorldMatrix() {return world;}
	void setScale(float s) {scale = s; radiusSquared = (s*radius)*(s*radius);}
	float getScale() {return scale;}
	void setActive() {active = true;}
	void setInActive() {active = false;}
	bool getActiveState() {return active;}
	bool collided(GameObject *gameObject);
	void faceObject(GameObject *o);
	void faceObject(Vector3 coordinate);
	void stopFacing(){facing = false;}
	void setRotateX(float rx){rotX = rx;}
	void setRotateY(float ry){rotY = ry;}
	void setRotateZ(float rz){rotZ = rz;}
	void startGlowing(){glow = true;}
	void stopGlowing() {glow = false;}

	Matrix transform(Vector3 scale, Vector3 rotate, Vector3 translate);

	//float getWidth(){return scale;}
	virtual float getWidth(){return width;}
	virtual float getHeight(){return 2*height;}
	virtual float getDepth(){return depth;}

	void setBox(Box* b){box = b;}

private:
	float speed;
	
protected:
	Box *box;
	Vector3 position;
	Matrix world;
	float scale;
	float rotX, rotY, rotZ;
	bool active;
	float radius;
	float radiusSquared;
	Vector3 velocity;
	float width, height, depth;
	Matrix mTranslate, mRotate, mScale;
	Matrix transformation;
	bool facing;
	GameObject* facedObject;
	Vector3 facedCoordinate;
	ID3D10EffectScalarVariable* mfxGlow;
	ID3D10EffectVariable* mfxCubeColorVar;
	bool glow;
};


#endif
