#pragma once
#include "Scene.h"

class Scene_SpatialHashing : public Scene
{
	
public:
	Scene_SpatialHashing();
	~Scene_SpatialHashing();

	void Init();
	void Update(bool inputList[INPUT_TOTAL], float deltaTime);
	void Draw();

	void windowsResize(int width, int height);
	void mouseCallback(double xpos, double ypos);
};