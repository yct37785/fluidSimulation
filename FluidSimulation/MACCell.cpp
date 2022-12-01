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

// half-indices
glm::vec2 MACCell::getVelocityAtPt(MACCell** gridCells, glm::vec2 pos, int xCellsCount, int yCellsCount, float timestep)
{
	float x1 = max(0.f, floor(pos.x - 0.5f));
	float y1 = max(0.f, floor(pos.y - 0.5f));
	float x2 = min((float)xCellsCount, ceil(pos.x - 0.5f));
	float y2 = min((float)xCellsCount, ceil(pos.x - 0.5f));
	cout << x1 << ", " << y1 << ", " << x2 << ", " << y2 << endl;
	// x comp
	return glm::vec2();
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
	// velocity at i,j-1/2
	// ux.y = j-1/2
	glm::vec2 uy_vel;
	uy_vel.y = cv.u.y;
	// ux.x = i: x(i-1/2,j + i+1/2,j + i-1/2,j+1 + i+1/2,j+1) / 4
	uy_vel.x = (gridCells[posY][posX].uy() + gridCells[posY][posX1].uy()
		+ gridCells[posY1][posX].uy() + gridCells[posY1][posX1].uy()) / 4.f;
	glm::vec2 uy_prevpos = glm::vec2((float)posX, (float)posY - 0.5f) - uy_vel * timestep;
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
	return cv.u.x;
}

float MACCell::uy()
{
	return cv.u.y;
}