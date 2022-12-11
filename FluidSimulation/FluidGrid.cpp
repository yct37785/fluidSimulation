#include "FluidGrid.h"

FluidGrid::FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	triangleMesh = MeshBuilder::CreateMesh("triangle");
	markerMesh = MeshBuilder::CreateMesh("blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	uField2 = new VelocityField2(xCellsCount, yCellsCount);
	// uField->runUT();
	ps2 = new PressureSolve2(xCellsCount, yCellsCount);

	// [Bridson 2007] 2 x 2 particles per grid cell
	for (float y = 0.f + (float)yCellsCount * H * 0.6f; y < (float)yCellsCount * H * 0.8f; y += 0.4f * H)
	{
		for (float x = 0.f + (float)xCellsCount * H * 0.2f; x < (float)xCellsCount * H * 0.8f; x += 0.4f * H)
		{
			markers.push_back(glm::vec2(x, y));
		}
	}
	liquidCells = new bool*[yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		liquidCells[y] = new bool[xCellsCount];
		for (int x = 0; x < xCellsCount; ++x)
			liquidCells[y][x] = false;
	}
	cout << "Total markers: " << markers.size() << endl;
}

FluidGrid::~FluidGrid()
{
	delete uField2;
	delete ps2;
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
	glm::vec2 maxU = uField2->getMaxU();
	float u_max = sqrt(abs(H * G));
	float dist = glm::length(maxU);
	if (!isinf(dist))
		u_max += dist;
	return Kcfl * H / u_max;
}

void FluidGrid::Update(float deltaTime)
{
	gridMesh->ResetCellsColor();
	// timestep
	// (VERY IMPORTANT!! timestep must not be too big or else it will 'override' pressure update and cause compressibility)
	// 7f multiplier is sweet spot
	float t = getTimeStep() * deltaTime * 7.f;
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
			gridMesh->colorCell(xpos, ypos, 0.f, 255.f, 153.f);
		}
		// exceed alert
		// if (xpos < 0 || xpos >= xCellsCount || ypos < 0 || ypos >= yCellsCount)
	}
	// advect + external forces
	uField2->advectSelf(t);
	uField2->applyExternalForces(t, liquidCells);
	uField2->postUpdate();	// must be called before marker update to update prev -> curr
	// pressure update
	ps2->update(*uField2, liquidCells, t);
	uField2->postUpdate();	// must be called before marker update to update prev -> curr
	// move particles through velocity field
	for (int i = 0; i < markers.size(); ++i)
	{
		glm::vec2 vel = uField2->getVelAtPos(markers[i]);
		markers[i] += vel * t;
	}
	// rendering
	//gridMesh->updateMesh();
}


void FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	uField2->draw(mvMat, mvpHandle, triangleMesh);
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