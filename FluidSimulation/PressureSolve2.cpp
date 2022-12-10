#include "PressureSolve2.h"

PressureSolve2::PressureSolve2(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	d.resize(xCellsCount * yCellsCount);
	p.resize(xCellsCount * yCellsCount);
	a.resize(xCellsCount * yCellsCount);
}

PressureSolve2::~PressureSolve2()
{
}

bool PressureSolve2::isValidCell(int x, int y)
{
	return x >= 0 && x < xCellsCount && y >= 0 && y < yCellsCount;
}

bool PressureSolve2::addNeighborNonSolidCell(int idx, int x, int y, float v)
{
	if (isValidCell(x, y))
	{
		a[idx].push_back(y * xCellsCount + x);
		a[idx].push_back(v);
		return true;
	}
	return false;
}

void PressureSolve2::countSurroundingCellTypes(int x, int y, bool** liquidCells, int& air, int& liquid)
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

float PressureSolve2::getDerivative(VelocityField2& uField, char comp, int x2, int y2, int x1, int y1)
{
	float v2 = 0.f, v1 = 0.f;
	v2 = uField.getCompByIdx(x2, y2, comp);
	v1 = uField.getCompByIdx(x1, y1, comp);
	return (v2 - v1) / H;
}

void PressureSolve2::update(VelocityField2& uField, bool** liquidCells, float t)
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
			d[idx] = (div * ((den * H) / t)) - (float)airNeighbors * Patm;
			// add coefficients
			// why when using 1.f for coefficient Jacobi Method won't converge
			// https://stackoverflow.com/questions/24730993/jacobi-iteration-doesnt-end
			addNeighborNonSolidCell(idx, x - 1, y, 1.f);
			addNeighborNonSolidCell(idx, x + 1, y, 1.f);
			addNeighborNonSolidCell(idx, x, y - 1, 1.f);
			addNeighborNonSolidCell(idx, x, y + 1, 1.f);
			a[idx].push_back(idx);
			a[idx].push_back(-(liquidNeighbors + airNeighbors));
		}
	}
	// solve P for AP = D
	JacobiMethod::solve(a, d, p);
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
