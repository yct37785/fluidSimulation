#pragma once
#include "SPH_Particle.h"

class DFSPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;
	float viewWidth, viewHeight;
	// max
	float maxVel, maxA, maxC;

	vector<SPH_Particle*> particles;
	unordered_map<int, vector<int>> grids;

	void spatialPartitioning();
	void getNeighborsInclusive(vector<int>& neighbors, int curr);

	void findDensityPressure();
	void computeForces();
	void integrate(float t);

public:
	DFSPH_FluidGrid(int xCellsCount, int yCellsCount);
	~DFSPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};