#include "SPH_Particle.h"

SPH_Particle::SPH_Particle()
{
}

SPH_Particle::~SPH_Particle()
{
}

void SPH_Particle::init(float m, glm::vec2 pos, glm::vec2 vel)
{
	curr.init(m, pos, vel);
	prev = curr;
}

void SPH_Particle::postUpdate()
{
	prev = curr;
}

void SPH_Particle::draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh)
{
	glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(curr.pos, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(particleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}

const glm::vec2& SPH_Particle::getPos()
{
	return prev.pos;
}

float SPH_Particle::getMass()
{
	return prev.m;
}

float SPH_Particle::getDensity()
{
	return prev.d;
}

float SPH_Particle::getPressure()
{
	return prev.p;
}

void SPH_Particle::setDensity(float d)
{
	curr.d = d;
}

void SPH_Particle::setPressure(float p)
{
	curr.p = p;
}