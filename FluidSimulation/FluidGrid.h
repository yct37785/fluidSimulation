#pragma once
#include "MACCell.h"

class FluidGrid
{
	GridMesh* gridMesh;
	int xCellsCount, yCellsCount;

	// debug
	Mesh* triangleMesh;

	// quantities
	MACCell** gridCells;

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