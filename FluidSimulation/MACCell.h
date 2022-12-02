#pragma once
#include "MeshBuilder.h"
// defines
const float G = -9.81f;	// gravity = -9.81m/s^2, adjustable depending on visual
const float DEN_WATER = 1000.f;	// water density = 1000 kg/m^3
const float DEN_AIR = 1.f;	// air density = 1 kg/m^3
const float Kcfl = 1.f;	// timestep scale, 1 - 5
const float H = 1.f;	// width/height of a grid cell

// store values in one struct for easier management
struct MACValues
{
	int state;	// 0: air, 1: liquid, 2: solid
	// center of grid cell
	float p;
	// minimal faces (i-1, j-1)
	glm::vec2 u;

	MACValues()
	{
		p = 1.f;
		u.x = (float)rand() / (RAND_MAX / 2.f) * (rand() % 2 ? -1.f : 1.f);
		u.y = (float)rand() / (RAND_MAX / 2.f) * (rand() % 2 ? -1.f : 1.f);
		state = 0;
	}

	MACValues& operator=(const MACValues& c)
	{
		state = c.state;
		p = c.p;
		u = c.u;
		return *this;
	}
};

/*
* boundary: (cell 0 min face) -> (cell cellCount - 1 min face)
*/
class MACCell
{
	MACValues pv;	// prev values
	MACValues cv;	// curr values
	int posX, posY;

	// utilities
	static bool withinBounds(float v, float maxv);
	static float bilinearInterpolate(float x1, float x2, float y1, float y2,
		glm::vec2& pos, int comp, glm::vec2 q11, glm::vec2 q21, glm::vec2 q12, glm::vec2 q22);
	static void getIndicesCoords(float pos, float& minv, float& maxv);
	static float getVelCompAtPt(MACCell** gridCells, glm::vec2 pos, int comp, int xCellsCount, int yCellsCount);

	static float getVelocityCompAtPt(MACCell** gridCells, glm::vec2& pos, char comp, int xCellsCount, int yCellsCount);

	// unit tests
	static void UT_bilinearInterpolate();
	static void UT_getIndicesCoords();
	static void UT_getVelCompAtPt();

public:
	MACCell();
	~MACCell();

	void setPos(int posX, int posY, int xCellsCount, int yCellsCount);
	void setU(glm::vec2 new_u);

	void AdvectSelf(MACCell** gridCells, int xCellsCount, int yCellsCount, float timestep);
	void applySolidVelocities();
	void postUpdate();

	void Draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);

	int pos_x();
	int pos_y();
	float ux();
	float uy();
	float u(char comp);
	glm::vec2 u();

	static void runUT();
};