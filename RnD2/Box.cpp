//=======================================================================================
// Box.cpp by Frank Luna (C) 2008 All Rights Reserved.
//=======================================================================================

#include "Box.h"
#include "Vertex.h"

Box::Box()
: mNumVertices(0), mNumFaces(0), md3dDevice(0), mVB(0), mIB(0)
{
}
 
Box::~Box()
{
	ReleaseCOM(mVB);
	ReleaseCOM(mIB);
}
void Box::init(ID3D10Device* device, float scale, D3DXCOLOR c)
{
	md3dDevice = device;
 
	mNumVertices = 24;
	mNumFaces    = 12; // 2 per quad

	D3DXCOLOR diffuse(1,1,1,1);
	D3DXCOLOR spec(1,1,1,1);

	// Create vertex buffer
    Vertex vertices[] =
    {
		//front - 0 1 2 3
		{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},

		//left - 7 6 1 0
		{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},

		//back - 4 5 6 7
		{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},

		//right - 3 2 5 4 
		{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},

		//top - 1 6 5 2
		{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},

		//bottom - 7 0 3 4
		{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
    };

	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
		vertices[i].pos *= scale;


    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));

	DWORD indices[] = {
		//front
		0, 1, 2, 
		0, 2, 3,
		//left
		4, 5, 6,
		4, 6, 7,
		//back
		8, 9, 10,
		8, 10, 11,
		//right
		12, 13, 14,
		12, 14, 15,
		//top
		16, 17, 18,
		16, 18, 19,
		//bottom
		20, 21, 22,
		20, 22, 23
	};

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
    HR(md3dDevice->CreateBuffer(&ibd, &iinitData, &mIB));
}
void Box::init(ID3D10Device* device, float scale)
{
	md3dDevice = device;
 
	mNumVertices = 24;
	mNumFaces    = 12; // 2 per quad
	D3DXCOLOR diffuse(1,1,1,1);
	D3DXCOLOR spec(1,1,1,1);

    Vertex vertices[] =
    {
		//front - 0 1 2 3
		{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), diffuse, spec},

		//left - 7 6 1 0
		{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), diffuse, spec},

		//back - 4 5 6 7
		{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), diffuse, spec},

		//right - 3 2 5 4 
		{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), diffuse, spec},

		//top - 1 6 5 2
		{D3DXVECTOR3(-1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, +1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, +1.0f, -1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), diffuse, spec},

		//bottom - 7 0 3 4
		{D3DXVECTOR3(-1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, -1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
		{D3DXVECTOR3(+1.0f, -1.0f, +1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), diffuse, spec},
    };


	// Scale the box.
	for(DWORD i = 0; i < mNumVertices; ++i)
		vertices[i].pos *= scale;


    D3D10_BUFFER_DESC vbd;
    vbd.Usage = D3D10_USAGE_IMMUTABLE;
    vbd.ByteWidth = sizeof(Vertex) * mNumVertices;
    vbd.BindFlags = D3D10_BIND_VERTEX_BUFFER;
    vbd.CPUAccessFlags = 0;
    vbd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA vinitData;
    vinitData.pSysMem = vertices;
    HR(md3dDevice->CreateBuffer(&vbd, &vinitData, &mVB));


	// Create the index buffer

	DWORD indices[] = {
		//front
		0, 1, 2, 
		0, 2, 3,
		//left
		4, 5, 6,
		4, 6, 7,
		//back
		8, 9, 10,
		8, 10, 11,
		//right
		12, 13, 14,
		13, 14, 15,
		//top
		16, 17, 18,
		16, 18, 19,
		//bottom
		20, 21, 22,
		20, 22, 23
	};

	D3D10_BUFFER_DESC ibd;
    ibd.Usage = D3D10_USAGE_IMMUTABLE;
    ibd.ByteWidth = sizeof(DWORD) * mNumFaces*3;
    ibd.BindFlags = D3D10_BIND_INDEX_BUFFER;
    ibd.CPUAccessFlags = 0;
    ibd.MiscFlags = 0;
    D3D10_SUBRESOURCE_DATA iinitData;
    iinitData.pSysMem = indices;
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