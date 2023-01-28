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

void GridInfo::setInfo(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
}