#pragma once
#include "MeshBuilder.h"

class MACCell
{
	int state;	// 0: air, 1: liquid, 2: solid
	// center of grid cell
	float p;
	// minimal faces (i-1, j-1)
	float ux, uy;
	// pos
	int posX, posY;

public:
	MACCell();
	~MACCell();

	void setPos(int posX, int posY);

	void Advect(MACCell& tl, MACCell& tr, MACCell& bl, MACCell& br);

	void Draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);
};