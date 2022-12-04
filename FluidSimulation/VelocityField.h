#pragma once
#include "MeshBuilder.h"

/*
* Staggered positioning:
* - x comps are stored starting from (x: 0, y: 1/2)
* - y comps are stored starting from (x: 1/2, y: 0)
* Advection stage:
* - backward trace must be applied per velocity component pos
*/
class VelocityField
{
	glm::vec2** prev;
	glm::vec2** curr;
	int xCellsCount, yCellsCount;

	// utilities
	static bool outOfRange(int x, int y, int maxx, int maxy);
	static float bilinearInterpolate(float x1, float x2, float y1, float y2, 
		glm::vec2& pos, float q11, float q21, float q12, float q22);
	static void getHalfIndicesCoords(float pos, float& minv, float& maxv);
	static void getIndicesCoords(float pos, int& minv, int& maxv);
	static void getIndices(glm::vec2 pos, char comp, int xCellsCount, int yCellsCount,
		int& x1, int& x2, int& y1, int& y2);
	float getVelCompAtPt(glm::vec2 pos, int comp);

	// unit tests
	static void UT_bilinearInterpolate();
	static void UT_getHalfIndicesCoords();
	static void UT_getIndicesCoords();
	static void UT_getIndices();
	static void UT_getVelCompAtPt();

public:
	VelocityField(int xCellsCount, int yCellsCount);
	~VelocityField();

	void advectSelf(float t);
	void applyExternalForces(float t);
	void postUpdate();

	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);

	glm::vec2 getMaxU();
	glm::vec2 getVelAtPos(glm::vec2 pos);
	void setVelByIdx(glm::vec2 vel, int x, int y);

	static void runUT();
};