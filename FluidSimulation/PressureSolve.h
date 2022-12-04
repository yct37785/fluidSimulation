#pragma once
#include "VelocityField.h"

class PressureSolve
{
	float** p;
	int xCellsCount, yCellsCount;

public:
	PressureSolve(int xCellsCount, int yCellsCount);
	~PressureSolve();

	void update(VelocityField& uField, map<int, int>& liquidCells);
};