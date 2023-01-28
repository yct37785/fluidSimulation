#pragma once
# include "Utility.h"

/*
* info of grid
*/
#define p_GridInfo GridInfo::GetInstance()
class GridInfo
{
public:
	int xCells, yCells;

	static std::unique_ptr<GridInfo>& GetInstance();
	void setInfo(int xCellsCount, int yCellsCount);

private:
	static std::unique_ptr<GridInfo> instance;
};