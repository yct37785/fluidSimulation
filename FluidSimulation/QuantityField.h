#pragma once
#include "MeshBuilder.h"

/*
* 2D quantity field
*/
class QuantityField
{
	float** prev, ** curr;
	float xOffset, yOffset;
	int xCount, yCount;
	Mesh* mesh;

public:
	QuantityField(int xCount, int yCount, float xOffset, float yOffset, Mesh* mesh);
	~QuantityField();

	void draw(glm::mat4& mvMat, int mvpHandle);
};