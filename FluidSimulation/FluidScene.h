#pragma once
#include "MeshBuilder.h"
#include "Shader.h"

class FluidScene
{
	glm::mat4 viewMat, projMat, cursorPosMat;
	Mesh* quadMesh;
	Shader* shader;

public:
	FluidScene();
	~FluidScene();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize();
	void mouse_callback(double xpos, double ypos);
};