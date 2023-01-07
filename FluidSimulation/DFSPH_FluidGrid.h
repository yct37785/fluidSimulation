#pragma once
#include "DFSPH_Particle.h"

class DFSPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;
	float viewWidth, viewHeight;
	bool first;

	vector<DFSPH_Particle*> particles;
	unordered_map<int, vector<int>> neighborhoods;

	float avgRho, avgRhoDivergence;

	void updateValues();

	float W(float r);
	float gradW(float r);

	void loadNeighborhoods();
	void getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx);

	// predict advection
	void PredictAdvection(float t);
	void computeDensitiesAndFactors(float t);

	// predict velocities
	void PredictVelocities(float t);
	void computeNonPressureForces(float t);
	void advanceVelocities(float t);

	// correct density
	void CorrectDensityError(float t);
	void predictDensity(float t);
	void adaptVelocities(float t);

	// update position
	void UpdatePositions(float t);

	// correct divergence error
	void CorrectDivergenceError(float t);
	void computeDensityDivergence(float t);
	void adaptVelocitiesDivSolver(float t);


	//void ComputeParticleValues(float t);
	//void loadNeighborhoods();
	//void getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx);
	//void computeDensitiesAndFactors(float t, bool factor);

	//// velocity update
	//void VelocityUpdate(float t);
	//void computeNonPressureForces(float t);
	//void predictVelocities(float t);

	//// correct density error
	//void CorrectDensityError(float t);
	//void computeUpdatedDensities(float t);

	//// forward particles
	//void forwardParticles(float t);
	//float computeDensityMat();	// returns avg
	//void correctDivergenceError(float t);

public:
	DFSPH_FluidGrid(int xCellsCount, int yCellsCount);
	~DFSPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};