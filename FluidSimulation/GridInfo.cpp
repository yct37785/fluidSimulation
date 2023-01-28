#include "GridInfo.h"
std::unique_ptr<GridInfo> GridInfo::instance;

std::unique_ptr<GridInfo>& GridInfo::GetInstance()
{
	if (instance == nullptr)
	{
		instance = std::make_unique<GridInfo>();
	}
	return instance;
}

void GridInfo::setInfo(int xCells, int yCells)
{
	this->xCells = xCells;
	this->yCells = yCells;
}

bool GridInfo::inBounds(int x, int y)
{
	return (x >= 0 && x < xCells) && (y >= 0 && y < yCells);
}

bool GridInfo::inBoundsX(int x) { return (x >= 0 && x < xCells); }

bool GridInfo::inBoundsY(int y) { return (y >= 0 && y < yCells); }