#pragma once
#include "MeshBuilder.h"

class FluidGrid
{
	GridMesh* gridMesh;
	int xCellsCount, yCellsCount;
	int xBorderCount, yBorderCount;

	// debug
	Mesh* triangleMesh;

	// quantities
	// per MAC specifications, velocity indices are off by -1/2
	float** pField;
	float** uField;
	float** vField;

	// tracker
	glm::vec2 maxU;

	//utils
	glm::vec2 getInterVelocity(float x, float y);

	// updates
	float getDeltaTime();

public:
	FluidGrid(int xCellsCount, int yCellsCount);
	~FluidGrid();

	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};