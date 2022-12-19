#pragma once
#include "PressureSolve.h"
#include "VelocityField.h"

class FluidGrid
{
	GridMesh* gridMesh;
	Mesh* triangleMesh;
	Mesh* markerMesh;
	Mesh* fluidSourceMesh;
	int xCellsCount, yCellsCount;

	// quantities
	VelocityField* uField;
	PressureSolve* ps;

	// marker
	vector<glm::vec2> markers;
	unordered_map<int, int> liquidCells;

	// fluid source
	double spawnFluidTimer;
	glm::vec2 fluidSource;

	// updates
	float getTimeStep();

public:
	FluidGrid(int xCellsCount, int yCellsCount);
	~FluidGrid();

	void spawnFluid();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};