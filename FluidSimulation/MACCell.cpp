#include "MACCell.h"

MACCell::MACCell()
{
	p = ux = uy = 1.f;
	posX = posY = state = 0;
}

MACCell::~MACCell()
{
}

void MACCell::setPos(int posX, int posY)
{
	this->posX = posX;
	this->posY = posY;
}

void MACCell::Advect(MACCell& tl, MACCell& tr, MACCell& bl, MACCell& br)
{
	// bilinear interpolate
}

void MACCell::Draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh)
{
	float x = (float)posX;
	float y = (float)posY;
	// u
	glm::mat4 mvpMat = mvMat *
		glm::translate(glm::mat4(1.f), glm::vec3(x, y + 0.5f, 0.f)) *
		glm::rotate(glm::mat4(1.f), ux >= 0.f ? 0.f : 3.14159f, glm::vec3(0.f, 0.f, 1.f)) *
		glm::scale(glm::mat4(1.f), glm::vec3(ux, ux, 1.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(triangleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
	// v
	mvpMat = mvMat *
		glm::translate(glm::mat4(1.f), glm::vec3(x + 0.5f, y, 0.f)) *
		glm::rotate(glm::mat4(1.f), uy >= 0.f ? -1.5708f : 1.5708f, glm::vec3(0.f, 0.f, 1.f)) *
		glm::scale(glm::mat4(1.f), glm::vec3(uy, uy, 1.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(triangleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}