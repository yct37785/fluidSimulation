#include "PressureSolve.h"
const float DEN_WATER = 1.f;	// water density = 1000 kg/m^3, but in simulation always set to 1
const float DEN_AIR = 1.f;	// air density = 1 kg/m^3

PressureSolve::PressureSolve(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	d.reserve(xCellsCount * yCellsCount);
	p.reserve(xCellsCount * yCellsCount);
	a.reserve(xCellsCount * yCellsCount);
	CGSolver::init(xCellsCount * yCellsCount);
}

PressureSolve::~PressureSolve()
{
}

bool PressureSolve::isValidCell(int x, int y)
{
	return x >= 0 && x < xCellsCount && y >= 0 && y < yCellsCount;
}

bool PressureSolve::isLiquidCell(int x, int y, unordered_map<int, int>& liquidCells)
{
	return isValidCell(x, y) && liquidCells.count(y * xCellsCount + x);
}

bool PressureSolve::addNeighborLiquidCell(int curr_map_idx, int x, int y, unordered_map<int, int>& liquidCells)
{
	if (isLiquidCell(x, y, liquidCells))
	{
		int map_idx = liquidCells[y * xCellsCount + x];
		a[curr_map_idx].push_back(map_idx);
		a[curr_map_idx].push_back(1.f);
		return true;
	}
	return false;
}

void PressureSolve::countSurroundingCellTypes(int x, int y, unordered_map<int, int>& liquidCells, int& air, int& liquid)
{
	air = liquid = 0;
	// top
	if (isValidCell(x, y + 1))
	{
		if (liquidCells.count((y + 1) * xCellsCount + x))
			liquid += 1;
		else
			air += 1;
	}
	// bottom
	if (isValidCell(x, y - 1))
	{
		if (liquidCells.count((y - 1) * xCellsCount + x))
			liquid += 1;
		else
			air += 1;
	}
	// left
	if (isValidCell(x - 1, y))
	{
		if (liquidCells.count(y * xCellsCount + x - 1))
			liquid += 1;
		else
			air += 1;
	}
	// right
	if (isValidCell(x + 1, y))
	{
		if (liquidCells.count(y * xCellsCount + x + 1))
			liquid += 1;
		else
			air += 1;
	}
}

float PressureSolve::getDerivative(VelocityField& uField, char comp, int x2, int y2, int x1, int y1)
{
	float v2 = 0.f, v1 = 0.f;
	v2 = uField.getCompByIdx(x2, y2, comp);
	v1 = uField.getCompByIdx(x1, y1, comp);
	return (v2 - v1) / H;
}

float PressureSolve::getLiquidCellPressure(int x, int y, unordered_map<int, int>& liquidCells)
{
	int idx = y * xCellsCount + x;
	if (liquidCells.count(idx))
		return p[liquidCells[idx]];
	else
		return 0.0;
}

void PressureSolve::update(VelocityField& uField, unordered_map<int, int>& liquidCells, float t)
{
	// reset
	d.clear();
	a.clear();
	p.clear();
	// resize
	d.resize(liquidCells.size(), 0.0);
	a.resize(liquidCells.size());
	p.resize(liquidCells.size(), 0.0);
	// for each liquid cell
	for (auto cell : liquidCells)
	{
		int map_idx = cell.second;
		int x = cell.first % xCellsCount;
		int y = cell.first / xCellsCount;
		int solidNeighbors = 0, liquidNeighbors = 0, airNeighbors = 0;
		countSurroundingCellTypes(x, y, liquidCells, airNeighbors, liquidNeighbors);
		// find divergence for center of cell
		float div = getDerivative(uField, 'x', x + 1, y, x, y) + getDerivative(uField, 'y', x, y + 1, x, y);
		// store divergence to D
		d[map_idx] = (div * ((DEN_WATER * H) / t));
		// add coefficients
		a[map_idx].clear();
		addNeighborLiquidCell(map_idx, x - 1, y, liquidCells);
		addNeighborLiquidCell(map_idx, x + 1, y, liquidCells);
		addNeighborLiquidCell(map_idx, x, y - 1, liquidCells);
		addNeighborLiquidCell(map_idx, x, y + 1, liquidCells);
		a[map_idx].push_back(map_idx);
		a[map_idx].push_back(-(liquidNeighbors + airNeighbors));
	}
	//// for each grid cell, pressure term P is situated in the center
	//for (int y = 0; y < yCellsCount; ++y)
	//{
	//	for (int x = 0; x < xCellsCount; ++x)
	//	{
	//		if (!liquidCells[y][x])
	//			continue;
	//		// some values
	//		float Patm = 1.f;
	//		int idx = y * xCellsCount + x;
	//		int solidNeighbors = 0, liquidNeighbors = 0, airNeighbors = 0;
	//		countSurroundingCellTypes(x, y, liquidCells, airNeighbors, liquidNeighbors);
	//		// find divergence for center of cell
	//		float div = getDerivative(uField, 'x', x + 1, y, x, y) + getDerivative(uField, 'y', x, y + 1, x, y);
	//		// store divergence to D
	//		a[idx].clear();
	//		// DO NOT ADD ATMOSPHERIC PRESSURE
	//		d[idx] = (div * ((DEN_WATER * H) / t));
	//		// add coefficients
	//		// why when using 1.f for coefficient Jacobi Method won't converge
	//		// https://stackoverflow.com/questions/24730993/jacobi-iteration-doesnt-end
	//		addNeighborLiquidCell(idx, x - 1, y, 1.f, liquidCells);
	//		addNeighborLiquidCell(idx, x + 1, y, 1.f, liquidCells);
	//		addNeighborLiquidCell(idx, x, y - 1, 1.f, liquidCells);
	//		addNeighborLiquidCell(idx, x, y + 1, 1.f, liquidCells);
	//		a[idx].push_back(idx);
	//		a[idx].push_back(-(liquidNeighbors + airNeighbors));
	//	}
	//}
	// solve P for AP = D
	JacobiMethod::solve(a, d, p);
	// CGSolver::solve(a, d, p);
	// update vel (boundary vels don't update)
	//for (int y = 0; y < yCellsCount; ++y)
	//{
	//	for (int x = 0; x < xCellsCount; ++x)
	//	{
	//		if (!liquidCells[y][x])
	//			continue;
	//		// x
	//		if (x > 0)
	//		{
	//			float nextP = p[y * xCellsCount + x];
	//			float prevP = p[y * xCellsCount + x - 1];
	//			float xp = (nextP - prevP) / H;
	//			uField.addToCompByIdx(x, y, 'x', -((t / (DEN_WATER * H)) * xp));
	//		}
	//		// y
	//		if (y > 0)
	//		{
	//			float nextP = p[y * xCellsCount + x];
	//			float prevP = p[(y - 1) * xCellsCount + x];
	//			float yp = (nextP - prevP) / H;
	//			uField.addToCompByIdx(x, y, 'y', -((t / (DEN_WATER * H)) * yp));
	//		}
	//	}
	//}
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		for (int x = 0; x < xCellsCount + 1; ++x)
		{
			// x
			if (x > 0 && y < yCellsCount)
			{
				//if (isLiquidCell(x, y, liquidCells) || isLiquidCell(x - 1, y, liquidCells))
				{
					float nextP = getLiquidCellPressure(x, y, liquidCells);
					float prevP = getLiquidCellPressure(x - 1, y, liquidCells);
					float xp = (nextP - prevP) / H;
					uField.addToCompByIdx(x, y, 'x', -((t / (DEN_WATER * H)) * xp));
				}
			}
			// y
			if (y > 0 && x < xCellsCount)
			{
				//if (isLiquidCell(x, y, liquidCells) || isLiquidCell(x, y - 1, liquidCells))
				{
					float nextP = getLiquidCellPressure(x, y, liquidCells);
					float prevP = getLiquidCellPressure(x, y - 1, liquidCells);
					float yp = (nextP - prevP) / H;
					uField.addToCompByIdx(x, y, 'y', -((t / (DEN_WATER * H)) * yp));
				}
			}
		}
	}
}
