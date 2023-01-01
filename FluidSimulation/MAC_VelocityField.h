#pragma once
#include "MeshBuilder.h"

/*
* Positioning:
* - each grid cell starts from n -> n + 1, i.e. 0 -> 1, 1 -> 2
* - center of grid cell = n + 0.5, i.e. x: 1.5 and y: 1.5
* - vel comps are placed on centers of each face of a grid cell
* - x comps on x-axis faces and y comps on y-axis faces
*/
class MAC_VelocityField
{
	float** x_prev, ** y_prev, ** x_curr, ** y_curr;
	int xCellsCount, yCellsCount;
	Mesh* xMarker, *yMarker;

	// utilities
	static bool outOfRange(int x, int y, int maxx, int maxy);
	static void getIndicesCoords(float pos, int& minv, int& maxv);
	float getVelCompAtPos(glm::vec2& pos, char comp);
	bool isLiquidCell(int x, int y, unordered_map<int, int>& liquidCells);

public:
	MAC_VelocityField(int xCellsCount, int yCellsCount);
	~MAC_VelocityField();

	void advectSelf(float t, unordered_map<int, int>& liquidCells);
	void applyExternalForces(float t, unordered_map<int, int>& liquidCells);
	void postUpdate();

	glm::vec2 getVelAtPos(glm::vec2& pos);
	float getCompByIdx(int x, int y, char comp);
	void addToCompByIdx(int x, int y, char comp, float v);
	void setCompByIdx(int x, int y, char comp, float v);

	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);

	float getMaxU();
};