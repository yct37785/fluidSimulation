#pragma once
#include "VelocityField.h"
#include "CGSolver.h";

class PressureSolve
{
	vec d;
	matrix a;
	int xCellsCount, yCellsCount;

	bool isValidCell(int x, int y);

public:
	PressureSolve(int xCellsCount, int yCellsCount);
	~PressureSolve();

	void update(VelocityField& uField, map<int, int>& liquidCells, float t);
};