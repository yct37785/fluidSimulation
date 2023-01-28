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

	// utilities
	bool isLiquidCell(int x, int y, std::unordered_map<int, int>& liquidCells);
	void countSurroundingCellTypes(int x, int y, std::unordered_map<int, int>& liquidCells, int& air, int& liquid);
	void addCoefficient(int curr_map_idx, int x, int y, std::unordered_map<int, int>& liquidCells);
	float getLiquidCellPressure(int x, int y, std::unordered_map<int, int>& liquidCells);

	// update
	void resetContainers(std::unordered_map<int, int>& liquidCells);
	void setupLinearEquations(VelocityField* uField, std::unordered_map<int, int>& liquidCells, float t);
	void solve(std::unordered_map<int, int>& liquidCells);
	void integration(VelocityField* uField, std::unordered_map<int, int>& liquidCells, float t);

public:
	PressureSolve();
	~PressureSolve();

	void update(VelocityField* uField, std::unordered_map<int, int>& liquidCells, float t);
};