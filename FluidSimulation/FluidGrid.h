#pragma once
#include "Utility.h"

class FluidGrid
{
	int xCellsCount, yCellsCount;
	int xBorderCount, yBorderCount;

	// quantities
	// per MAC specifications, velocity indices are off by -1/2
	glm::vec2** pressure;
	glm::vec2** velocity;

	// tracker
	glm::vec2 maxVelocity;

	// updates
	float getDeltaTime();

	//utils

public:
	FluidGrid(int xCellsCount, int yCellsCount);
	~FluidGrid();

	void Update();
};