#include "SPH_FluidGrid.h"

SPH_FluidGrid::SPH_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("sph_blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	float mass = 10.f;
	float xmin = 0.7f;
	float xmax = 0.9f;
	float ymin = 0.4f;
	float ymax = 0.6f;
	float space = 0.4f;
	for (float y = 0.f + (float)yCellsCount * Hrad * ymin; y < (float)yCellsCount * Hrad * ymax; y += space * Hrad)
	{
		for (float x = 0.f + (float)xCellsCount * Hrad * xmin; x < (float)xCellsCount * Hrad * xmax; x += space * Hrad)
		{
			particles.push_back(SPH_Particle());
			particles.back().init(mass, glm::vec2(x, y), glm::vec2(0.f, 0.f));
		}
	}
	cout << "Total particles: " << particles.size() << endl;

	wMultiplier = 315.f / (64.f * PI * pow(Hrad, 9));
}

SPH_FluidGrid::~SPH_FluidGrid()
{
}

// smoothing kernel, returns 0 -> 1
// param r: dist btw particle i and j
float SPH_FluidGrid::calW(float r)
{
	float distVar = Hrad * Hrad - r * r;
	distVar = pow(distVar, 3);
	return wMultiplier * distVar;
}

void SPH_FluidGrid::Update(float deltaTime)
{
	float t = 0.f;
	// find neighbors
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i].neighbors.clear();
		particles[i].neighborDist.clear();
		for (int j = 0; j < particles.size(); ++j)
		{
			float r = glm::length(particles[i].getPos() - particles[j].getPos());
			// only consider neighbors within < 2 * Hrad dist
			if (j != i && r < Hrad * 2.f)
			{
				particles[i].neighbors.push_back(j);
				particles[i].neighborDist.push_back(r);
			}
		}
	}
	// find density
	for (int i = 0; i < particles.size(); ++i)
	{
		// find density
		// pi = sum(mj * Wij)
		float density = 0.f;
		for (int j = 0; j < particles[i].neighbors.size(); ++j)
		{
			int nIdx = particles[i].neighbors[j];
			density += particles[nIdx].getMass() * calW(particles[i].neighborDist[j]);
		}
		particles[i].setDensity(density);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i].postUpdate();
	// find pressure
	for (int i = 0; i < particles.size(); ++i)
		float pressure = K * (particles[i].getDensity() - P0);
	for (int i = 0; i < particles.size(); ++i)
		particles[i].postUpdate();
	// find acceleration

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
