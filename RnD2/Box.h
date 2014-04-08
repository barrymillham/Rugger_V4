//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

#include "d3dUtil.h"
#include "constants.h"

class Box
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale, ID3D10Effect* mFX);
	void init(ID3D10Device* device, float scale, D3DXCOLOR c, ID3D10Effect* mFX);
	void draw();

	void pullInVariables() {
		mfxCubeColorVar	= mFX->GetVariableByName("gCubeColor");
		mfxGlow			= mFX->GetVariableByName("gGlow")->AsScalar();
	}

	Vector3  getColor() {return boxColor;}
	ID3D10EffectScalarVariable* getGlowVar() {return mfxGlow;}
	ID3D10EffectVariable* getCubeColorVar() {return mfxCubeColorVar;}
	ID3D10Effect* getMFX() {return mFX;}
	ID3D10Device* getDevice() {return md3dDevice;}

private:
	DWORD mNumVertices;
	DWORD mNumFaces;
	bool glow;
	Vector3 boxColor;

	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	ID3D10EffectScalarVariable* mfxGlow;
	ID3D10EffectVariable* mfxCubeColorVar;
	ID3D10Effect* mFX;
};


#endif // BOX_H
