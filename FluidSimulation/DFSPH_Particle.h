#pragma once
#include "MeshBuilder.h"

//struct DFSPH_ParticleData
//{
//	float rho, rho_adv, p;
//	float a, rho_mat;
//	glm::vec2 pos;
//	glm::vec2 v;
//	glm::vec2 v_adv_initial;
//	glm::vec2 v_adv;
//	glm::vec2 f_adv;
//
//	void init(glm::vec2 pos, glm::vec2 v)
//	{
//		this->pos = pos;
//		this->v = this->v_adv_initial = this->v_adv = v;
//		this->f_adv = glm::vec2(0.f, 0.f);
//		rho = rho_adv = p = a = rho_mat = 0.f;
//	}
//
//	DFSPH_ParticleData& operator=(const DFSPH_ParticleData& copy)
//	{
//		this->rho = copy.rho;
//		this->rho_adv = copy.rho_adv;
//		this->p = copy.p;
//		this->a = copy.a;
//		this->rho_mat = copy.rho_mat;
//		this->pos = copy.pos;
//		this->v = copy.v;
//		this->v_adv_initial = copy.v_adv_initial;
//		this->v_adv = copy.v_adv;
//		this->f_adv = copy.f_adv;
//		return *this;
//	}
//};

class DFSPH_Particle
{
public:
	float rho, rho_adv, p;
	float a, rho_mat;
	glm::vec2 pos;
	glm::vec2 v;
	glm::vec2 v_adv;
	glm::vec2 f_adv;

	DFSPH_Particle();
	~DFSPH_Particle();

	void init(glm::vec2 pos, glm::vec2 v);
	void draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh);
};