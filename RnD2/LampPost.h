#ifndef LAMPPOST_H
#define LAMPPOST_H
#include "GameObject.h"

class LampPost : public GameObject
{
public:
	LampPost();
	~LampPost();

	//Width and height in integral number of boxes(bricks)
	void init(Box *b, Vector3 pos, float r = 1, float s = 1, int width = 1, int height = 1, int depth = 1, float rx = 0.0f, float ry = 0.0f, float rz = 0.0f);
	void draw(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP);
	void update(float dt);
	void drawWithWorld(ID3D10EffectMatrixVariable* mfxWVPVar, ID3D10EffectMatrixVariable* mfxWorldVar, ID3D10EffectTechnique* mTech, Matrix* mVP, Matrix transformation, bool glow = false);
	Matrix transform(Vector3 scale, Vector3 rotate, Vector3 translate);

	float getWidth(){return width;}
	float getHeight(){return height;}
	float getDepth(){return depth;}
	void giveGlowVar(ID3D10EffectScalarVariable* g) {
		mfxGlow = g;
	}

private:
	ID3D10EffectScalarVariable* mfxGlow;
	float radius;
	float radiusSquared;
};

#endif