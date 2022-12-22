#include "SPH_FluidGrid.h"

SPH_FluidGrid::SPH_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("sph_blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	float mass = 10.f;
	float density = 30.f;
	float xmin = 0.3f;
	float xmax = 0.7f;
	float ymin = 0.3f;
	float ymax = 0.7f;
	float space = 0.4f;
	for (float y = 0.f + (float)yCellsCount * Hrad * ymin; y < (float)yCellsCount * Hrad * ymax; y += space * Hrad)
	{
		for (float x = 0.f + (float)xCellsCount * Hrad * xmin; x < (float)xCellsCount * Hrad * xmax; x += space * Hrad)
		{
			particles.push_back(SPH_Particle());
			particles.back().init(mass, density, glm::vec2(x, y));
		}
	}
	cout << "Total particles: " << particles.size() << endl;
}

SPH_FluidGrid::~SPH_FluidGrid()
{
}

void SPH_FluidGrid::spawnFluid()
{
}

void SPH_FluidGrid::Update(float deltaTime)
{
}

void SPH_FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	glm::mat4 mvpMat;
	for (int i = 0; i < particles.size(); ++i)
	{
		mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(particles[i].getPos(), 0.f));
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
		glBindVertexArray(particleMesh->getVAO());
		glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
	}

	mvpMat = mvMat * glm::scale(glm::mat4(1.f), glm::vec3(Hrad, Hrad, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
