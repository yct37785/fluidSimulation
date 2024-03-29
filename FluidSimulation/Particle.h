#pragma once
#include "MeshBuilder.h"
#include "GridInfo.h"

/*
* Can be used for:
* - semi-Lagrangian (backwards trace)
* - Lagrangian (PIC/FLIP)
*/
class Particle
{
	glm::vec2 pos;
	glm::vec2 u;

public:
	Particle(glm::vec2 pos, glm::vec2 u);
	~Particle();

	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);

	void forwardEuler(glm::vec2 vel, float t);

	void set_u(glm::vec2 u);
	glm::vec2 get_u();
	void set_pos(glm::vec2 pos);
	glm::vec2 get_pos();
};
