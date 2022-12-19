#include "PressureSolve.h"
const float DEN_WATER = 1000.f;	// water density = 1000 kg/m^3, but in simulation always set to 1
const float DEN_AIR = 1.f;	// air density = 1 kg/m^3

PressureSolve::PressureSolve(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	d.reserve(xCellsCount * yCellsCount);
	p.reserve(xCellsCount * yCellsCount);
	a.reserve(xCellsCount * yCellsCount);
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
	// solve P for AP = D
	//JacobiMethod::solve(a, d, p);
	CGSolver::init(liquidCells.size());
	CGSolver::solve(a, d, p);
	// update vel (boundary vels don't update)
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		for (int x = 0; x < xCellsCount + 1; ++x)
		{
			// x
			if (x > 0 && y < yCellsCount)
			{
				// border liquid
				if (isLiquidCell(x, y, liquidCells) || isLiquidCell(x - 1, y, liquidCells))
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
				// border liquid
				if (isLiquidCell(x, y, liquidCells) || isLiquidCell(x, y - 1, liquidCells))
				{
					float nextP = getLiquidCellPressure(x, y, liquidCells);
					float prevP = getLiquidCellPressure(x, y - 1, liquidCells);
					float yp = (nextP - prevP) / H;
					uField.addToCompByIdx(x, y, 'y', -((t / (DEN_WATER * H)) * yp));
				}
			}
		}
	}
	// extrapolate vels
	uField.postUpdate();
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		for (int x = 0; x < xCellsCount + 1; ++x)
		{
			// for each non-fluid cell, for vel comps not bordering fluid cells
			// set to average of neighbors of C that are fluid
			// x
			if (x > 0 && y < yCellsCount)
			{
				// not border liquid
				if (!isLiquidCell(x, y, liquidCells) && !isLiquidCell(x, y - 1, liquidCells))
				{
					// avg of 4 neighbors
					float xvals[4];
					xvals[0] = uField.getCompByIdx(x + 1, y, 'x');
					xvals[1] = uField.getCompByIdx(x - 1, y, 'x');
					xvals[2] = uField.getCompByIdx(x, y + 1, 'x');
					xvals[3] = uField.getCompByIdx(x, y - 1, 'x');
					float total = 0.f;
					float count = 0.f;
					for (int i = 0; i < 4; ++i)
					{
						if (xvals[i] != -1.f)
						{
							total += xvals[i];
							count += 1.f;
						}
					}
					uField.setCompByIdx(x, y, 'x', total / count);
				}
			}
			// y
			if (y > 0 && x < xCellsCount)
			{
				// not border liquid
				if (!isLiquidCell(x, y, liquidCells) && !isLiquidCell(x - 1, y, liquidCells))
				{
					// avg of 4 neighbors
					float xvals[4];
					xvals[0] = uField.getCompByIdx(x + 1, y, 'y');
					xvals[1] = uField.getCompByIdx(x - 1, y, 'y');
					xvals[2] = uField.getCompByIdx(x, y + 1, 'y');
					xvals[3] = uField.getCompByIdx(x, y - 1, 'y');
					float total = 0.f;
					float count = 0.f;
					for (int i = 0; i < 4; ++i)
					{
						if (xvals[i] != -1.f)
						{
							total += xvals[i];
							count += 1.f;
						}
					}
					uField.setCompByIdx(x, y, 'y', total / count);
				}
			}
			// no slip: vel comps that point into solid will be set to 0
			if ((x == 0 || x == xCellsCount) && y < yCellsCount)
				uField.setCompByIdx(x, y, 'x', 0.f);
			if ((y == 0 || y == yCellsCount) && x < xCellsCount)
				uField.setCompByIdx(x, y, 'y', 0.f);
		}
	}
}
