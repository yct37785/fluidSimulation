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

	vector<SPH_Particle*> particles;

	void findDensityPressure();
	void computeForces();
	void integrate(float t);

public:
	SPH_FluidGrid(int xCellsCount, int yCellsCount);
	~SPH_FluidGrid();

	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};