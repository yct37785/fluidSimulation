#include "PressureSolve.h"

PressureSolve::PressureSolve(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	CGSolver::UT_cgsolver();
	d.resize(xCellsCount * yCellsCount);
	a.resize(xCellsCount * yCellsCount);
	for (int i = 0; i < xCellsCount * yCellsCount; ++i)
	{
		a[i].resize(xCellsCount * yCellsCount);
	}
}

PressureSolve::~PressureSolve()
{
}

bool PressureSolve::isValidCell(int x, int y)
{
	return x < 0 || x >= xCellsCount || y < 0 || y > yCellsCount;
}

void PressureSolve::update(VelocityField& uField, map<int, int>& liquidCells, float t)
{
	const int c = t / DEN_WATER * H * H;
	// find divergence d for p value of each cell
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// x axis
			float xDiv = 0.f;
			if (isValidCell(x + 1, y))
				xDiv += uField.getVelByIdx(x + 1, y).x;
			else
				xDiv += 0.f;
			xDiv -= uField.getVelByIdx(x, y).x;
			xDiv /= H;
			// y axis
			float yDiv = 0.f;
			if (isValidCell(x, y + 1))
				yDiv += uField.getVelByIdx(x + 1, y).y;
			else
				yDiv += 0.f;
			yDiv -= uField.getVelByIdx(x, y).y;
			yDiv /= H;
			d[y * xCellsCount + x] = -(xDiv + yDiv) / c;
		}
	}
}
