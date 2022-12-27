#include "MAC_FluidGrid.h"

MAC_FluidGrid::MAC_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	triangleMesh = MeshBuilder::CreateMesh("triangle");
	markerMesh = MeshBuilder::CreateMesh("blue_marker");
	fluidSourceMesh = MeshBuilder::CreateMesh("orange_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	uField = new MAC_VelocityField(xCellsCount, yCellsCount);
	// uField->runUT();
	ps = new MAC_PressureSolve(xCellsCount, yCellsCount);

	// fluidSource = glm::vec2((float)xCellsCount * 0.2f, (float)yCellsCount * 0.7f);
	fluidSource = glm::vec2(2.5f, 10.5f);
	spawnFluidTimer = 0.0;
}

MAC_FluidGrid::~MAC_FluidGrid()
{
	delete uField;
	delete ps;
	delete gridMesh;
	delete triangleMesh;
	delete markerMesh;
	delete fluidSourceMesh;
}

float MAC_FluidGrid::getTimeStep()
{
	// Bridson 2007, a more robust timestep calculation where no divide by zero errors will occur
	float dist = uField->getMaxU();
	float u_max = sqrt(abs(H * G));
	u_max += dist;
	return Kcfl * H / u_max;
}

void MAC_FluidGrid::spawnFluid()
{
	// [Bridson 2007] 2 x 2 particles per grid cell
	/*float xmin = 0.3f;
	float xmax = 0.7f;
	float ymin = 0.3f;
	float ymax = 0.7f;*/
	float xmin = 0.55f;
	float xmax = 0.85f;
	float ymin = 0.6f;
	float ymax = 0.8f;
	float space = 0.4f;
	for (float y = 0.f + (float)yCellsCount * H * ymin; y < (float)yCellsCount * H * ymax; y += space * H)
	{
		for (float x = 0.f + (float)xCellsCount * H * xmin; x < (float)xCellsCount * H * xmax; x += space * H)
		{
			float jitterX = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * H;
			float jitterY = static_cast<float>(rand()) / static_cast<float>(RAND_MAX) * H;
			markers.push_back(glm::vec2(x + jitterX, y + jitterY));
		}
	}
	cout << "Total markers: " << markers.size() << endl;
}

void MAC_FluidGrid::Update(float deltaTime)
{
	gridMesh->ResetCellsColor();
	// timestep
	// (VERY IMPORTANT!! timestep must not be too big or else it will 'override' pressure update and cause compressibility)
	// 7f multiplier is sweet spot
	//float t = getTimeStep() * deltaTime * 20.f;
	float t = deltaTime * 4.f;
	// fluid cells update
	liquidCells.clear();
	int count = 0;
	for (int i = 0; i < markers.size(); ++i)
	{
		int xpos = (int)floor(markers[i].x / H);
		int ypos = (int)floor(markers[i].y / H);
		int idx = ypos * xCellsCount + xpos;
		if (xpos >= 0 && xpos < xCellsCount && ypos >= 0 && ypos < yCellsCount && !liquidCells.count(idx))
		{
			liquidCells[idx] = count;
			count++;
			gridMesh->colorCell(xpos, ypos, 255.f, 255.f, 230.f);
		}
		//// also put neighbouring cells
		//int x1 = xpos - 1, x2 = xpos + 1;
		//int y1 = ypos - 1, y2 = ypos + 1;
		//if (x1 >= 0 && x1 < xCellsCount && ypos >= 0 && ypos < yCellsCount && !liquidCells.count(ypos * xCellsCount + x1))
		//	liquidCells[ypos * xCellsCount + x1] = count++;
		//if (x2 >= 0 && x2 < xCellsCount && ypos >= 0 && ypos < yCellsCount && !liquidCells.count(ypos * xCellsCount + x2))
		//	liquidCells[ypos * xCellsCount + x2] = count++;
		//if (y1 >= 0 && y1 < xCellsCount && xpos >= 0 && xpos < xCellsCount && !liquidCells.count(y1 * xCellsCount + xpos))
		//	liquidCells[y1 * xCellsCount + xpos] = count++;
		//if (y2 >= 0 && y2 < xCellsCount && xpos >= 0 && xpos < xCellsCount && !liquidCells.count(y2 * xCellsCount + xpos))
		//	liquidCells[y2 * xCellsCount + xpos] = count++;
	}
	// advect + external forces
	uField->advectSelf(t, liquidCells);
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


void MAC_FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	glm::mat4 mvpMat;

	/*float fluidSourceScale = 3.f;
	mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(fluidSource, 0.f)) * 
		glm::scale(glm::mat4(1.f), glm::vec3(fluidSourceScale));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(fluidSourceMesh->getVAO());
	glDrawElements(GL_TRIANGLES, fluidSourceMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);*/

	// uField->draw(mvMat, mvpHandle, triangleMesh);

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