#pragma once
# include "Mesh.h"

class GridMesh : public Mesh
{
	int xCellsCount, yCellsCount;
	int xOffset, yOffset;

	// utils
	int buildVertex(std::vector<float>& vertices, int coordsCounter, int x, int y, int cellCounter);

public:
	GridMesh(int xCellsCount, int yCellsCount, int xOffset, int yOffset);
	~GridMesh();

	void ResetCellsColor();
	void colorCell(int x, int y, float r, float g, float b);
	void updateMesh();
};