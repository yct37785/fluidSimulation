#include "IISPH_Particle.h"

IISPH_Particle::IISPH_Particle()
{
}

IISPH_Particle::~IISPH_Particle()
{
}

void IISPH_Particle::init(glm::vec2 pos, glm::vec2 v)
{
	this->pos = pos;
	this->v = v;
	this->v_adv = v;
	this->f = glm::vec2(0.f, 0.f);
	rho = rho_adv = p = p_prev = p0 = d_ii = a_ii = dij_pj = 0.f;
}

void IISPH_Particle::draw(glm::mat4& mvMat, int mvpHandle, Mesh* particleMesh)
{
	glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(pos, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(particleMesh->getVAO());
	glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
