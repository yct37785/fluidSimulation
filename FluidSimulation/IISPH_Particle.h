#pragma once
#include "MeshBuilder.h"

struct IISPH_ParticleData
{
	float rho, rho_adv, p, p_prev, p0;
	float d_ii, a_ii, dij_pj;
	glm::vec2 pos;
	glm::vec2 v;
	glm::vec2 v_adv;
	glm::vec2 f;

	void init(glm::vec2 pos, glm::vec2 v)
	{
		this->pos = pos;
		this->v = v;
		this->v_adv = v;
		this->f = glm::vec2(0.f, 0.f);
		rho = rho_adv = p = p_prev = p0 = d_ii = a_ii = dij_pj = 0.f;
	}

	IISPH_ParticleData& operator=(const IISPH_ParticleData& copy)
	{
		this->rho = copy.rho;
		this->rho_adv = copy.rho_adv;
		this->p = copy.p;
		this->p_prev = copy.p_prev;
		this->p0 = copy.p0;
		this->d_ii = copy.d_ii;
		this->a_ii = copy.a_ii;
		this->dij_pj = copy.dij_pj;
		this->pos = copy.pos;
		this->v = copy.v;
		this->v_adv = copy.v_adv;
		this->f = copy.f;
		return *this;
	}
};

class IISPH_Particle
{
public:
	IISPH_ParticleData prev, curr;

	IISPH_Particle();
	~IISPH_Particle();

	void init(glm::vec2 pos, glm::vec2 v);
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);
};