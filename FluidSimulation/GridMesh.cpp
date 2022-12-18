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
	vertices[coordsCounter + 0] = (float)x * H;
	vertices[coordsCounter + 1] = (float)y * H;
	/*if (cellCounter % 2)
	{
		vertices[coordsCounter + 2] = 51.f / 255.f;
		vertices[coordsCounter + 3] = 153.f / 255.f;
		vertices[coordsCounter + 4] = 51.f / 255.f;
	}
	else*/
	{
		vertices[coordsCounter + 2] = 0.f;
		vertices[coordsCounter + 3] = 0.f;
		vertices[coordsCounter + 4] = 0.f;
	}
	return coordsCounter + Mesh::COORDS_PER_VERTEX;
}

void GridMesh::ResetCellsColor()
{
	for (int i = 0; i < vertices.size(); i += Mesh::COORDS_PER_VERTEX)
	{
		vertices[i + 2] = 0.f;
		vertices[i + 3] = 0.f;
		vertices[i + 4] = 0.f;
	}
}

void GridMesh::colorCell(int x, int y, float r, float g, float b)
{
	int i = ((y - yOffset) * xCellsCount + (x - xOffset)) * 4 * Mesh::COORDS_PER_VERTEX;
	for (int j = i; j < i + 4 * Mesh::COORDS_PER_VERTEX; j += Mesh::COORDS_PER_VERTEX)
	{
		vertices[j + 2] = r / 255.f;
		vertices[j + 3] = g / 255.f;
		vertices[j + 4] = b / 255.f;
	}
}

void GridMesh::updateMesh()
{
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, vertices.size() * sizeof(float), vertices.data());
}