#include "Hybrid_FluidGrid.h"

Hybrid_FluidGrid::Hybrid_FluidGrid(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("blue_marker");
	uField = new VelocityField(xCellsCount, yCellsCount);
	ps = new PressureSolve(xCellsCount, yCellsCount);
}

Hybrid_FluidGrid::~Hybrid_FluidGrid()
{
	for (int i = 0; i < particles.size(); ++i)
		delete particles[i];
	delete gridMesh;
	delete particleMesh;
	delete uField;
	delete ps;
}

void Hybrid_FluidGrid::spawnParticles()
{
	// [Bridson 2007] 2 x 2 particles per grid cell
	float xmin = 0.3f;
	float xmax = 0.6f;
	float ymin = 0.6f;
	float ymax = 0.8f;
	float space = 0.4f;
	for (float y = 0.f + (float)yCellsCount * H * ymin; y < (float)yCellsCount * H * ymax; y += space * H)
	{
		for (float x = 0.f + (float)xCellsCount * H * xmin; x < (float)xCellsCount * H * xmax; x += space * H)
		{
			float jitterX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * H;
			float jitterY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * H;
			particles.push_back(new Particle(glm::vec2(x + jitterX, y + jitterY), glm::vec2(0.f)));
		}
	}
	cout << "Total particles: " << particles.size() << endl;
}

float Hybrid_FluidGrid::getTimestep(float deltaTime)
{
	return deltaTime * 4.f;
}

void Hybrid_FluidGrid::updateLiquidCells()
{
	liquidCells.clear();
	int count = 0;
	for (int i = 0; i < particles.size(); ++i)
	{
		glm::vec2 pos = particles[i]->get_pos();
		int xpos = (int)floor(pos.x / H);
		int ypos = (int)floor(pos.y / H);
		int idx = ypos * xCellsCount + xpos;
		if (xpos >= 0 && xpos < xCellsCount && ypos >= 0 && ypos < yCellsCount && !liquidCells.count(idx))
		{
			liquidCells[idx] = count;
			count++;
			gridMesh->colorCell(xpos, ypos, 255.f, 255.f, 230.f);
		}
	}
}

void Hybrid_FluidGrid::advectParticles_Eulerian(float t)
{
	for (int i = 0; i < particles.size(); ++i)
	{
		glm::vec2 vel = uField->getVelAtPos(particles[i]->get_pos());
		particles[i]->forwardEuler(vel, t);
	}
}

void Hybrid_FluidGrid::Update(float deltaTime)
{
	float t = getTimestep(deltaTime);
	updateLiquidCells();
	uField->advectSelf_semiLagrangian(t, liquidCells);
	uField->applyExternalForces(t, liquidCells);
	ps->update(uField, liquidCells, t);
	uField->extrapolate(liquidCells);
	advectParticles_Eulerian(t);
}

void Hybrid_FluidGrid::Draw(glm::mat4& mvMat, int mvpHandle)
{
	uField->draw(mvMat, mvpHandle);

	glm::mat4 mvpMat;
	for (int i = 0; i < particles.size(); ++i)
	{
		glm::vec2 pos = particles[i]->get_pos();
		mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(pos.x, pos.y, 0.f));
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
		glBindVertexArray(particleMesh->getVAO());
		glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
	}

	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
