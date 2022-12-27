#pragma once
#include "MAC_VelocityField.h"
#include "JacobiMethod.h"
#include "CGSolver.h"

class MAC_PressureSolve
{
	vec d;
	vec p;
	matrix a;
	int xCellsCount, yCellsCount;

	bool isValidCell(int x, int y);
	bool isLiquidCell(int x, int y, unordered_map<int, int>& liquidCells);
	bool addNeighborLiquidCell(int curr_map_idx, int x, int y, unordered_map<int, int>& liquidCells);
	void countSurroundingCellTypes(int x, int y, unordered_map<int, int>& liquidCells, int& air, int& liquid);
	float getDerivative(MAC_VelocityField& uField, char comp, int x2, int y2, int x1, int y1);
	float getLiquidCellPressure(int x, int y, unordered_map<int, int>& liquidCells);
	float getAvgOf4Neighbors(MAC_VelocityField& uField, int x, int y, char comp);

public:
	MAC_PressureSolve(int xCellsCount, int yCellsCount);
	~MAC_PressureSolve();

	void update(MAC_VelocityField& uField, unordered_map<int, int>& liquidCells, float t);
};