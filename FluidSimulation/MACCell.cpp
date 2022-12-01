#include "MACCell.h"

MACCell::MACCell()
{
}

MACCell::~MACCell()
{
}

// is a given face pos at the boundary (half indices)
//bool MACCell::isBoundaryFace(float x, float y, int xCellsCount, int yCellsCount)
//{
//	return (x <= -0.5f || x >= (float)xCellsCount + 0.5f) ||
//		(y <= -0.5f || y >= (float)yCellsCount + 0.5f);
//}

bool MACCell::withinBounds(float v, float maxv)
{
	return v >= 0.f && v < maxv;
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
	// cout << x1 << ", " << x2 << ", " << y1 << ", " << y2 << " s: " << ceil(-1.5) << endl;
	float q11, q21, q12, q22;
	withinBounds(x1_idx, (float)xCellsCount);
	if (!withinBounds(x1_idx, xCellsCount) || !withinBounds(y1_idx, yCellsCount)) q11 = 0.f;
	else
		q11 = gridCells[y1_idx][x1_idx].u(comp, x1_idx, y1_idx) * (x2 - pos.x) / (x2 - x1);
	if (!withinBounds(x2_idx, xCellsCount) || !withinBounds(y1_idx, yCellsCount)) q21 = 0.f;
	else
		q21 = gridCells[y1_idx][x2_idx].u(comp, x2_idx, y1_idx) * (pos.x - x1) / (x2 - x1);
	if (!withinBounds(x1_idx, xCellsCount) || !withinBounds(y2_idx, yCellsCount)) q12 = 0.f;
	else
		q12 = gridCells[y2_idx][x1_idx].u(comp, x1_idx, y2_idx) * (x2 - pos.x) / (x2 - x1);
	if (!withinBounds(x2_idx, xCellsCount) || !withinBounds(y2_idx, yCellsCount)) q22 = 0.f;
	else
		q22 = gridCells[y2_idx][x2_idx].u(comp, x2_idx, y2_idx) * (pos.x - x1) / (x2 - x1);
	float r1 = q11 + q21;
	float r2 = q12 + q22;
	return r1 * (y2 - pos.y) / (y2 - y1) + r2 * (pos.y - y1) / (y2 - y1);
}

void MACCell::setPos(int posX, int posY)
{
	this->posX = posX;
	this->posY = posY;
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
	glm::vec2 ux_vel;
	ux_vel.x = cv.u.x;
	// ux.y = j: y(i,j-1/2 + i,j+1/2 + i+1,j-1/2 + i+1,j+1/2) / 4
	ux_vel.y = (gridCells[posY][posX].uy() + gridCells[posY1][posX].uy()
		+ gridCells[posY][posX1].uy() + gridCells[posY1][posX1].uy()) / 4.f;
	glm::vec2 ux_prevpos = glm::vec2((float)posX - 0.5f, (float)posY) - ux_vel * timestep;
	/*cout << "Before: " << (float)posX - 0.5f << ", " << (float)posY << endl;
	cout << "After: " << ux_prevpos.x << ", " << ux_prevpos.y << endl;*/
	cv.u.x = getVelocityCompAtPt(gridCells, ux_prevpos, 'x', xCellsCount, yCellsCount);
	// velocity at i,j-1/2
	// ux.y = j-1/2
	glm::vec2 uy_vel;
	uy_vel.y = cv.u.y;
	// ux.x = i: x(i-1/2,j + i+1/2,j + i-1/2,j+1 + i+1/2,j+1) / 4
	uy_vel.x = (gridCells[posY][posX].ux() + gridCells[posY][posX1].ux()
		+ gridCells[posY1][posX].ux() + gridCells[posY1][posX1].ux()) / 4.f;
	glm::vec2 uy_prevpos = glm::vec2((float)posX, (float)posY - 0.5f) - uy_vel * timestep;
	cv.u.y = getVelocityCompAtPt(gridCells, uy_prevpos, 'y', xCellsCount, yCellsCount);
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
	float scale = glm::vec2(cv.u.x, cv.u.y).length();
	// u
	glm::mat4 mvpMat = mvMat *
		glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f)) *
		glm::rotate(glm::mat4(1.f), angleRad, glm::vec3(0.f, 0.f, 1.f)) *
		glm::scale(glm::mat4(1.f), glm::vec3(scale, scale, 1.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(triangleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}

float MACCell::ux()
{
	return pv.u.x;
}

float MACCell::uy()
{
	return pv.u.y;
}

float MACCell::u(char comp, int x, int y)
{
	if (comp == 'x') return pv.u.x;
	return pv.u.y;
}