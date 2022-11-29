#include "GridMesh.h"

GridMesh::GridMesh(int xCellsCount, int yCellsCount, int xOffset, int yOffset)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	this->xOffset = xOffset;
	this->yOffset = yOffset;

	vector<float> vertices(xCellsCount * yCellsCount * 4 * Mesh::COORDS_PER_VERTEX);
	vector<int> indices(xCellsCount * yCellsCount * 6);
	// start generating grid cells
	int indicesCounter = 0;
	int coordsCounter = 0;
	int verticesCounter = 0;
	for (int y = yOffset; y < yCellsCount + yOffset; ++y)
	{
		int cellCounter = y % 2 ? 0 : 1;
		for (int x = xOffset; x < xCellsCount + xOffset; ++x)
		{
			coordsCounter = buildVertex(vertices, coordsCounter, x, y, cellCounter);
			coordsCounter = buildVertex(vertices, coordsCounter, x, y + 1, cellCounter);
			coordsCounter = buildVertex(vertices, coordsCounter, x + 1, y + 1, cellCounter);
			coordsCounter = buildVertex(vertices, coordsCounter, x + 1, y, cellCounter);
			indices[indicesCounter + 0] = verticesCounter + 0;
			indices[indicesCounter + 1] = verticesCounter + 1;
			indices[indicesCounter + 2] = verticesCounter + 2;
			indices[indicesCounter + 3] = verticesCounter + 0;
			indices[indicesCounter + 4] = verticesCounter + 2;
			indices[indicesCounter + 5] = verticesCounter + 3;
			indicesCounter += 6;
			verticesCounter += 4;
			cellCounter += 1;
		}
	}
	initMesh(vertices, indices, GL_DYNAMIC_DRAW);
}

GridMesh::~GridMesh()
{
}

int GridMesh::buildVertex(vector<float>& vertices, int coordsCounter, int x, int y, int cellCounter)
{
	// x, y, r, g, b
	vertices[coordsCounter + 0] = (float)x;
	vertices[coordsCounter + 1] = (float)y;
	vertices[coordsCounter + 2] = 0.f;
	vertices[coordsCounter + 3] = cellCounter % 2 ? 1.f : 0.f;
	vertices[coordsCounter + 4] = 0.f;
	return coordsCounter + Mesh::COORDS_PER_VERTEX;
}