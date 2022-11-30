#pragma once
#include "MeshBuilder.h"

class MACCell
{
	// center of grid cell
	float p;
	// minimal faces (i-1, j-1)
	float ux, uy;

public:
	MACCell();
	~MACCell();

	void Draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh, float x, float y);
};