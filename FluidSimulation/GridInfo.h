#pragma once
# include "Utility.h"

/*
* info of grid
*/
#define p_GridInfo GridInfo::GetInstance()
class GridInfo
{
public:
	int xCellsCount, yCellsCount;

	static std::unique_ptr<GridInfo>& GetInstance();
	void setInfo(int xCellsCount, int yCellsCount);

private:
	static std::unique_ptr<GridInfo> instance;
};