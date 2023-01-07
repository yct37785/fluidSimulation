#include "DFSPH_Particle.h"

DFSPH_Particle::DFSPH_Particle()
{
}

DFSPH_Particle::~DFSPH_Particle()
{
}

void DFSPH_Particle::init(glm::vec2 pos, glm::vec2 v)
{
	curr.init(pos, v);
	prev = curr;
}

void DFSPH_Particle::draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh)
{
	glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(curr.pos, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(particleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}