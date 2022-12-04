#include "PressureSolve.h"

PressureSolve::PressureSolve(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	p = new float*[yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		p[y] = new float[xCellsCount];
		for (int x = 0; x < xCellsCount; ++x)
		{
			p[y][x] = 0.f;
		}
	}
}

PressureSolve::~PressureSolve()
{
}

void PressureSolve::update(VelocityField& uField, map<int, int>& liquidCells)
{
}
