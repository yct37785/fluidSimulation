#pragma once
#include "MeshBuilder.h"

/*
* Full indices
* x comp at cell[0] min face is stored at curr[0,0]
* y comp at cell[0] min face is stored at curr[0,0]
* - By principle velocity field is stored in a staggered formation, -0.5 -> totalCells - 0.5, and
vel.x is stored in (i-1/2,j) while vel.y is stored in (i,j-1/2)
* So any pos values passed into vel_space will be offseted by (0.5, 0,5)
* - Eg: I want to find the velocity at world_space(5.6, 6.2), in vel_space it will be derive velocity at (6.1, 6.7)
* - Eg: I want to find the velocity at world_space(1.5, 1.5), in vel_space it will be derive velocity at (0.0, 0.0)
*/
class VelocityField
{
	glm::vec2** prev;
	glm::vec2** curr;
	int xCellsCount, yCellsCount;

	// utilities
	static float bilinearInterpolate(float x1, float x2, float y1, float y2,
		glm::vec2& pos, int comp, glm::vec2 q11, glm::vec2 q21, glm::vec2 q12, glm::vec2 q22);
	static void getIndicesCoords(float pos, int& minv, int& maxv);
	float getVelCompAtPt(glm::vec2 pos, int comp);

	// unit tests
	static void UT_bilinearInterpolate();
	static void UT_getIndicesCoords();
	static void UT_getVelCompAtPt();

public:
	VelocityField(int xCellsCount, int yCellsCount);
	~VelocityField();

	glm::vec2 getVelAtPos(glm::vec2 pos);
	glm::vec2 getVelByIdx(int x, int y);
	void setVelByIdx(glm::vec2 vel, int x, int y);

	static void runUT();
};