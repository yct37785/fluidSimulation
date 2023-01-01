#pragma once
#include "VelocityField.h"

class Hybrid_FluidGrid
{
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;

	// quantities
	VelocityField* uField;

public:
	Hybrid_FluidGrid(int xCellsCount, int yCellsCount);
	~Hybrid_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(glm::mat4& mvMat, int mvpHandle);
};