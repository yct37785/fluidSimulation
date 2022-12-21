#pragma once
#include "MeshBuilder.h"
#include "Shader.h"

class SPHFluidScene
{
	// fluid
	int xCellsCount, yCellsCount;
	// rendering/update
	glm::mat4 viewMat, projMat;
	float cursorPosX, cursorPosY = 0.f;
	int cursorCellX, cursorCellY = 0;
	Mesh* quadMesh;
	Shader* shader;

public:
	SPHFluidScene();
	~SPHFluidScene();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize(int width, int height);
	void mouseCallback(double xpos, double ypos);
};