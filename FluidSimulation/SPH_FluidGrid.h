#pragma once
#include "SPH_Particle.h"

class SPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;
	// predefines
	float wMultiplier;
	float acc_wMultiplier;

	vector<SPH_Particle> particles;

	// utils
	float calW(float r);

public:
	SPH_FluidGrid(int xCellsCount, int yCellsCount);
	~SPH_FluidGrid();

	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};