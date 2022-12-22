#pragma once
#include "MeshBuilder.h"

struct SPH_ParticleData
{
	float m, d;	// mass, density
	glm::vec2 pos;	// pos
	float p;	// pressure

	void init(float m, float d, glm::vec2 pos)
	{
		this->m = m;
		this->d = d;
		this->pos = pos;
		p = 0.f;
	}

	SPH_ParticleData& operator=(const SPH_ParticleData& copy)
	{
		this->m = copy.m;
		this->d = copy.d;
		this->pos = copy.pos;
		p = copy.p;
	}
};

class SPH_Particle
{
	SPH_ParticleData prev, curr;

public:
	SPH_Particle();
	~SPH_Particle();

	void init(float m, float d, glm::vec2 pos);
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);

	const glm::vec2& getPos();
};