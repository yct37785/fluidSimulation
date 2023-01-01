#pragma once
#include "Particle.h"

/*
* 2D quantity field
*/
class QuantityField
{
	float** prev, ** curr;
	float xOffset, yOffset;
	int xCount, yCount;
	Mesh* mesh;

	// utils
	bool outOfBounds(int x, int y);

public:
	QuantityField(int xCount, int yCount, float xOffset, float yOffset, Mesh* mesh);
	~QuantityField();

	float getQuantityAtIdx(int x, int y);
	void setQuantityAtIdx(int x, int y, float value);
	float getQuantityAtPos(glm::vec2 pos);
	float getNeighboringAvg(int x, int y);
	void postUpdate();

	void draw(glm::mat4& mvMat, int mvpHandle);
};