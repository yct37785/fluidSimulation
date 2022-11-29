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
	this->xBorderCount = xCellsCount + 1;
	this->yBorderCount = yCellsCount + 1;

	pField = new float*[yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		pField[y] = new float[xCellsCount];

		for (int x = 0; x < xCellsCount; ++x)
		{
			pField[y][x] = 1.f;
		}
	}

	uField = new float*[yBorderCount];
	vField = new float*[yBorderCount];
	for (int y = 0; y < yBorderCount; ++y)
	{
		uField[y] = new float[xBorderCount];
		vField[y] = new float[xBorderCount];

		for (int x = 0; x < xBorderCount; ++x)
		{
			uField[y][x] = x % 2 ? 1.f : -2.f;
			vField[y][x] = y % 2 ? 2.f : -1.f;
		}
	}

	maxU = glm::vec2(0.f, 0.f);
}

FluidGrid::~FluidGrid()
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		delete[] pField[y];
	}
	for (int y = 0; y < yBorderCount; ++y)
	{
		delete[] uField[y];
		delete[] vField[y];
	}
	delete[] pField;
	delete[] uField;
	delete[] vField;
	delete gridMesh;
	delete triangleMesh;
}

glm::vec2 FluidGrid::getInterVelocity(float x, float y)
{
	// u: (conceptual) i - 1/2 = (indice) i
	glm::vec2 u = glm::vec2();
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
	for (int y = 0; y < yBorderCount; ++y)
	{
		for (int x = 0; x < xBorderCount; ++x)
		{
			float u = uField[y][x];
			float v = vField[y][x];
			// u
			glm::mat4 mvpMat = mvMat *
				glm::translate(glm::mat4(1.f), glm::vec3((float)x, (float)y + 0.5f, 0.f)) *
				glm::rotate(glm::mat4(1.f), u >= 0.f ? 0.f : 3.14159f, glm::vec3(0.f, 0.f, 1.f)) *
				glm::scale(glm::mat4(1.f), glm::vec3(u, u, 1.f));
			glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
			glBindVertexArray(triangleMesh->getVAO());
			glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
			// v
			mvpMat = mvMat *
				glm::translate(glm::mat4(1.f), glm::vec3((float)x + 0.5f, (float)y, 0.f)) *
				glm::rotate(glm::mat4(1.f), v >= 0.f ? 1.5708f : -1.5708f, glm::vec3(0.f, 0.f, 1.f)) *
				glm::scale(glm::mat4(1.f), glm::vec3(v, v, 1.f));
			glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
			glBindVertexArray(triangleMesh->getVAO());
			glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
		}
	}
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}