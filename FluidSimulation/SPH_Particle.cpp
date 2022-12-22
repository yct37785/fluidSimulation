#include "SPH_Particle.h"

SPH_Particle::SPH_Particle()
{
}

SPH_Particle::~SPH_Particle()
{
}

void SPH_Particle::init(float m, float d, glm::vec2 pos)
{
	curr.init(m, d, pos);
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
	return curr.pos;
}