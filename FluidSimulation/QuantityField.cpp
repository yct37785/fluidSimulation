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

bool QuantityField::outOfBounds(int x, int y)
{
	return x < 0.f || x >= xCount || y < 0.f || y >= yCount;
}

float QuantityField::getQuantityAtIdx(int x, int y)
{
	if (outOfBounds(x, y))
		return 0.f;
	return prev[y][x];
}

void QuantityField::setQuantityAtIdx(int x, int y, float value)
{
	if (!outOfBounds(x, y))
		curr[y][x] = value;
}

float QuantityField::getQuantityAtPos(glm::vec2 pos)
{
	// world space -> q field space
	// eg. 1.7 -> 1.2
	pos.x -= xOffset;
	pos.y -= yOffset;
	// normalize
	glm::vec2 normPos = pos / H;
	// indexes
	int x1, x2, y1, y2;
	x1 = (int)floor(normPos.x);
	x2 = (int)ceil(normPos.x);
	y1 = (int)floor(normPos.y);
	y2 = (int)ceil(normPos.y);
	// out of bounds = 0
	float q11 = outOfBounds(x1, y1) ? 0.f : prev[y1][x1];
	float q21 = outOfBounds(x2, y1) ? 0.f : prev[y1][x2];
	float q12 = outOfBounds(x1, y2) ? 0.f : prev[y2][x1];
	float q22 = outOfBounds(x2, y2) ? 0.f : prev[y2][x2];
	// bilinear
	return bilinearInterpolate(x1, x2, y1, y2, normPos, q11, q21, q12, q22);
}

float QuantityField::getNeighboringAvg(int x, int y)
{
	if (outOfBounds(x, y))
		return 0.f;
	int offset[4][2] = { { 0, 1 }, { 0, -1 }, { 1, 0 }, { -1, 0 } };
	int count = 0;
	float sum = 0.f;
	for (int i = 0; i < 4; ++i)
	{
		if (!outOfBounds(x + offset[i][0], y + offset[i][1]))
		{
			count++;
			sum += prev[y + offset[i][1]][x + offset[i][0]];
		}
	}
	return sum / (float)count;
}

void QuantityField::postUpdate()
{
	for (int y = 0; y < yCount; ++y)
	{
		for (int x = 0; x < xCount; ++x)
			prev[y][x] = curr[y][x];
	}
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