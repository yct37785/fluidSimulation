#pragma once
#include "QuantityField.h"

/*
* Positioning:
* - each grid cell starts from n -> n + 1, i.e. 0 -> 1, 1 -> 2
* - center of grid cell = n + 0.5, i.e. x: 1.5 and y: 1.5
* - vel comps are placed on centers of each face of a grid cell
* - x comps on x-axis faces and y comps on y-axis faces
*/
class VelocityField
{
	QuantityField* u, * v;
	int xCellsCount, yCellsCount;
	Mesh* uMarker, * vMarker;

public:
	VelocityField(int xCellsCount, int yCellsCount);
	~VelocityField();

	// particle-to-grid
	void advectSelf_semiLagrangian(float t, unordered_map<int, int>& liquidCells);

	// update
	void applyExternalForces(float t, unordered_map<int, int>& liquidCells);

	// grid-to-particle


	// get
	glm::vec2 getVelAtPos(glm::vec2 pos);

	// draw
	void draw(glm::mat4& mvMat, int mvpHandle);
};