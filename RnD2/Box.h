//=======================================================================================
// Box.h by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#ifndef BOX_H
#define BOX_H

//#include "d3dUtil.h"
#include "d3dUtil.h"

class Box
{
public:

	Box();
	~Box();

	void init(ID3D10Device* device, float scale, ID3D10EffectScalarVariable* mfxGlow);
	void init(ID3D10Device* device, float scale, D3DXCOLOR c, ID3D10EffectScalarVariable* mfxGlow);
	void draw();
	void toggleGlow() {glow = true;}
	ID3D10EffectScalarVariable* getGlowVar() {return mfxGlow;}
	//static const int NUM_VERTICES = 24;
	//static const int NUM_INDEXES = 36;

private:
	DWORD mNumVertices;
	DWORD mNumFaces;
	bool glow;

	D3DXCOLOR diffuse;
	D3DXCOLOR spec;
	ID3D10Device* md3dDevice;
	ID3D10Buffer* mVB;
	ID3D10Buffer* mIB;
	ID3D10EffectScalarVariable* mfxGlow;
};


#endif // BOX_H
