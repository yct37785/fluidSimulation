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

void PressureSolve::update(VelocityField& uField, bool** liquidCells, float t)
{
	std::fill(d.begin(), d.end(), 0);
	for (int y = 0; y < yCellsCount - 0; ++y)
	{
		for (int x = 0; x < xCellsCount - 0; ++x)
		{
			float den = liquidCells[y][x] ? DEN_WATER : DEN_AIR;
			float scale = t / (den * H * H);
			int idx = y * xCellsCount + x;
			// find divergence d for p value of each cell
			// x axis
			float xdiv = 0.f;
			if (isValidCell(x + 1, y))
				xdiv = uField.getVelByIdx(x + 1, y).x;
			if (isValidCell(x - 0, y))
				xdiv -= uField.getVelByIdx(x - 0, y).x;
			xdiv /= H;
			// y axis
			float ydiv = 0.f;
			if (isValidCell(x, y + 1))
				ydiv = uField.getVelByIdx(x, y + 1).y;
			if (isValidCell(x, y - 0))
				ydiv -= uField.getVelByIdx(x, y - 0).y;
			ydiv /= H;
			 d[idx] = -(xdiv + ydiv);
			// form the coefficient matrix
			int totalNonSolidNeighbors = 0;
			a[idx].clear();
			// add neighboring cells coefficients
			if (addNeighborNonSolidCell(idx, x - 1, y, scale))
				totalNonSolidNeighbors++;
			if (addNeighborNonSolidCell(idx, x + 1, y, scale))
				totalNonSolidNeighbors++;
			if (addNeighborNonSolidCell(idx, x, y - 1, scale))
				totalNonSolidNeighbors++;
			if (addNeighborNonSolidCell(idx, x, y + 1, scale))
				totalNonSolidNeighbors++;
			a[idx].push_back(idx);
			a[idx].push_back(-totalNonSolidNeighbors);
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
	for (int y = 0; y < yCellsCount - 0; ++y)
	{
		for (int x = 0; x < xCellsCount - 0; ++x)
		{
			glm::vec2 vel = uField.getVelByIdx(x, y);
			// x comp
			float nextP = p[y * xCellsCount + x];
			float currP = 0.f;
			if (y * xCellsCount + (x - 1) >= 0)
				currP = p[y * xCellsCount + (x - 1)];
			float xDiff = (nextP - currP);
			// y comp
			nextP = p[y * xCellsCount + x];
			currP = 0.f;
			if ((y - 1) * xCellsCount + x >= 0)
				currP = p[(y - 1) * xCellsCount + x];
			float yDiff = (nextP - currP);
			// will cause particles to compress
			/*if (x == 0)
				xDiff = 0.f;
			else if (y == 0)
				yDiff = 0.f;
			else if (x == xCellsCount - 1)
				xDiff = 0.f;
			else if (y == yCellsCount - 1)
				yDiff = 0.f;*/
			// NEEDED, won't cause particles to compress
			if (x == 0)
				xDiff = max(0.f, xDiff);
			else if (y == 0)
				yDiff = max(0.f, yDiff);
			else if (x == xCellsCount - 1)
				xDiff = min(0.f, xDiff);
			else if (y == yCellsCount - 1)
				yDiff = min(0.f, yDiff);
			vel.x += xDiff;
			vel.y += yDiff;
			if (x == 0)
				vel.x = max(0.f, vel.x);
			else if (y == 0)
				vel.y = max(0.f, vel.y);
			else if (x == xCellsCount - 1)
				vel.x = min(0.f, vel.x);
			else if (y == yCellsCount - 1)
				vel.y = min(0.f, vel.y);
			uField.setVelByIdx(vel, x, y);
		}
	}
}
