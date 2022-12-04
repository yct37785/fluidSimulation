#pragma once
#include "MeshBuilder.h"

class PressureSolve
{
	float** p;
	int xCellsCount, yCellsCount;

public:
	PressureSolve(int xCellsCount, int yCellsCount);
	~PressureSolve();
};