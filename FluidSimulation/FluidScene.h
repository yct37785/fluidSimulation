#pragma once
#include "MeshBuilder.h"
#include "Shader.h"

class FluidScene
{
	int xCellsCount, yCellsCount;
	glm::mat4 viewMat, projMat;
	float cursorPosX, cursorPosY = 0.f;
	int cursorCellX, cursorCellY = 0;
	Mesh* quadMesh;
	GridMesh* gridMesh;
	Shader* shader;

public:
	FluidScene();
	~FluidScene();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize(int width, int height);
	void mouseCallback(double xpos, double ypos);
};