#pragma once
#include "VelocityField2.h"
#include "JacobiMethod.h";

class PressureSolve2
{
	vec d;
	vec p;
	matrix a;
	int xCellsCount, yCellsCount;

	bool isValidCell(int x, int y);
	bool addNeighborLiquidCell(int idx, int x, int y, float v, bool** liquidCells);
	void countSurroundingCellTypes(int x, int y, bool** liquidCells, int& air, int& liquid);
	float getDerivative(VelocityField2& uField, char comp, int x2, int y2, int x1, int y1);

public:
	PressureSolve2(int xCellsCount, int yCellsCount);
	~PressureSolve2();

	void update(VelocityField2& uField, bool** liquidCells, float t);
};