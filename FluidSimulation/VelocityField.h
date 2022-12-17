#pragma once
#include "MeshBuilder.h"

/*
* Positioning:
* - each grid cell starts from n -> n + 1, i.e. 0 -> 1, 1 -> 2
* - center of grid cell = n + 0.5, i.e. x: 1.5 and y: 1.5
* - vel comps are placed on centers of each face of a grid cell
* - x comps on x-axis faces and y comps on y-axis faces
*/
class VelocityField
{
	float** x_prev, ** y_prev, ** x_curr, ** y_curr;
	int xCellsCount, yCellsCount;
	Mesh* xMarker, *yMarker;

	// utilities
	static bool outOfRange(int x, int y, int maxx, int maxy);
	static void getIndicesCoords(float pos, int& minv, int& maxv);
	static float bilinearInterpolate(float x1, float x2, float y1, float y2,
		glm::vec2& pos, float q11, float q21, float q12, float q22);
	float getVelCompAtPos(glm::vec2& pos, char comp);
	bool isLiquidCell(int x, int y, bool** liquidCells);

public:
	VelocityField(int xCellsCount, int yCellsCount);
	~VelocityField();

	void advectSelf(float t);
	void applyExternalForces(float t, bool** liquidCells);
	void postUpdate();

	glm::vec2 getVelAtPos(glm::vec2& pos);
	float getCompByIdx(int x, int y, char comp);
	void addToCompByIdx(int x, int y, char comp, float v);
	void setCompByIdx(int x, int y, char comp, float v);

	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);

	glm::vec2 getMaxU();
};