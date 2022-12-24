#pragma once
#include "MeshBuilder.h"

struct SPH_ParticleData
{
	float m, d, p, a;	// mass, density, pressure, acceleration
	glm::vec2 pos;	// pos
	glm::vec2 vel;

	void init(float m, glm::vec2 pos, glm::vec2 vel)
	{
		this->m = m;
		this->pos = pos;
		this->vel = vel;
		d = p = a = 0.f;
	}

	SPH_ParticleData& operator=(const SPH_ParticleData& copy)
	{
		this->m = copy.m;
		this->d = copy.d;
		this->p = copy.p;
		this->a = copy.a;
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
	void updatePosByVel(float t);
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);

	const glm::vec2& getPos();
	float getMass();
	float getDensity();
	float getPressure();
	float getAcceleration();
	glm::vec2 getVel();
	void setDensity(float d);
	void setPressure(float p);
	void accelerateVel(glm::vec2 acc);
	void applyGravity();
};