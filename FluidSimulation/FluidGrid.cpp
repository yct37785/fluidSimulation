#include "FluidGrid.h"
// defines
const float G = -9.81f;	// gravity = -9.81m/s^2
const float DEN = 1000.f;	// water density = 1000 kh/m^3
const float Kcfl = 1.f;
const float X = 1.f;	// width/height of a grid cell

FluidGrid::FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	triangleMesh = MeshBuilder::CreateMesh("triangle");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	gridCells = new MACCell*[yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		gridCells[y] = new MACCell[xCellsCount];
	}

	maxU = glm::vec2(0.f, 0.f);
}

FluidGrid::~FluidGrid()
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		delete[] gridCells[y];
	}
	delete[] gridCells;
	delete gridMesh;
	delete triangleMesh;
}

glm::vec2 FluidGrid::getInterVelocity(float x, float y)
{
	// u: (conceptual) i - 1/2 = (indice) i
	glm::vec2 u = glm::vec2();
	int xMin = floor(x);
	int xMax = x + 1;
	int yMin = floor(y);
	int yMax = y + 1;
	// u
	return u;
}

float FluidGrid::getDeltaTime()
{
	float maxVelocityScalar = glm::length(maxU);
	float u_max = maxVelocityScalar + sqrt(abs(Kcfl * X * G));
	return Kcfl * X / u_max;
}

void FluidGrid::Update(float deltaTime)
{
	float timestep = getDeltaTime() * deltaTime * 60.f;	// mul with offset for fps
}


void FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			gridCells[y][x].Draw(mvMat, mvpHandle, triangleMesh, (float)x, (float)y);
		}
	}
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}