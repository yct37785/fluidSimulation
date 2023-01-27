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

	std::vector<DFSPH_Particle*> particles;
	std::unordered_map<int, std::vector<int>> neighborhoods;

	float avgRho, avgRhoDivergence;

	float W(float r);
	float gradW(float r);

	void loadNeighborhoods();
	void getNeighborsInclusive(std::vector<int>& neighbors, int currparticleIdx);

	float getTimeStep(float t);

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

	// integrate + bounds
	void IntegrateVelocityAndBounds();

public:
	DFSPH_FluidGrid(int xCellsCount, int yCellsCount);
	~DFSPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};