#pragma once
#include "PressureSolve.h"
#include "VelocityField.h"

class FluidGrid
{
	GridMesh* gridMesh;
	Mesh* triangleMesh;
	Mesh* markerMesh;
	int xCellsCount, yCellsCount;

	// quantities
	VelocityField* uField2;
	PressureSolve* ps2;

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