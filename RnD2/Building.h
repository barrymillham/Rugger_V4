#ifndef BUILDING_H
#define BUILDING_H
#include "GameObject.h"

class Building : public GameObject
{
public:
	Building();
	~Building();

	//Width and height in integral number of boxes(bricks)
	virtual void init(Box *b, float r, Vector3 pos, float s = 1, int width = 1, int height = 1, int depth = 1, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
	virtual void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	virtual void update(float dt);

	//float getWidth(){
	//	return width;
	//}
	//float getHeight(){
	//	return height;
	//}
	//float getDepth(){
	//	return depth;
	//}

private:
	float radius;
	float radiusSquared;
};

#endif