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
	float avgRho;

	vector<SPH_Particle*> particles;
	unordered_map<int, vector<int>> neighborhoods;

	// compute particle values
	void ComputeParticleValues(float t);
	void loadNeighborhoods();
	void getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx);
	void computeDensitiesAndFactors(float t, bool factor);
	// velocity update
	void VelocityUpdate(float t);
	void computeNonPressureForces(float t);
	void predictVelocities(float t);
	// correct density error
	void CorrectDensityError(float t);
	void computeUpdatedDensities(float t);
	// forward particles
	void forwardParticles(float t);
	float computeDensityMat();	// returns avg
	void correctDivergenceError(float t);

public:
	DFSPH_FluidGrid(int xCellsCount, int yCellsCount);
	~DFSPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};