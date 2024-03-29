#include "PressureSolve.h"
const float EULERIAN_DEN_WATER = 1000.f;	// water density = 1000 kg/m^3
const float EULERIAN_DEN_AIR = 1.f;	// air density = 1 kg/m^3

PressureSolve::PressureSolve()
{
	// max size to 
	d.reserve(p_GridInfo->xCells * p_GridInfo->yCells);
	p.reserve(p_GridInfo->xCells * p_GridInfo->yCells);
	a.reserve(p_GridInfo->xCells * p_GridInfo->yCells);
}

PressureSolve::~PressureSolve()
{
}

bool PressureSolve::isLiquidCell(int x, int y, std::unordered_map<int, int>& liquidCells)
{
	return p_GridInfo->inBounds(x, y) && liquidCells.count(y * p_GridInfo->xCells + x);
}

void PressureSolve::countSurroundingCellTypes(int x, int y, std::unordered_map<int, int>& liquidCells, int& air, int& liquid)
{
	air = liquid = 0;
	int offset[4][2] = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 } };
	for (int i = 0; i < 4; ++i)
	{
		if (p_GridInfo->inBounds(x + offset[i][0], y + offset[i][1]))
		{
			if (liquidCells.count((y + offset[i][1]) * p_GridInfo->xCells + x + offset[i][0]))
				liquid += 1;
			else
				air += 1;
		}
	}
}

void PressureSolve::addCoefficient(int curr_map_idx, int x, int y, std::unordered_map<int, int>& liquidCells)
{
	if (isLiquidCell(x, y, liquidCells))
	{
		int map_idx = liquidCells[y * p_GridInfo->xCells + x];
		a[curr_map_idx].push_back(map_idx);
		a[curr_map_idx].push_back(1.f);
	}
}

float PressureSolve::getLiquidCellPressure(int x, int y, std::unordered_map<int, int>& liquidCells)
{
	int idx = y * p_GridInfo->xCells + x;
	if (liquidCells.count(idx))
		return p[liquidCells[idx]];
	else
		return 0.f;
}

void PressureSolve::resetContainers(std::unordered_map<int, int>& liquidCells)
{
	d.clear();
	a.clear();
	p.clear();
	d.resize(liquidCells.size(), 0.0);
	a.resize(liquidCells.size());
	p.resize(liquidCells.size(), 0.0);
}

/*
* Note that density is on both LHS and RHS and plays 0 role in deriving the value of p, this makes sense since 
* in this Eulerian solver the fluid is 100% incompressible, hence density doesn't change
*/
void PressureSolve::setupLinearEquations(VelocityField* uField, std::unordered_map<int, int>& liquidCells, float t)
{
	for (auto cell : liquidCells)
	{
		int map_idx = cell.second;
		int x = cell.first % p_GridInfo->xCells;
		int y = cell.first / p_GridInfo->xCells;
		int solidNeighbors = 0, liquidNeighbors = 0, airNeighbors = 0;
		countSurroundingCellTypes(x, y, liquidCells, airNeighbors, liquidNeighbors);
		// find divergence for center of cell
		float div = uField->getDerivative(x, y, x + 1, y, 'x') + uField->getDerivative(x, y, x, y + 1, 'y');
		// store divergence to D
		d[map_idx] = (div * ((EULERIAN_DEN_WATER * pow(H, 2)) / t));
		// add coefficients for surrounding cells
		a[map_idx].clear();
		addCoefficient(map_idx, x - 1, y, liquidCells);
		addCoefficient(map_idx, x + 1, y, liquidCells);
		addCoefficient(map_idx, x, y - 1, liquidCells);
		addCoefficient(map_idx, x, y + 1, liquidCells);
		// store total non-solid neighbors for diagonals
		a[map_idx].push_back(map_idx);
		a[map_idx].push_back(-(liquidNeighbors + airNeighbors));
	}
}

void PressureSolve::solve(std::unordered_map<int, int>& liquidCells)
{
	CGSolver::init(liquidCells.size());
	CGSolver::solve(a, d, p);
}

void PressureSolve::integration(VelocityField* uField, std::unordered_map<int, int>& liquidCells, float t)
{
	// update vel (boundary vels don't update)
	for (int y = 0; y < p_GridInfo->yCells; ++y)
	{
		for (int x = 0; x < p_GridInfo->xCells; ++x)
		{
			// x
			if (x > 0)
			{
				// border liquid
				if (isLiquidCell(x, y, liquidCells) || isLiquidCell(x - 1, y, liquidCells))
				{
					float nextP = getLiquidCellPressure(x, y, liquidCells);
					float prevP = getLiquidCellPressure(x - 1, y, liquidCells);
					float xp = (nextP - prevP) / H;
					uField->addToCompByIdx(x, y, 'x', -((t / (EULERIAN_DEN_WATER * H)) * xp));
				}
			}
			// y
			if (y > 0)
			{
				// border liquid
				if (isLiquidCell(x, y, liquidCells) || isLiquidCell(x, y - 1, liquidCells))
				{
					float nextP = getLiquidCellPressure(x, y, liquidCells);
					float prevP = getLiquidCellPressure(x, y - 1, liquidCells);
					float yp = (nextP - prevP) / H;
					uField->addToCompByIdx(x, y, 'y', -((t / (EULERIAN_DEN_WATER * H)) * yp));
				}
			}
		}
	}
	uField->postUpdate();
}

void PressureSolve::update(VelocityField* uField, std::unordered_map<int, int>& liquidCells, float t)
{
	resetContainers(liquidCells);
	setupLinearEquations(uField, liquidCells, t);
	solve(liquidCells);
	integration(uField, liquidCells, t);
}
