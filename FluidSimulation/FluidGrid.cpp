#include "FluidGrid.h"
// defines
const float G = -9.81f;	// gravity = -9.81m/s^2
const float DEN = 1000.f;	// water density = 1000 kh/m^3
const float Kcfl = 1.f;
const float X = 1.f;	// width/height of a grid cell

FluidGrid::FluidGrid(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	this->xBorderCount = xCellsCount + 1;
	this->yBorderCount = yCellsCount + 1;

	pressure = new glm::vec2*[yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		pressure[y] = new glm::vec2[xCellsCount];

		for (int x = 0; x < xCellsCount; ++x)
		{
			pressure[y][x] = glm::vec2(0.f, 0.f);
		}
	}

	velocity = new glm::vec2*[yBorderCount];
	for (int y = 0; y < yBorderCount; ++y)
	{
		velocity[y] = new glm::vec2[xBorderCount];

		for (int x = 0; x < xBorderCount; ++x)
		{
			velocity[y][x] = glm::vec2(0.f, 0.f);
		}
	}

	maxVelocity = glm::vec2(0.f, 0.f);
}

FluidGrid::~FluidGrid()
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		delete[] pressure[y];
	}
	for (int y = 0; y < yBorderCount; ++y)
	{
		delete[] velocity[y];
	}
	delete[] pressure;
	delete[] velocity;
}

float FluidGrid::getDeltaTime()
{
	return 0.f;
}

void FluidGrid::Update()
{
	float deltaT = getDeltaTime();
}