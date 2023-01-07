#pragma once
#include "MeshBuilder.h"

struct IISPH_ParticleData
{
	float rho, p;	// density, pressur
	glm::vec2 pos;
	glm::vec2 v;
	glm::vec2 f;

	void init(glm::vec2 pos, glm::vec2 v)
	{
		this->pos = pos;
		this->v = v;
		this->f = glm::vec2(0.f, 0.f);
		rho = p = 0.f;
	}

	IISPH_ParticleData& operator=(const IISPH_ParticleData& copy)
	{
		this->rho = copy.rho;
		this->p = copy.p;
		this->pos = copy.pos;
		this->v = copy.v;
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