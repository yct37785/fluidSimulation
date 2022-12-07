#pragma once
#include "VelocityField.h"
#include "JacobiMethod.h";

class PressureSolve
{
	vec d;
	vec p;
	matrix a;
	int xCellsCount, yCellsCount;

	bool isValidCell(int x, int y);
	bool addNeighborNonSolidCell(int idx, int x, int y, float v);

public:
	PressureSolve(int xCellsCount, int yCellsCount);
	~PressureSolve();

	void update(VelocityField& uField, bool** liquidCells, float t);
};