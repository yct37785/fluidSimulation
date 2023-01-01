#include "Particle.h"

Particle::Particle(glm::vec2 pos, glm::vec2 u)
{
	this->pos = pos;
	this->u = u;
}

Particle::~Particle()
{
}

void Particle::draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh)
{
}

void Particle::forwardEuler(glm::vec2 vel, float t)
{
	pos += vel * t;
}

void Particle::set_u(glm::vec2 u) { this->u = u; }

glm::vec2 Particle::get_u() { return u; }

void Particle::set_pos(glm::vec2 pos) { this->pos = pos; }

glm::vec2 Particle::get_pos() { return pos; }