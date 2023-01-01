#include "Hybrid_FluidGrid.h"

Hybrid_FluidGrid::Hybrid_FluidGrid(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("blue_marker");
	uField = new VelocityField(xCellsCount, yCellsCount);
}

Hybrid_FluidGrid::~Hybrid_FluidGrid()
{
}

void Hybrid_FluidGrid::spawnParticles()
{
}

void Hybrid_FluidGrid::Update(float deltaTime)
{
}

void Hybrid_FluidGrid::Draw(glm::mat4& mvMat, int mvpHandle)
{
	uField->draw(mvMat, mvpHandle);

	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
