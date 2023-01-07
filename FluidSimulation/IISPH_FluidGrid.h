#pragma once
#include "SPH_Particle.h"

class IISPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;
	float viewWidth, viewHeight;

	vector<SPH_Particle*> particles;
	unordered_map<int, vector<int>> neighborhoods;

	void loadNeighborhoods();
	void getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx);
	// predict advection
	void PredictAdvection(float t);
	void compute_vadv_dii(float t);
	void compute_aii(float t);

public:
	IISPH_FluidGrid(int xCellsCount, int yCellsCount);
	~IISPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};