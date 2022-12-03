#pragma once
#include "MeshBuilder.h"

/*
* Full indices, we won't be doing staggered grids until we can get a basic simulator running
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

	void advectSelf(float t);
	void applyExternalForces(glm::vec2 F, float t);
	void postUpdate();

	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);

	glm::vec2 getVelAtPos(glm::vec2 pos);
	glm::vec2 getVelByIdx(int x, int y);
	void setVelByIdx(glm::vec2 vel, int x, int y);

	static void runUT();
};