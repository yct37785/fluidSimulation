#include "FluidGrid.h"

FluidGrid::FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	triangleMesh = MeshBuilder::CreateMesh("triangle");
	markerMesh = MeshBuilder::CreateMesh("blue_marker");
	fluidSourceMesh = MeshBuilder::CreateMesh("orange_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	uField = new VelocityField(xCellsCount, yCellsCount);
	// uField->runUT();
	ps = new PressureSolve(xCellsCount, yCellsCount);

	liquidCells = new bool* [yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		liquidCells[y] = new bool[xCellsCount];
		for (int x = 0; x < xCellsCount; ++x)
			liquidCells[y][x] = false;
	}

	loadFluid();

	// fluidSource = glm::vec2((float)xCellsCount * 0.2f, (float)yCellsCount * 0.7f);
	fluidSource = glm::vec2(2.5f, 10.5f);
	spawnFluidTimer = 0.0;
}

FluidGrid::~FluidGrid()
{
	delete uField;
	delete ps;
	delete gridMesh;
	delete triangleMesh;
	delete markerMesh;
	delete fluidSourceMesh;
	for (int i = 0; i < yCellsCount; ++i)
		delete[] liquidCells[i];
	delete[] liquidCells;
}

void FluidGrid::loadFluid()
{
	// [Bridson 2007] 2 x 2 particles per grid cell
	float space = 0.4f;
	for (float y = 0.f + (float)yCellsCount * H * 0.5f; y < (float)yCellsCount * H * 0.9f; y += space * H)
		for (float x = 0.f + (float)xCellsCount * H * 0.2f; x < (float)xCellsCount * H * 0.8f; x += space * H)
			markers.push_back(glm::vec2(x, y));
	cout << "Total markers: " << markers.size() << endl;
}

float FluidGrid::getTimeStep()
{
	// Bridson 2007, a more robust timestep calculation where no divide by zero errors will occur
	glm::vec2 maxU = uField->getMaxU();
	float u_max = sqrt(abs(H * G));
	float dist = glm::length(maxU);
	if (!isinf(dist))
		u_max += dist;
	return Kcfl * H / u_max;
}

void FluidGrid::spawnFluid(float deltaTime)
{
	// always set vel at fluid source
	float xVel = -10.f;
	float yVel = 1.f;
	uField->setCompByIdx((int)fluidSource.x, (int)fluidSource.y, 'x', xVel);
	uField->setCompByIdx((int)fluidSource.x, (int)fluidSource.y + 1, 'x', xVel);
	uField->setCompByIdx((int)fluidSource.x, (int)fluidSource.y, 'y', yVel);
	uField->setCompByIdx((int)fluidSource.x, (int)fluidSource.y + 1, 'y', yVel);
	// spawn the fluid at intervals
	spawnFluidTimer += (double)deltaTime;
	if (spawnFluidTimer > 0.1 && markers.size() < 500)
	{
		spawnFluidTimer = 0.0;
		if (markers.size() % 2)
			markers.push_back(glm::vec2(fluidSource.x, fluidSource.y + 0.4f));
		else
			markers.push_back(glm::vec2(fluidSource.x + 0.2f, fluidSource.y - 0.4f));
	}
}

void FluidGrid::Update(float deltaTime)
{
	gridMesh->ResetCellsColor();
	// spawnFluid(deltaTime);
	// timestep
	// (VERY IMPORTANT!! timestep must not be too big or else it will 'override' pressure update and cause compressibility)
	// 7f multiplier is sweet spot
	// float t = getTimeStep() * deltaTime * 7.f;
	float t = deltaTime * 1.f;
	// fluid cells update
	for (int y = 0; y < yCellsCount; ++y)
		for (int x = 0; x < xCellsCount; ++x)
			liquidCells[y][x] = false;
	for (int i = 0; i < markers.size(); ++i)
	{
		int xpos = (int)floor(markers[i].x / H);
		int ypos = (int)floor(markers[i].y / H);
		if (xpos >= 0 && xpos < xCellsCount && ypos >= 0 && ypos < yCellsCount && !liquidCells[ypos][xpos])
		{
			liquidCells[ypos][xpos] = true;
			gridMesh->colorCell(xpos, ypos, 255.f, 255.f, 230.f);
		}
		// exceed alert
		// if (xpos < 0 || xpos >= xCellsCount || ypos < 0 || ypos >= yCellsCount)
	}
	// advect + external forces
	uField->advectSelf(t);
	uField->applyExternalForces(t, liquidCells);
	uField->postUpdate();	// must be called before marker update to update prev -> curr
	// pressure update
	ps->update(*uField, liquidCells, t);
	uField->postUpdate();	// must be called before marker update to update prev -> curr
	// move particles through velocity field
	for (int i = 0; i < markers.size(); ++i)
	{
		glm::vec2 vel = uField->getVelAtPos(markers[i]);
		markers[i] += vel * t;
	}
	// rendering
	//gridMesh->updateMesh();
}


void FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	glm::mat4 mvpMat;

	/*float fluidSourceScale = 3.f;
	mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(fluidSource, 0.f)) * 
		glm::scale(glm::mat4(1.f), glm::vec3(fluidSourceScale));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(fluidSourceMesh->getVAO());
	glDrawElements(GL_TRIANGLES, fluidSourceMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);*/

	//uField2->draw(mvMat, mvpHandle, triangleMesh);

	for (int i = 0; i < markers.size(); ++i)
	{
		mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(markers[i].x, markers[i].y, 0.f));
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
		glBindVertexArray(markerMesh->getVAO());
		glDrawElements(GL_TRIANGLES, markerMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
	}

	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}