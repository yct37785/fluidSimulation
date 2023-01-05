#pragma once
#include "MeshBuilder.h"

struct SPH_ParticleData
{
	float rho, p, a;	// density, pressure, factor (for DFSPH)
	float rho_mat;	// (for DFSPH)
	glm::vec2 pos;	// pos
	glm::vec2 v;
	glm::vec2 f;

	void init(glm::vec2 pos, glm::vec2 v)
	{
		this->pos = pos;
		this->v = v;
		this->f = glm::vec2(0.f, 0.f);
		rho = p = a = rho_mat = 0.f;
	}

	SPH_ParticleData& operator=(const SPH_ParticleData& copy)
	{
		this->rho = copy.rho;
		this->p = copy.p;
		this->a = copy.a;
		this->rho_mat = copy.rho_mat;
		this->pos = copy.pos;
		this->v = copy.v;
		this->f = copy.f;
		return *this;
	}
};

class SPH_Particle
{
	SPH_ParticleData prev, curr;

public:
	SPH_Particle();
	~SPH_Particle();

	void init(glm::vec2 pos, glm::vec2 v);
	void postUpdate();
	void forwardEuler(float t);
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);

	const glm::vec2& pos();
	void pos(glm::vec2 pos);
	float rho();
	void rho(float rho);
	float p();
	void p(float p);
	float a();
	void a(float a);
	float rho_mat();
	void rho_mat(float rho_mat);
	glm::vec2 f();
	void f(glm::vec2 f);
	glm::vec2 v();
	void v(glm::vec2 v);
};