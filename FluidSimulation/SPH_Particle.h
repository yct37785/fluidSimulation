#pragma once
#include "MeshBuilder.h"

class SPH_Particle
{
	// given
	float m, d;
	glm::vec2 pos;
	// to be derived
	float p;

public:
	SPH_Particle();
	~SPH_Particle();

	void init(float m, float d, glm::vec2 pos);
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);

	const glm::vec2& getPos();
};