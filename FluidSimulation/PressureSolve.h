#pragma once
#include "VelocityField.h"
#include "JacobiMethod.h"
#include "CGSolver.h"

class PressureSolve
{
	vec d;
	vec p;
	matrix a;
	int xCellsCount, yCellsCount;

	bool isValidCell(int x, int y);
	bool addNeighborLiquidCell(int idx, int x, int y, float v, bool** liquidCells);
	void countSurroundingCellTypes(int x, int y, bool** liquidCells, int& air, int& liquid);
	float getDerivative(VelocityField& uField, char comp, int x2, int y2, int x1, int y1);

public:
	PressureSolve(int xCellsCount, int yCellsCount);
	~PressureSolve();

	void update(VelocityField& uField, bool** liquidCells, float t);
};