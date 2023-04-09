#pragma once
#include "Scene.h"
#include "Eulerian_FluidGrid.h"

class Scene_EulerianFluid : public Scene
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
	Scene_EulerianFluid();
	~Scene_EulerianFluid();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize(int width, int height);
	void mouseCallback(double xpos, double ypos);
};