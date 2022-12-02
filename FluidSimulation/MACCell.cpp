#include "MACCell.h"

MACCell::MACCell()
{
	pv = cv;
}

MACCell::~MACCell()
{
}

bool MACCell::withinBounds(float v, float maxv)
{
	return v >= 0.f && v < maxv;
}

float MACCell::bilinearInterpolate(float x1, float x2, float y1, float y2,
	glm::vec2& pos, int comp, glm::vec2 q11, glm::vec2 q21, glm::vec2 q12, glm::vec2 q22)
{
	float h = x2 - x1;
	float r1 = q11[comp] * (x2 - pos.x) / h + q21[comp] * (pos.x - x1) / h;
	float r2 = q12[comp] * (x2 - pos.x) / h + q22[comp] * (pos.x - x1) / h;
	return r1 * (y2 - pos.y) / (y2 - y1) + r2 * (pos.y - y1) / (y2 - y1);
}

void MACCell::UT_bilinearInterpolate()
{
	glm::vec2 pos(2.3f, 2.4f);
	float x1 = 2.f, x2 = 3.f, y1 = 2.f, y2 = 3.f;
	glm::vec2 q11(22.f, 0.f);
	glm::vec2 q21(23.f, 0.f);
	glm::vec2 q12(32.f, 0.f);
	glm::vec2 q22(33.f, 0.f);
	float v = bilinearInterpolate(x1, x2, y1, y2, pos, 0, q11, q21, q12, q22);
	if (v == 26.3f)
		cout << "UT: bilinearInterpolate success" << endl;
	else
		cout << "UT: bilinearInterpolate fail" << endl;
}

void MACCell::getIndicesCoords(float pos, float& minv, float& maxv)
{
	minv = (int)floor(pos);
	maxv = (int)ceil(pos);
}

void MACCell::UT_getIndicesCoords()
{
	int successTestCount = 0;
	float minv = 0.f, maxv = 0.f;
	// v(2.1): 2 - 3
	MACCell::getIndicesCoords(2.1f, minv, maxv);
	if (minv == 2.f && maxv == 3.f)
		successTestCount++;
	// v(1.9): 1 - 2
	MACCell::getIndicesCoords(1.9f, minv, maxv);
	if (minv == 1.f && maxv == 2.f)
		successTestCount++;
	if (successTestCount == 2)
		cout << "UT: getHalfIndicesCoords success" << endl;
	else
		cout << "UT: getHalfIndicesCoords fail" << endl;
}

// full indices
float MACCell::getVelCompAtPt(MACCell** gridCells, glm::vec2 pos, int comp, int xCellsCount, int yCellsCount)
{
	// get indices
	float x1, x2, y1, y2;
	MACCell::getIndicesCoords(pos.x, x1, x2);
	MACCell::getIndicesCoords(pos.y, y1, y2);
	// bilinear interpolate with surrounding 4 cells
	float v = bilinearInterpolate(x1, x2, y1, y2, pos, comp, gridCells[(int)y1][(int)x1].u(),
		gridCells[(int)y1][(int)x2].u(), gridCells[(int)y2][(int)x1].u(), gridCells[(int)y2][(int)x2].u());
	// boundary
	return v;
}

void MACCell::UT_getVelCompAtPt()
{
	int successTestCount = 0;
	glm::vec2 vel;
	// create a grid
	int xCellsCount = 11, yCellsCount = 11;
	MACCell** gridCells = new MACCell*[yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		gridCells[y] = new MACCell[xCellsCount];
		for (int x = 0; x < xCellsCount; ++x)
		{
			gridCells[y][x].setPos(x, y, xCellsCount, yCellsCount);
		}
	}
	// test 1: manually set surround cells of (i,j), find y = (x + .3f, y + .4f)
	float test1_y_1 = 26.3f;
	gridCells[1][1].setU(glm::vec2(0.f, 22.f));
	gridCells[1][2].setU(glm::vec2(0.f, 23.f));
	gridCells[2][1].setU(glm::vec2(0.f, 32.f));
	gridCells[2][2].setU(glm::vec2(0.f, 33.f));
	float test1_y_2 = getVelCompAtPt(gridCells, glm::vec2(1.3f, 1.4f), 1, xCellsCount, yCellsCount);
	if (test1_y_1 == test1_y_2)
		successTestCount++;
	// test 2: find y at (i-1/2,j)
	// ux.y = j: y(i,j-1/2 + i,j+1/2 + i+1,j-1/2 + i+1,j+1/2) / 4
	float test2_y_1 = (gridCells[7][7].uy() + gridCells[7][8].uy() + gridCells[8][7].uy() + gridCells[8][8].uy()) / 4.f;
	float test2_y_2 = getVelCompAtPt(gridCells, glm::vec2(7.001f, 7.001f), 1, xCellsCount, yCellsCount);
	cout << test2_y_1 << ", " << test2_y_2 << endl;
	if (successTestCount == 1)
		cout << "UT: getVelCompAtPt success" << endl;
	else
		cout << "UT: getVelCompAtPt fail" << endl;
}


// pos: half-indices
float MACCell::getVelocityCompAtPt(MACCell** gridCells, glm::vec2& pos, char comp, int xCellsCount, int yCellsCount)
{
	// half-indices
	float x1, x2, y1, y2;
	// over floor
	if (pos.x - floor(pos.x) <= 0.5f)
	{
		x1 = floor(pos.x) - 0.5f;
		x2 = floor(pos.x) + 0.5f;
	}
	// below floor
	else
	{
		x1 = ceil(pos.x) - 0.5f;
		x2 = ceil(pos.x) + 0.5f;
	}
	// over floor
	if (pos.y - floor(pos.y) <= 0.5f)
	{
		y1 = floor(pos.y) - 0.5f;
		y2 = floor(pos.y) + 0.5f;
	}
	// below floor
	else
	{
		y1 = ceil(pos.y) - 0.5f;
		y2 = ceil(pos.y) + 0.5f;
	}
	// eg. i-1/2: pos indice = i: array idx
	int x1_idx = (int)ceil(x1);
	int y1_idx = (int)ceil(y1);
	int x2_idx = (int)ceil(x2);
	int y2_idx = (int)ceil(y2);
	// cout << x1 << ", " << x2 << ", " << y1 << ", " << y2 << endl;
	float q11, q21, q12, q22;
	withinBounds(x1_idx, (float)xCellsCount);
	if (!withinBounds(x1_idx, xCellsCount) || !withinBounds(y1_idx, yCellsCount)) q11 = 0.f;
	else
		q11 = gridCells[y1_idx][x1_idx].u(comp) * (x2 - pos.x) / 1.f;
	if (!withinBounds(x2_idx, xCellsCount) || !withinBounds(y1_idx, yCellsCount)) q21 = 0.f;
	else
		q21 = gridCells[y1_idx][x2_idx].u(comp) * (pos.x - x1) / 1.f;
	if (!withinBounds(x1_idx, xCellsCount) || !withinBounds(y2_idx, yCellsCount)) q12 = 0.f;
	else
		q12 = gridCells[y2_idx][x1_idx].u(comp) * (x2 - pos.x) / 1.f;
	if (!withinBounds(x2_idx, xCellsCount) || !withinBounds(y2_idx, yCellsCount)) q22 = 0.f;
	else
		q22 = gridCells[y2_idx][x2_idx].u(comp) * (pos.x - x1) / 1.f;
	float r1 = q11 + q21;
	float r2 = q12 + q22;
	return r1 * (y2 - pos.y) / (y2 - y1) + r2 * (pos.y - y1) / (y2 - y1);
}

void MACCell::setPos(int posX, int posY, int xCellsCount, int yCellsCount)
{
	this->posX = posX;
	this->posY = posY;
	// enforce boundary: no velocity flowing in/out of solid/boundary
	// boundary: (cell 0 min face) -> (cell cellCount - 1 min face)
	if (posX == 0 || posX == xCellsCount - 1)
	{
		pv.u.x = cv.u.x = 0.f;
	}
	if (posY == 0 || posY == yCellsCount - 1)
	{
		pv.u.y = cv.u.y = 0.f;
	}
}

void MACCell::setU(glm::vec2 new_u)
{
	pv.u = cv.u = new_u;
}

void MACCell::AdvectSelf(MACCell** gridCells, int xCellsCount, int yCellsCount, float timestep)
{
	int posX1 = min(xCellsCount - 1, posX + 1);
	int posY1 = min(yCellsCount - 1, posY + 1);
	// 1) get velocity(x,y) at a min face pos X
	// 2) backtrace from minimum face X by the abv velocity * timestep
	// 3) get the velocity component at backtraced pos
	/*
	   |
	ux |  i,j
	   |______
	      uy
	*/
	// half indices
	// velocity at i-1/2,j
	// ux.x = i-1/2
	glm::vec2 currPosUx((float)posX - 0.5f, (float)posY);
	glm::vec2 ux_vel;
	ux_vel.x = cv.u.x;
	// ux.y = j: y(i,j-1/2 + i,j+1/2 + i+1,j-1/2 + i+1,j+1/2) / 4
	ux_vel.y = (gridCells[posY][posX].uy() + gridCells[posY1][posX].uy()
		+ gridCells[posY][posX1].uy() + gridCells[posY1][posX1].uy()) / 4.f;
	glm::vec2 ux_prevpos = currPosUx - ux_vel * 0.f;
	// cout << currPosUx.x << ", " << currPosUx.y << ", " << ux_prevpos.x << ", " << ux_prevpos.y << endl;
	cv.u.x = getVelocityCompAtPt(gridCells, ux_prevpos, 'x', xCellsCount, yCellsCount);
	// velocity at i,j-1/2
	// ux.y = j-1/2
	glm::vec2 uy_vel;
	uy_vel.y = cv.u.y;
	// ux.x = i: x(i-1/2,j + i+1/2,j + i-1/2,j+1 + i+1/2,j+1) / 4
	uy_vel.x = (gridCells[posY][posX].ux() + gridCells[posY][posX1].ux()
		+ gridCells[posY1][posX].ux() + gridCells[posY1][posX1].ux()) / 4.f;
	glm::vec2 uy_prevpos = glm::vec2((float)posX, (float)posY - 0.5f) - uy_vel * 0.f;
	//cv.u.y = getVelocityCompAtPt(gridCells, uy_prevpos, 'y', xCellsCount, yCellsCount);
	
	//// rk2
	//glm::vec2 posux((float)xCellsCount - 0.5f, (float)yCellsCount);
	//glm::vec2 V = ux_vel;
	//V = glm::vec2(((float)xCellsCount - 0.5f) - 0.5f * timestep * V.x, ((float)yCellsCount) - 0.5f * timestep * V.y);
	//V.x = getVelocityCompAtPt(gridCells, posux, 'x', xCellsCount, yCellsCount);
	//V.y = getVelocityCompAtPt(gridCells, posux, 'y', xCellsCount, yCellsCount);
	//posux = posux + timestep * V;
	//cv.u.x = getVelocityCompAtPt(gridCells, posux, 'y', xCellsCount, yCellsCount);

	//glm::vec2 posuy((float)xCellsCount, (float)yCellsCount - 0.5f);
	//V = uy_vel;
	//V = glm::vec2(((float)xCellsCount) - 0.5f * timestep * V.x, ((float)yCellsCount - 0.5f) - 0.5f * timestep * V.y);
	//V.x = getVelocityCompAtPt(gridCells, posuy, 'x', xCellsCount, yCellsCount);
	//V.y = getVelocityCompAtPt(gridCells, posuy, 'y', xCellsCount, yCellsCount);
	//posuy = posuy + timestep * V;
	//cv.u.y = getVelocityCompAtPt(gridCells, posuy, 'y', xCellsCount, yCellsCount);
}

void MACCell::applySolidVelocities()
{
	// boundary of solid faces conditions (bodering boundary/solid cells):
	// velocity component is 0 since no liquid is flowing into/out of solid/boundary
}

void MACCell::postUpdate()
{
	// update prev values with curr values
	pv = cv;
}

void MACCell::Draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh)
{
	float x = (float)posX;
	float y = (float)posY;
	// get angular dir of velocity from lesser faces
	float angleRad = (float)atan2(-cv.u.x, cv.u.y);
	float scale = glm::length(cv.u);
	// u
	glm::mat4 mvpMat = mvMat *
		glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f)) *
		glm::rotate(glm::mat4(1.f), angleRad, glm::vec3(0.f, 0.f, 1.f)) *
		glm::scale(glm::mat4(1.f), glm::vec3(scale, scale, 1.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(triangleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}

int MACCell::pos_x()
{
	return posX;
}

int MACCell::pos_y()
{
	return posY;
}

float MACCell::ux()
{
	return pv.u.x;
}

float MACCell::uy()
{
	return pv.u.y;
}

float MACCell::u(char comp)
{
	if (comp == 'x') return pv.u.x;
	return pv.u.y;
}

glm::vec2 MACCell::u()
{
	return pv.u;
}

void MACCell::runUT()
{
	UT_bilinearInterpolate();
	UT_getIndicesCoords();
	UT_getVelCompAtPt();
}