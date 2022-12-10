#include "PressureSolve.h"

PressureSolve::PressureSolve(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	d.resize((xCellsCount - 1) * (yCellsCount - 1));
	p.resize((xCellsCount - 1) * (yCellsCount - 1));
	a.resize((xCellsCount - 1) * (yCellsCount - 1));
}

PressureSolve::~PressureSolve()
{
}

bool PressureSolve::isValidCell(int x, int y)
{
	return x >= 0 && x < xCellsCount - 1 && y >= 0 && y < yCellsCount - 1;
}

bool PressureSolve::addNeighborNonSolidCell(int idx, int x, int y, float v)
{
	if (isValidCell(x, y))
	{
		a[idx].push_back(y * (xCellsCount - 1) + x);
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
	// ignore top-most row for x or right-most column for y
	bool isEnd = comp == 0 ? y2 == yCellsCount - 1 : x2 == xCellsCount - 1;
	float v2 = 0.f, v1 = 0.f;
	if (isValidCell(x2, y2) && !isEnd)
		v2 = uField.getVelByIdx(x2, y2)[comp];
	if (isValidCell(x1, y1))
		v1 = uField.getVelByIdx(x1, y1)[comp];
	return (v2 - v1) / H;
}

void PressureSolve::update(VelocityField& uField, bool** liquidCells, float t)
{
	std::fill(d.begin(), d.end(), 0);
	for (int y = 0; y < yCellsCount - 1; ++y)
	{
		for (int x = 0; x < xCellsCount - 1; ++x)
		{
			// some values
			float den = liquidCells[y][x] ? DEN_WATER : DEN_AIR;
			float Patm = 1.f;
			int idx = y * (xCellsCount - 1) + x;
			int solidNeighbors = 0, liquidNeighbors = 0, airNeighbors = 0;
			countSurroundingCellTypes(x, y, liquidCells, airNeighbors, liquidNeighbors);
			// find divergence for center of cell
			float div = getDerivative(uField, 0, x + 1, y, x, y) + getDerivative(uField, 1, x, y + 1, x, y);
			//cout << "idx: " << idx << ", a: " << a.size() << endl;
			d[idx] = (div * ((den * H) / t)) - (float)airNeighbors * Patm;
			// form the coefficient matrix
			a[idx].clear();
			// add coefficients
			// why when using 1.f for coefficient Jacobi Method won't converge
			// https://stackoverflow.com/questions/24730993/jacobi-iteration-doesnt-end
			addNeighborNonSolidCell(idx, x - 1, y, 1.f);
			addNeighborNonSolidCell(idx, x + 1, y, 1.f);
			addNeighborNonSolidCell(idx, x, y - 1, 1.f);
			addNeighborNonSolidCell(idx, x, y + 1, 1.f);
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
	for (int y = 0; y < yCellsCount - 1; ++y)
	{
		for (int x = 0; x < xCellsCount - 1; ++x)
		{
			float den = liquidCells[y][x] ? DEN_WATER : DEN_AIR;
			float xp = 0.f, yp = 0.f;
			// x (also ignore top-most row for x)
			if (x > 0 && x < xCellsCount - 1 && y < yCellsCount - 1)
			{
				float nextP = p[y * (xCellsCount - 1) + x], prevP = 0.f;
				if (y * xCellsCount + x - 1 > 0)
					prevP = p[y * (xCellsCount - 1) + x - 1];
				xp = (nextP - prevP) / H;
			}
			// y (also ignore right-most column for y)
			if (y > 0 && y < yCellsCount - 1 && x < xCellsCount - 1)
			{
				float nextP = p[y * (xCellsCount - 1) + x], prevP = 0.f;
				if ((y - 1) * xCellsCount + x > 0)
					prevP = p[(y - 1) * (xCellsCount - 1) + x];
				yp = (nextP - prevP) / H;
			}
			// update vel
			glm::vec2 offset = (t / (den * H)) * glm::vec2(xp, yp);
			uField.setVelByIdx(uField.getVelByIdx(x, y) - offset, x, y);
		}
	}
	//// clamp vel comps facing solid/boundary
	//for (int y = 0; y < yCellsCount; ++y)
	//{
	//	for (int x = 0; x < xCellsCount; ++x)
	//	{
	//		glm::vec2 vel = uField.getVelByIdx(x, y);
	//		// clamp vels that point into solids/boundaries
	//		if (x == 0)
	//			vel.x = max(vel.x, 0.f);
	//		if (y == 0) {
	//			vel.y = max(vel.y, 0.f);
	//		}
	//		if (x == xCellsCount - 1)
	//			vel.x = min(vel.x, 0.f);
	//		if (y == yCellsCount - 1) {
	//			vel.y = min(vel.y, 0.f);
	//		}
	//		// set
	//		uField.setVelByIdx(vel, x, y);
	//	}
	//}
	/*uField.setVelByIdx(glm::vec2(0, 0), 0, 1);
	uField.setVelByIdx(glm::vec2(0, 0), 1, 0);*/
}
