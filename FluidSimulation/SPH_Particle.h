#pragma once
#include "MeshBuilder.h"

struct SPH_ParticleData
{
	float m, d, p;	// mass, density, pressure
	glm::vec2 pos;	// pos
	glm::vec2 vel;

	void init(float m, glm::vec2 pos, glm::vec2 vel)
	{
		this->m = m;
		this->pos = pos;
		this->vel = vel;
		d = p = 0.f;
	}

	SPH_ParticleData& operator=(const SPH_ParticleData& copy)
	{
		this->m = copy.m;
		this->d = copy.d;
		this->p = copy.p;
		this->pos = copy.pos;
		this->vel = copy.vel;
		return *this;
	}
};

class SPH_Particle
{
	SPH_ParticleData prev, curr;

public:
	vector<int> neighbors;
	vector<float> neighborDist;

	SPH_Particle();
	~SPH_Particle();

	void init(float m, glm::vec2 pos, glm::vec2 vel);
	void postUpdate();
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);

	const glm::vec2& getPos();
	float getMass();
	float getDensity();
	float getPressure();
	void setDensity(float d);
	void setPressure(float p);
};