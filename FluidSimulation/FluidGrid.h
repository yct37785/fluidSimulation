#pragma once
#include "PressureSolve2.h"
#include "VelocityField2.h"

class FluidGrid
{
	GridMesh* gridMesh;
	Mesh* triangleMesh;
	Mesh* markerMesh;
	int xCellsCount, yCellsCount;

	// quantities
	VelocityField2* uField2;
	PressureSolve2* ps2;

	// marker
	vector<glm::vec2> markers;
	bool** liquidCells;

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