#include "SPH_Particle.h"

SPH_Particle::SPH_Particle()
{
}

SPH_Particle::~SPH_Particle()
{
}

void SPH_Particle::init(glm::vec2 pos, glm::vec2 v)
{
	curr.init(pos, v);
	prev = curr;
}

void SPH_Particle::postUpdate()
{
	prev = curr;
}

void SPH_Particle::forwardEuler(float t)
{
	curr.v += t * curr.f / curr.rho;
	curr.pos += t * curr.v;
}

void SPH_Particle::draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh)
{
	glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(curr.pos, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(particleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}

const glm::vec2& SPH_Particle::pos() { return prev.pos; }
void SPH_Particle::pos(glm::vec2 v) { curr.pos = v; }

float SPH_Particle::rho() { return prev.rho; }
void SPH_Particle::rho(float v) { curr.rho = v; }

float SPH_Particle::p() { return prev.p; }
void SPH_Particle::p(float v) { curr.p = v; }

glm::vec2 SPH_Particle::f() { return prev.f; }
void SPH_Particle::f(glm::vec2 v) { curr.f = v; }

glm::vec2 SPH_Particle::v() { return prev.v; }
void SPH_Particle::v(glm::vec2 v) { curr.v = v; }