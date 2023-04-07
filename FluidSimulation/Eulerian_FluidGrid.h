#pragma once
#include "VelocityField.h"
#include "PressureSolve.h"

class Eulerian_FluidGrid
{
	GridMesh* gridMesh;
	Mesh* particleMesh;

	// quantities
	VelocityField* uField;

	// pressure
	PressureSolve* ps;

	// particles
	std::vector<Particle*> particles;
	CELL_TYPES** cellType;
	std::unordered_map<int, int> liquidCells;

	// update
	float getTimestep(float deltaTime);
	void updateLiquidCells();
	void advectParticles_Eulerian(float t);

public:
	Eulerian_FluidGrid();
	~Eulerian_FluidGrid();

	void spawnParticles(float xmin, float xmax, float ymin, float ymax, float space);
	void Update(float deltaTime);
	void Draw(glm::mat4& mvMat, int mvpHandle);
};