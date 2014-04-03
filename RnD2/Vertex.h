

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
	Vertex(D3DXVECTOR3 p, D3DXVECTOR3 n, D3DXCOLOR d, D3DXCOLOR s, D3DXVECTOR2 t):
		pos(p), normal(n), diffuse(d), spec(s), texc(t) {}
	Vertex(D3DXVECTOR3 p, D3DXCOLOR c): pos(p), diffuse(c), spec(c) {}
	D3DXVECTOR3 pos;
	D3DXVECTOR3 normal;
	D3DXCOLOR   diffuse;
	D3DXCOLOR   spec; // (r, g, b, specPower);
	D3DXVECTOR2 texc;
};
 
#endif // VERTEX_H