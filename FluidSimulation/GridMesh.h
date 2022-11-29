#pragma once
# include "Mesh.h"

class GridMesh : public Mesh
{
	int xCellsCount, yCellsCount;
	int xOffset, yOffset;

	// utils
	int buildVertex(vector<float>& vertices, int coordsCounter, int x, int y, int cellCounter);

public:
	GridMesh(int xCellsCount, int yCellsCount, int xOffset, int yOffset);
	~GridMesh();
};