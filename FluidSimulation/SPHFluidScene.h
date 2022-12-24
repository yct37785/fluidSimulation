#pragma once
#include "MeshBuilder.h"
#include "Shader.h"
#include "SPH_FluidGrid.h"

class SPHFluidScene
{
	// fluid
	int xCellsCount, yCellsCount;
	SPH_FluidGrid* fluidGrid;
	// rendering/update
	glm::mat4 viewMat, projMat;
	float cursorPosX, cursorPosY = 0.f;
	int cursorCellX, cursorCellY = 0;
	Mesh* quadMesh;
	Shader* shader;
	bool spawnParticles;

public:
	SPHFluidScene();
	~SPHFluidScene();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize(int width, int height);
	void mouseCallback(double xpos, double ypos);
};