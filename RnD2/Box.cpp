//=======================================================================================
// Box.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Box.h"
#include "Vertex.h"

Box::Box()
: md3dDevice(0), mVB(0), mIB(0)
{
	diffuse = D3DXCOLOR(1,1,1,1);
	spec = D3DXCOLOR(1,1,1,1);
	mNumVertices = 24;
	mNumFaces    = 12; // 2 per quad
}
 
Box::~Box()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}
void Box::init(ID3D10Device* device, float scale, D3DXCOLOR c)
{
	md3dDevice = device;
 
	//mNumVertices = 24;
	//mNumFaces    = 12; // 2 per quad

	//D3DXCOLOR diffuse = c;
	//D3DXCOLOR spec = c;
	////D3DXCOLOR diffuse(0.5,0.5,1,1);

	//// Create vertex buffer
 //   Vertex vertices[] =
 //   {
	//	//front - 0 1 2 3
	//	{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},

	//	//left - 7 6 1 0
	//	{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},

	//	//back - 4 5 6 7
	//	{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},

	//	//right - 3 2 5 4 
	//	{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},

	//	//top - 1 6 5 2
	//	{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},

	//	//bottom - 7 0 3 4
	//	{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
	//	{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
 //   };

	//// Scale the box.
	//for(DWORD i = 0; i < mNumVertices; ++i)
	//	vertices[i].pos *= scale;


 //   D3D10_BUFFER_DESC vbd;
 //   vbd.Usage = D3D10_USAGE_IMMUTABLE;
 //   vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
 //   vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
 //   vbd.CPUAccessFlags = 0;
 //   vbd.MiscFlags = 0;
 //   D3D10_SUBRESOURCE_DATA vinitData;
 //   vinitData.pSysMem = vertices;
 //   HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	//DWORD indices[] = {
	//	//front
	//	//0, 1, 2,
	//	//0, 2, 3,
	//	0, 1, 2,
	//	3, 2, 0,
	//	//left
	//	//4, 5, 6,
	//	//4, 6, 7,
	//	4, 5, 6,
	//	7, 6, 4,
	//	//back
	//	//8, 9, 10,
	//	//8, 10, 11,
	//	8, 9, 10,
	//	11, 10, 8,
	//	//right
	//	//12, 13, 14,
	//	//12, 14, 15,
	//	12, 13, 14,
	//	15, 14, 12,
	//	//top
	//	//16, 17, 18,
	//	//16, 18, 19,
	//	16, 17, 18,
	//	19, 18, 16,
	//	//bottom
	//	//20, 21, 22,
	//	//20, 22, 23
	//	20, 21, 22,
	//	23, 22, 20
	//};

	//D3D10_BUFFER_DESC ibd;
 //   ibd.Usage = D3D10_USAGE_IMMUTABLE;
 //   ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
 //   ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
 //   ibd.CPUAccessFlags = 0;
 //   ibd.MiscFlags = 0;
 //   D3D10_SUBRESOURCE_DATA iinitData;
 //   iinitData.pSysMem = indices;
 //   HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
	mNumVertices = 24;
	mNumFaces    = 12; // 2 per quad

	// Create vertex buffer
    Vertex v[24];

	// Fill in the front face vertex data.
 	v[0] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[1] = Vertex(D3DXVECTOR3(-1.0f,  1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[2] = Vertex(D3DXVECTOR3( 1.0f,  1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[3] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Fill in the back face vertex data.
	v[4] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);
	v[5] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[6] = Vertex(D3DXVECTOR3( 1.0f,  1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[7] = Vertex(D3DXVECTOR3(-1.0f,  1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);

	// Fill in the top face vertex data.
	v[8]  = Vertex(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[9]  = Vertex(D3DXVECTOR3(-1.0f, 1.0f,  1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[10] = Vertex(D3DXVECTOR3( 1.0f, 1.0f,  1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[11] = Vertex(D3DXVECTOR3( 1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);
	v[13] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[14] = Vertex(D3DXVECTOR3( 1.0f, -1.0f,  1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[15] = Vertex(D3DXVECTOR3(-1.0f, -1.0f,  1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);

	// Fill in the left face vertex data.
	v[16] = Vertex(D3DXVECTOR3(-1.0f, -1.0f,  1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[17] = Vertex(D3DXVECTOR3(-1.0f,  1.0f,  1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[18] = Vertex(D3DXVECTOR3(-1.0f,  1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[19] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Fill in the right face vertex data.
	v[20] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[21] = Vertex(D3DXVECTOR3( 1.0f,  1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[22] = Vertex(D3DXVECTOR3( 1.0f,  1.0f,  1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[23] = Vertex(D3DXVECTOR3( 1.0f, -1.0f,  1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
		v[i].pos *= scale;


    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer

	DWORD i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = i;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Box::init(ID3D10Device* device, float scale) //I don't think this init function is ever called.
{
	//I don't think this init function is ever called.
	md3dDevice = device;

	mNumVertices = 24;
	mNumFaces    = 12; // 2 per quad

	// Create vertex buffer
    Vertex v[24];

	// Fill in the front face vertex data.
 	v[0] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[1] = Vertex(D3DXVECTOR3(-1.0f,  1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[2] = Vertex(D3DXVECTOR3( 1.0f,  1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[3] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Fill in the back face vertex data.
	v[4] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);
	v[5] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[6] = Vertex(D3DXVECTOR3( 1.0f,  1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[7] = Vertex(D3DXVECTOR3(-1.0f,  1.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);

	// Fill in the top face vertex data.
	v[8]  = Vertex(D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[9]  = Vertex(D3DXVECTOR3(-1.0f, 1.0f,  1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[10] = Vertex(D3DXVECTOR3( 1.0f, 1.0f,  1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[11] = Vertex(D3DXVECTOR3( 1.0f, 1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Fill in the bottom face vertex data.
	v[12] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);
	v[13] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[14] = Vertex(D3DXVECTOR3( 1.0f, -1.0f,  1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[15] = Vertex(D3DXVECTOR3(-1.0f, -1.0f,  1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);

	// Fill in the left face vertex data.
	v[16] = Vertex(D3DXVECTOR3(-1.0f, -1.0f,  1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[17] = Vertex(D3DXVECTOR3(-1.0f,  1.0f,  1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[18] = Vertex(D3DXVECTOR3(-1.0f,  1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[19] = Vertex(D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Fill in the right face vertex data.
	v[20] = Vertex(D3DXVECTOR3( 1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 1.0f), diffuse, spec);
	v[21] = Vertex(D3DXVECTOR3( 1.0f,  1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(0.0f, 0.0f), diffuse, spec);
	v[22] = Vertex(D3DXVECTOR3( 1.0f,  1.0f,  1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 0.0f), diffuse, spec);
	v[23] = Vertex(D3DXVECTOR3( 1.0f, -1.0f,  1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR2(1.0f, 1.0f), diffuse, spec);

	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
		v[i].pos *= scale;


    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = v;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer

	DWORD i[36];

	// Fill in the front face index data
	i[0] = 0; i[1] = 1; i[2] = 2;
	i[3] = 0; i[4] = 2; i[5] = 3;

	// Fill in the back face index data
	i[6] = 4; i[7]  = 5; i[8]  = 6;
	i[9] = 4; i[10] = 6; i[11] = 7;

	// Fill in the top face index data
	i[12] = 8; i[13] =  9; i[14] = 10;
	i[15] = 8; i[16] = 10; i[17] = 11;

	// Fill in the bottom face index data
	i[18] = 12; i[19] = 13; i[20] = 14;
	i[21] = 12; i[22] = 14; i[23] = 15;

	// Fill in the left face index data
	i[24] = 16; i[25] = 17; i[26] = 18;
	i[27] = 16; i[28] = 18; i[29] = 19;

	// Fill in the right face index data
	i[30] = 20; i[31] = 21; i[32] = 22;
	i[33] = 20; i[34] = 22; i[35] = 23;

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
	ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = i;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}

void Box::draw()
{
	UINT stride = sizeof(Vertex);
    UINT offset = 0;
	md3dDevice->IASetPrimitiveTopology(D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    md3dDevice->IASetVertexBuffers(0, 1, &mVB, &stride, &offset);
	md3dDevice->IASetIndexBuffer(mIB, DXGI_FORMAT_R32_UINT, 0);
	md3dDevice->DrawIndexed(mNumFaces*3, 0, 0);
}