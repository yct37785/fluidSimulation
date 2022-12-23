#pragma once
#include "SPH_Particle.h"

class SPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;
	// predefines
	float W_Scaler;
	float dW_Scaler;
	float acc_wMultiplier;

	vector<SPH_Particle> particles;

	// utils
	float cal_W(float r);
	float cal_dW(float r);

	// update
	void findNeighbors();
	void findDensity();
	void findPressure();
	void applyAcceleration(float t);

public:
	SPH_FluidGrid(int xCellsCount, int yCellsCount);
	~SPH_FluidGrid();

	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};