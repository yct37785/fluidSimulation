#include "PressureSolve.h"

PressureSolve::PressureSolve(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	d.resize(xCellsCount * yCellsCount);
	p.resize(xCellsCount * yCellsCount);
	a.resize(xCellsCount * yCellsCount);
}

PressureSolve::~PressureSolve()
{
}

bool PressureSolve::isValidCell(int x, int y)
{
	return x >= 0 && x < xCellsCount && y >= 0 && y < yCellsCount;
}

bool PressureSolve::addNeighborNonSolidCell(int idx, int x, int y, float v)
{
	if (isValidCell(x, y))
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

float PressureSolve::getDerivative(VelocityField& uField, int comp, int x2, int y2, int x1, int y1)
{
	float v2 = 0.f, v1 = 0.f;
	if (isValidCell(x2, y2))
		v2 = uField.getVelByIdx(x2, y2)[comp];
	if (isValidCell(x1, y1))
		v1 = uField.getVelByIdx(x1, y1)[comp];
	return (v2 - v1) / H;
}

void PressureSolve::update(VelocityField& uField, bool** liquidCells, float t)
{
	std::fill(d.begin(), d.end(), 0);
	for (int y = 0; y < yCellsCount - 0; ++y)
	{
		for (int x = 0; x < xCellsCount - 0; ++x)
		{
			// some values
			float den = liquidCells[y][x] ? DEN_WATER : DEN_AIR;
			float Patm = 1.f;
			int idx = y * xCellsCount + x;
			int solidNeighbors = 0, liquidNeighbors = 0, airNeighbors = 0;
			countSurroundingCellTypes(x, y, liquidCells, airNeighbors, liquidNeighbors);
			// find divergence for center of cell
			float div = getDerivative(uField, 0, x + 1, y, x, y) + getDerivative(uField, 1, x, y + 1, x, y);
			d[idx] = (div * ((den * H) / t)) - (float)airNeighbors * Patm;
			// form the coefficient matrix
			a[idx].clear();
			// add coefficients
			// why when using 1.f for coefficient Jacobi Method won't converge
			// https://stackoverflow.com/questions/24730993/jacobi-iteration-doesnt-end
			addNeighborNonSolidCell(idx, x - 1, y, 0.99f);
			addNeighborNonSolidCell(idx, x + 1, y, 0.99f);
			addNeighborNonSolidCell(idx, x, y - 1, 0.99f);
			addNeighborNonSolidCell(idx, x, y + 1, 0.99f);
			a[idx].push_back(idx);
			// cout << (liquidNeighbors + airNeighbors) << endl;
			a[idx].push_back(-(liquidNeighbors + airNeighbors));
		}
	}
	// CGSolver::print("A", a);
	// solve P for AP = D
	std::fill(p.begin(), p.end(), 0);
	JacobiMethod::solve(a, d, p);
	// cout << p[50] << endl;
	//cout << a.size() << " " << d.size() << " " << p.size() << endl;
	
	// update velocity
	// pressure is only applied to vel components that border fluid cells
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			float den = liquidCells[y][x] ? DEN_WATER : DEN_AIR;
			float xp = 0.f, yp = 0.f;
			// x
			if (x > 0)
			{
				float nextP = p[y * xCellsCount + x], prevP = 0.f;
				if (y * xCellsCount + x - 1 > 0)
					prevP = p[y * xCellsCount + x - 1];
				xp = (nextP - prevP) / 2.f;
			}
			// y
			if (y > 0)
			{
				float nextP = p[y * xCellsCount + x], prevP = 0.f;
				if ((y - 1) * xCellsCount + x > 0)
					prevP = p[(y - 1) * xCellsCount + x];
				yp = (nextP - prevP) / 2.f;
			}
			// update vel
			glm::vec2 vel = uField.getVelByIdx(x, y);
			vel -= (t / (den * H)) * glm::vec2(xp, yp);
			uField.setVelByIdx(vel, x, y);
		}
	}
}
