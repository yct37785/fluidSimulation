#pragma once
#include "MeshBuilder.h"
#include "Shader.h"
#include "Eulerian_FluidGrid.h"

class EulerianFluidScene
{
	// fluid
	int xCellsCount, yCellsCount;
	Eulerian_FluidGrid* fluidGrid;
	// rendering/update
	glm::mat4 viewMat, projMat;
	Shader* shader;
	// state
	double spawnParticlesTimer;

public:
	EulerianFluidScene();
	~EulerianFluidScene();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize(int width, int height);
	void mouseCallback(double xpos, double ypos);
};