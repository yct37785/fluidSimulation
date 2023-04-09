#pragma once
#include "MeshBuilder.h"
#include "Shader.h"

class Scene
{

public:
	Scene() {};
	~Scene() {};

	virtual void Init() = 0;
	virtual void Update(bool inputList[INPUT_TOTAL], float deltaTime) = 0;
	virtual void Draw() = 0;

	virtual void windowsResize(int width, int height) = 0;
	virtual void mouseCallback(double xpos, double ypos) = 0;
};