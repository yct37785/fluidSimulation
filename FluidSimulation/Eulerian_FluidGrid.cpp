#include "Eulerian_FluidGrid.h"

Eulerian_FluidGrid::Eulerian_FluidGrid()
{
	gridMesh = new GridMesh(p_GridInfo->xCells, p_GridInfo->yCells, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("blue_marker");
	uField = new VelocityField();
	ps = new PressureSolve();
	cellType = new CELL_TYPES * [p_GridInfo->yCells];
	for (int i = 0; i < p_GridInfo->yCells; ++i)
		cellType[i] = new CELL_TYPES[p_GridInfo->xCells];
}

Eulerian_FluidGrid::~Eulerian_FluidGrid()
{
	for (int i = 0; i < particles.size(); ++i)
		delete particles[i];
	delete gridMesh;
	delete particleMesh;
	delete uField;
	delete ps;
	for (int i = 0; i < p_GridInfo->yCells; ++i)
		delete[] cellType[i];
	delete[] cellType;
}

void Eulerian_FluidGrid::spawnParticles(float xmin, float xmax, float ymin, float ymax, float space)
{
	// [Bridson 2007] 2 x 2 particles per grid cell
	for (float y = 0.f + (float)p_GridInfo->yCells * H * ymin; y < (float)p_GridInfo->yCells * H * ymax; y += space * H)
	{
		for (float x = 0.f + (float)p_GridInfo->xCells * H * xmin; x < (float)p_GridInfo->xCells * H * xmax; x += space * H)
		{
			float jitterX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * H;
			float jitterY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * H;
			particles.push_back(new Particle(glm::vec2(x + jitterX, y + jitterY), glm::vec2(0.f)));
		}
	}
	std::cout << "Total particles: " << particles.size() << std::endl;
}

float Eulerian_FluidGrid::getTimestep(float deltaTime)
{
	return deltaTime * 4.f;
}

void Eulerian_FluidGrid::updateLiquidCells()
{
	// reset all to air
	/*for (int y = 0; y < yCellsCount; ++y)
		for (int x = 0; x < xCellsCount; ++x)
			cellType[y][x] = CELL_AIR;*/
	liquidCells.clear();
	int count = 0;
	for (int i = 0; i < particles.size(); ++i)
	{
		glm::vec2 pos = particles[i]->get_pos();
		int xpos = (int)floor(pos.x / H);
		int ypos = (int)floor(pos.y / H);
		int idx = ypos * p_GridInfo->xCells + xpos;
		if (p_GridInfo->inBounds(xpos, ypos) && !liquidCells.count(idx))
		{
			liquidCells[idx] = count;
			count++;
			gridMesh->colorCell(xpos, ypos, 255.f, 255.f, 230.f);
		}
	}
}

void Eulerian_FluidGrid::advectParticles_Eulerian(float t)
{
	for (int i = 0; i < particles.size(); ++i)
	{
		glm::vec2 vel = uField->getVelAtPos(particles[i]->get_pos());
		particles[i]->forwardEuler(vel, t);
	}
}

void Eulerian_FluidGrid::Update(float deltaTime)
{
	float t = getTimestep(deltaTime);
	updateLiquidCells();
	uField->advectSelf_semiLagrangian(t, liquidCells);
	uField->applyExternalForces(t, liquidCells);
	ps->update(uField, liquidCells, t);
	uField->extrapolate(liquidCells);
	advectParticles_Eulerian(t);
}

void Eulerian_FluidGrid::Draw(glm::mat4& mvMat, int mvpHandle)
{
	uField->draw(mvMat, mvpHandle);

	glm::mat4 mvpMat;
	for (int i = 0; i < particles.size(); i += 1)
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
