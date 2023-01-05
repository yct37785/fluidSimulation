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
	unordered_map<int, vector<int>> neighborhoods;

	void loadNeighborhoods();
	void getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx);
	void computeDensitiesAndFactors(float t);
	void computeNonPressureForces(float t);
	void predictVelocities(float t);
	void correctDensityError(float t);

public:
	DFSPH_FluidGrid(int xCellsCount, int yCellsCount);
	~DFSPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};