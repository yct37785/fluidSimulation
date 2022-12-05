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

	for (float y = 10.f; y < 12.5f; y += 0.5f)
	{
		for (float x = 1.f; x < 14.f; x += 0.5f)
		{
			markers.push_back(glm::vec2(x, y));
		}
	}
	cout << "Total markers: " << markers.size() << endl;
}

FluidGrid::~FluidGrid()
{
	delete uField;
	delete ps;
	delete gridMesh;
	delete triangleMesh;
	delete markerMesh;
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
	liquidCells.clear();
	float t = getTimeStep() * deltaTime * 20.f;
	// advection + external forces
	uField->advectSelf(t);
	uField->applyExternalForces(t);
	uField->postUpdate();	// must be called before marker update to update prev -> curr
	// move particles through velocity field
	for (int i = 0; i < markers.size(); ++i)
	{
		glm::vec2 vel = uField->getVelAtPos(markers[i]);
		markers[i] += vel * t;
		liquidCells.insert(make_pair((int)floor(markers[i].x) * (int)floor(markers[i].y), 1));
	}
	ps->update(*uField, liquidCells, t);
}


void FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	// uField->draw(mvMat, mvpHandle, triangleMesh);
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