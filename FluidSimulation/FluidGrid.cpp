#include "FluidGrid.h"

FluidGrid::FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	triangleMesh = MeshBuilder::CreateMesh("triangle");
	markerMesh = MeshBuilder::CreateMesh("blue_quad");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	uField = new VelocityField(xCellsCount, yCellsCount);
	uField->runUT();
	ps = new PressureSolve(xCellsCount, yCellsCount);

	for (float y = 0.f + (float)yCellsCount * 0.6f; y < (float)yCellsCount * 0.9f; y += 0.5f)
	{
		for (float x = 0.f + (float)xCellsCount * 0.05f; x < (float)xCellsCount * 0.95f; x += 0.5f)
		{
			markers.push_back(glm::vec2(x, y));
		}
	}
	liquidCells = new bool*[yCellsCount];
	for (int i = 0; i < yCellsCount; ++i)
		liquidCells[i] = new bool[xCellsCount];
	cout << "Total markers: " << markers.size() << endl;
}

FluidGrid::~FluidGrid()
{
	delete uField;
	delete ps;
	delete gridMesh;
	delete triangleMesh;
	delete markerMesh;
	for (int i = 0; i < yCellsCount; ++i)
		delete[] liquidCells[i];
	delete[] liquidCells;
}

glm::vec2 FluidGrid::getVelocityBilinear(float x, float y)
{
	return glm::vec2();
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

void FluidGrid::Update(float deltaTime)
{
	// rendering
	//gridMesh->ResetCellsColor();
	// calculate timestep
	float t = getTimeStep() * deltaTime * 20.f;
	// update liquid cells
	for (int y = 0; y < yCellsCount; ++y)
		for (int x = 0; x < xCellsCount; ++x)
			liquidCells[y][x] = false;
	for (int i = 0; i < markers.size(); ++i)
	{
		int xpos = (int)floor(markers[i].x);
		int ypos = (int)floor(markers[i].y);
		if (xpos >= 0 && xpos < xCellsCount && ypos >= 0 && ypos < yCellsCount && !liquidCells[ypos][xpos])
		{
			liquidCells[ypos][xpos] = true;
			//gridMesh->colorCell(xpos, ypos, 0.f, 255.f, 153.f);
		}
		// exceed alert
		// if (xpos < 0 || xpos >= xCellsCount || ypos < 0 || ypos >= yCellsCount)
	}
	// advection + external forces
	uField->advectSelf(t);
	uField->applyExternalForces(t, liquidCells);
	uField->postUpdate();	// must be called before marker update to update prev -> curr
	// pressure update
	ps->update(*uField, liquidCells, t);
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
	//uField->draw(mvMat, mvpHandle, triangleMesh);
	for (int i = 0; i < markers.size(); ++i)
	{
		glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(markers[i].x, markers[i].y, 0.f));
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
		glBindVertexArray(markerMesh->getVAO());
		glDrawElements(GL_TRIANGLES, markerMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
	}
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}