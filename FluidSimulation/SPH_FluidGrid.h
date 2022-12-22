#pragma once
#include "SPH_Particle.h"

class SPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;

	vector<SPH_Particle> particles;

public:
	SPH_FluidGrid(int xCellsCount, int yCellsCount);
	~SPH_FluidGrid();

	void spawnFluid();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};