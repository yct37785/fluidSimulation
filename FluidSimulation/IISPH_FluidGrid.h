#pragma once
#include "IISPH_Particle.h"

class IISPH_FluidGrid
{
	// gridmesh for spatial partitioning visualisation
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;
	float viewWidth, viewHeight;

	std::vector<IISPH_Particle*> particles;
	std::unordered_map<int, std::vector<int>> neighborhoods;

	float avgRho;

	float W(float r);
	float gradW(float r);

	void loadNeighborhoods();
	void getNeighborsInclusive(std::vector<int>& neighbors, int currparticleIdx);

	// predict advection
	void PredictAdvection(float t);
	void computeDensity(float t);
	void part_1(float t);
	void part_2(float t);

	// pressure solve
	void PressureSolve(float t);
	void part_3(float t);
	void part_4(float t);

	// integration
	void Integration(float t);

public:
	IISPH_FluidGrid(int xCellsCount, int yCellsCount);
	~IISPH_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(int mvpHandle, glm::mat4& mvMat);
};