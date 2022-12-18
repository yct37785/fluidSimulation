#include "PressureSolve.h"
const float DEN_WATER = 1.f;	// water density = 1000 kg/m^3, but in simulation always set to 1
const float DEN_AIR = 1.f;	// air density = 1 kg/m^3

PressureSolve::PressureSolve(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	d.resize(xCellsCount * yCellsCount);
	p.resize(xCellsCount * yCellsCount);
	a.resize(xCellsCount * yCellsCount);
	CGSolver::init(xCellsCount * yCellsCount);
}

PressureSolve::~PressureSolve()
{
}

bool PressureSolve::isValidCell(int x, int y)
{
	return x >= 0 && x < xCellsCount && y >= 0 && y < yCellsCount;
}

bool PressureSolve::addNeighborLiquidCell(int idx, int x, int y, float v, bool** liquidCells)
{
	if (isValidCell(x, y) && liquidCells[y][x])
	{
		a[idx].push_back(y * xCellsCount + x);
		a[idx].push_back(v);
		return true;
	}
	return false;
}

void PressureSolve::countSurroundingCellTypes(int x, int y, bool** liquidCells, int& air, int& liquid)
{
	air = liquid = 0;
	// top
	if (isValidCell(x, y + 1))
	{
		if (liquidCells[x][y + 1])
			liquid += 1;
		else
			air += 1;
	}
	// bottom
	if (isValidCell(x, y - 1))
	{
		if (liquidCells[x][y - 1])
			liquid += 1;
		else
			air += 1;
	}
	// left
	if (isValidCell(x - 1, y))
	{
		if (liquidCells[x - 1][y])
			liquid += 1;
		else
			air += 1;
	}
	// right
	if (isValidCell(x + 1, y))
	{
		if (liquidCells[x + 1][y])
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

void PressureSolve::update(VelocityField& uField, bool** liquidCells, float t)
{
	std::fill(d.begin(), d.end(), 0);
	std::fill(p.begin(), p.end(), 0);
	// for each grid cell, pressure term P is situated in the center
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// some values
			float den = liquidCells[y][x] ? DEN_WATER : DEN_AIR;
			float Patm = 1.f;
			int idx = y * xCellsCount + x;
			int solidNeighbors = 0, liquidNeighbors = 0, airNeighbors = 0;
			countSurroundingCellTypes(x, y, liquidCells, airNeighbors, liquidNeighbors);
			// find divergence for center of cell
			float div = getDerivative(uField, 'x', x + 1, y, x, y) + getDerivative(uField, 'y', x, y + 1, x, y);
			// store divergence to D
			a[idx].clear();
			// DO NOT ADD ATMOSPHERIC PRESSURE
			d[idx] = (div * ((den * H) / t));
			// add coefficients
			// why when using 1.f for coefficient Jacobi Method won't converge
			// https://stackoverflow.com/questions/24730993/jacobi-iteration-doesnt-end
			addNeighborLiquidCell(idx, x - 1, y, 1.f, liquidCells);
			addNeighborLiquidCell(idx, x + 1, y, 1.f, liquidCells);
			addNeighborLiquidCell(idx, x, y - 1, 1.f, liquidCells);
			addNeighborLiquidCell(idx, x, y + 1, 1.f, liquidCells);
			a[idx].push_back(idx);
			a[idx].push_back(-(liquidNeighbors + airNeighbors));
		}
	}
	// solve P for AP = D
	JacobiMethod::solve(a, d, p);
	// CGSolver::solve(a, d, p);
	// update vel (boundary vels don't update)
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			float den = liquidCells[y][x] ? DEN_WATER : DEN_AIR;
			// x
			if (x > 0)
			{
				float nextP = p[y * xCellsCount + x];
				float prevP = p[y * xCellsCount + x - 1];
				float xp = (nextP - prevP) / H;
				uField.addToCompByIdx(x, y, 'x', -((t / (den * H)) * xp));
			}
			// y
			if (y > 0)
			{
				float nextP = p[y * xCellsCount + x];
				float prevP = p[(y - 1) * xCellsCount + x];
				float yp = (nextP - prevP) / H;
				uField.addToCompByIdx(x, y, 'y', -((t / (den * H)) * yp));
			}
		}
	}
}
