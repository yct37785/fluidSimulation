#pragma once
#include "VelocityField.h"

class FluidGrid
{
	GridMesh* gridMesh;
	Mesh* triangleMesh;
	int xCellsCount, yCellsCount;

	// quantities
	VelocityField* uField;

	// tracker
	glm::vec2 maxU;

	//utils
	glm::vec2 getVelocityBilinear(float x, float y);

	// updates
	float getTimeStep();

public:
	FluidGrid(int xCellsCount, int yCellsCount);
	~FluidGrid();

	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};