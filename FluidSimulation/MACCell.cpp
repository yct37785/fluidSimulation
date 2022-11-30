#include "MACCell.h"

MACCell::MACCell()
{
}

MACCell::~MACCell()
{
}

void MACCell::setPos(int posX, int posY)
{
	this->posX = posX;
	this->posY = posY;
}

void MACCell::Advect(MACCell** gridCells, int xCellsCount, int yCellsCount, float timestep)
{
	// bilinear interpolate
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
	float angleRad = (float)atan2(-cv.ux, cv.uy);
	float scale = glm::vec2(cv.ux, cv.uy).length();
	// u
	glm::mat4 mvpMat = mvMat *
		glm::translate(glm::mat4(1.f), glm::vec3(x + 0.5f, y + 0.5f, 0.f)) *
		glm::rotate(glm::mat4(1.f), angleRad, glm::vec3(0.f, 0.f, 1.f)) *
		glm::scale(glm::mat4(1.f), glm::vec3(scale, scale, 1.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(triangleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}