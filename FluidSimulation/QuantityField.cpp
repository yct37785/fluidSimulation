#include "QuantityField.h"

QuantityField::QuantityField(int xCount, int yCount, float xOffset, float yOffset, Mesh* mesh)
{
	this->xCount = xCount;
	this->yCount = yCount;
	prev = new float* [yCount];
	curr = new float* [yCount];
	for (int y = 0; y < yCount; ++y)
	{
		prev[y] = new float[xCount];
		curr[y] = new float[xCount];
		for (int x = 0; x < xCount; ++x)
			prev[y][x] = curr[y][x] = 0.f;
	}
	this->mesh = mesh;
	this->xOffset = xOffset;
	this->yOffset = yOffset;
}

QuantityField::~QuantityField()
{
	for (int y = 0; y < yCount; ++y)
	{
		delete[] prev[y];
		delete[] curr[y];
	}
	delete[] prev;
	delete[] curr;
}

void QuantityField::draw(glm::mat4& mvMat, int mvpHandle)
{
	for (int y = 0; y < yCount; ++y)
	{
		for (int x = 0; x < xCount; ++x)
		{
			glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3((x + xOffset) * H, (y + yOffset) * H, 0.f));
			glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
			glBindVertexArray(mesh->getVAO());
			glDrawElements(GL_TRIANGLES, mesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
		}
	}
}