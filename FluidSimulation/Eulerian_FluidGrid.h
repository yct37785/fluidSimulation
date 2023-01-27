#pragma once
#include "VelocityField.h"
#include "PressureSolve.h"

class Eulerian_FluidGrid
{
	GridMesh* gridMesh;
	Mesh* particleMesh;
	int xCellsCount, yCellsCount;

	// quantities
	VelocityField* uField;

	// pressure
	PressureSolve* ps;

	// particles
	std::vector<Particle*> particles;
	std::unordered_map<int, int> liquidCells;

	// update
	float getTimestep(float deltaTime);
	void updateLiquidCells();
	void advectParticles_Eulerian(float t);

public:
	Eulerian_FluidGrid(int xCellsCount, int yCellsCount);
	~Eulerian_FluidGrid();

	void spawnParticles();
	void Update(float deltaTime);
	void Draw(glm::mat4& mvMat, int mvpHandle);
};