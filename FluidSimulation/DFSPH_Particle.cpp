#include "DFSPH_Particle.h"

DFSPH_Particle::DFSPH_Particle()
{
}

DFSPH_Particle::~DFSPH_Particle()
{
}

void DFSPH_Particle::init(glm::vec2 pos, glm::vec2 v)
{
	this->pos = pos;
	this->v = this->v_adv = v;
	this->f_adv = glm::vec2(0.f, 0.f);
	rho = rho_adv = p = a = rho_mat = 0.f;
}

void DFSPH_Particle::draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh)
{
	glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(pos, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(particleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
