#include "FluidGrid.h"
// defines
const float G = -9.81f;	// gravity = -9.81m/s^2, adjustable depending on visual
const float DEN = 1.f;	// water density = 1000 kh/m^3, but here we set it to 1
const float Kcfl = 1.f;	// timestep scale, 1 - 5
const float H = 1.f;	// width/height of a grid cell

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
		for (int x = 0; x < xCellsCount; ++x)
		{
			gridCells[y][x].setPos(x, y);
		}
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

glm::vec2 FluidGrid::getVelocityBilinear(float x, float y)
{
	return glm::vec2();
}

float FluidGrid::getTimeStep()
{
	// Bridson 2007, a more robust timestep calculation where no divide by zero errors will occur
	float u_max = glm::length(maxU) + sqrt(abs(H * G));
	return Kcfl * H / u_max;
}

void FluidGrid::Update(float deltaTime)
{
	// time step
	float timestep = getTimeStep() * deltaTime * 20.f;
	// update cell states with marker particles

	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// advect velocity field through itself (velocity at boundaries will be 0)
			gridCells[y][x].Advect(gridCells, xCellsCount, yCellsCount, timestep);
			// apply external forces
		}
	}
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// pressure update

			// post update
			gridCells[y][x].postUpdate();
		}
	}
}


void FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			gridCells[y][x].Draw(mvMat, mvpHandle, triangleMesh);
		}
	}
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}