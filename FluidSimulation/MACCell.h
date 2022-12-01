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
		u.x = 1.f;
		u.y = 1.f;
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

class MACCell
{
	MACValues pv;	// prev values
	MACValues cv;	// curr values
	int posX, posY;

	// utilities
	//static bool isBoundaryFace(float x, float y, int xCellsCount, int yCellsCount);
	static float getVelocityCompAtPt(MACCell** gridCells, glm::vec2& pos, char comp, int xCellsCount, int yCellsCount, float timestep);

public:
	MACCell();
	~MACCell();

	void setPos(int posX, int posY);

	void AdvectSelf(MACCell** gridCells, int xCellsCount, int yCellsCount, float timestep);
	void applySolidVelocities();
	void postUpdate();

	void Draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);

	float ux();
	float uy();
	float u(char comp);
};