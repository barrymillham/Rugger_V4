

#ifndef VERTEX_H
#define VERTEX_H

//#include "d3dUtil.h"
#include "d3dUtil.h"

//struct Vertex
//{
//	D3DXVECTOR3 pos;
//	D3DXCOLOR   color;
//};
struct Vertex
{
	Vertex() {}
	Vertex(D3DXVECTOR3 p, D3DXVECTOR3 n, D3DXVECTOR2 t, D3DXCOLOR dif, D3DXCOLOR sp):
		pos(p), normal(n), texC(t), diffuse(dif), spec(sp) {}
	Vertex(D3DXVECTOR3 p, D3DXCOLOR c):
		pos(p), diffuse(c), spec(c) {}
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXVECTOR2 texC;
	D3DXCOLOR   diffuse;
	D3DXCOLOR   spec; // (r, g, b, specPower);
};
 
#endif // VERTEX_H