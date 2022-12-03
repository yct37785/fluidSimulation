#include "FluidGrid.h"

FluidGrid::FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	triangleMesh = MeshBuilder::CreateMesh("triangle");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	uField = new VelocityField(xCellsCount, yCellsCount);
	uField->runUT();
}

FluidGrid::~FluidGrid()
{
	delete uField;
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
	glm::vec2 maxU = uField->getMaxU();
	float u_max = glm::length(maxU) + sqrt(abs(H * G));
	return Kcfl * H / u_max;
}

void FluidGrid::Update(float deltaTime)
{
	float t = getTimeStep() * deltaTime * 1.f;
	uField->advectSelf(t);
	/*glm::vec2 extForces(0.f, -0.981f);
	uField->applyExternalForces(extForces, t);*/
	// more stuff
	// update prev value with curr value
	uField->postUpdate();
}


void FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	uField->draw(mvMat, mvpHandle, triangleMesh);
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}