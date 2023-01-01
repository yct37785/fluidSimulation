#pragma once
#include "VelocityField.h"
#include "JacobiMethod.h"
#include "CGSolver.h"

/*
* Solve for p in aP = d
*/
class PressureSolve
{
	vec d;
	vec p;
	matrix a;
	int xCellsCount, yCellsCount;

	// utilities
	bool outOfBounds(int x, int y);
	bool isLiquidCell(int x, int y, unordered_map<int, int>& liquidCells);
	void countSurroundingCellTypes(int x, int y, unordered_map<int, int>& liquidCells, int& air, int& liquid);
	void addCoefficient(int curr_map_idx, int x, int y, unordered_map<int, int>& liquidCells);
	float getLiquidCellPressure(int x, int y, unordered_map<int, int>& liquidCells);

	// update
	void resetContainers(unordered_map<int, int>& liquidCells);
	void setupLinearEquations(VelocityField* uField, unordered_map<int, int>& liquidCells, float t);
	void solve(unordered_map<int, int>& liquidCells);
	void integration(VelocityField* uField, unordered_map<int, int>& liquidCells, float t);

public:
	PressureSolve(int xCellsCount, int yCellsCount);
	~PressureSolve();

	void update(VelocityField* uField, unordered_map<int, int>& liquidCells, float t);
};