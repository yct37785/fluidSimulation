#pragma once
#include "SPH_Particle.h"

class SPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;
	float viewWidth, viewHeight;
	// max
	float maxVel, maxA, maxC;

	std::vector<SPH_Particle*> particles;
	std::unordered_map<int, std::vector<int>> grids;

	void spatialPartitioning();
	void getNeighborsInclusive(std::vector<int>& neighbors, int curr);

	void findDensityPressure();
	void computeForces();
	void integrate(float t);

public:
	SPH_FluidGrid(int xCellsCount, int yCellsCount);
	~SPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};