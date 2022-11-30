#pragma once
#include "MeshBuilder.h"

// store values in one struct for easier management
struct MACValues
{
	int state;	// 0: air, 1: liquid, 2: solid
	// center of grid cell
	float p;
	// minimal faces (i-1, j-1)
	float ux, uy;

	MACValues()
	{
		p = ux = uy = 1.f;
		state = 0;
	}

	MACValues& operator=(const MACValues& c)
	{
		state = c.state;
		p = c.p;
		ux = c.ux;
		uy = c.uy;
		return *this;
	}
};

class MACCell
{
	MACValues pv;	// prev values
	MACValues cv;	// curr values
	int posX, posY;

public:
	MACCell();
	~MACCell();

	void setPos(int posX, int posY);

	void Advect(MACCell** gridCells, int xCellsCount, int yCellsCount, float timestep);
	void postUpdate();

	void Draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh);
};