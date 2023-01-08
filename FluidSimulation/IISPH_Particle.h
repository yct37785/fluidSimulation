#pragma once
#include "MeshBuilder.h"

class IISPH_Particle
{
public:
	float rho, rho_adv, p, p_prev, p0;
	float d_ii, a_ii, dij_pj;
	glm::vec2 pos;
	glm::vec2 v;
	glm::vec2 v_adv;
	glm::vec2 f;

	IISPH_Particle();
	~IISPH_Particle();

	void init(glm::vec2 pos, glm::vec2 v);
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);
};