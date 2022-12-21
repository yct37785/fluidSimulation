#pragma once
#include "MAC_PressureSolve.h"
#include "MAC_VelocityField.h"

class MAC_FluidGrid
{
	GridMesh* gridMesh;
	Mesh* triangleMesh;
	Mesh* markerMesh;
	Mesh* fluidSourceMesh;
	int xCellsCount, yCellsCount;

	// quantities
	MAC_VelocityField* uField;
	MAC_PressureSolve* ps;

	// marker
	vector<glm::vec2> markers;
	unordered_map<int, int> liquidCells;

	// fluid source
	double spawnFluidTimer;
	glm::vec2 fluidSource;

	// updates
	float getTimeStep();

public:
	MAC_FluidGrid(int xCellsCount, int yCellsCount);
	~MAC_FluidGrid();

	void spawnFluid();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};