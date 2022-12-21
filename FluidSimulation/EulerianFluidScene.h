#pragma once
#include "MeshBuilder.h"
#include "Shader.h"
#include "MAC_FluidGrid.h"

class EulerianFluidScene
{
	// fluid
	int xCellsCount, yCellsCount;
	MAC_FluidGrid* fluidGrid;
	// state
	double spawnFluidTimer;
	bool unfreezeFluid;
	// rendering/update
	glm::mat4 viewMat, projMat;
	float cursorPosX, cursorPosY = 0.f;
	int cursorCellX, cursorCellY = 0;
	Mesh* quadMesh;
	Shader* shader;

public:
	EulerianFluidScene();
	~EulerianFluidScene();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize(int width, int height);
	void mouseCallback(double xpos, double ypos);
};