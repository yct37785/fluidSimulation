#include "DFSPH_FluidGrid.h"

DFSPH_FluidGrid::DFSPH_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("sph_blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	viewWidth = (float)xCellsCount * Hrad;
	viewHeight = (float)yCellsCount * Hrad;
}

DFSPH_FluidGrid::~DFSPH_FluidGrid()
{
	for (int i = 0; i < particles.size(); ++i)
		delete particles[i];
}

void DFSPH_FluidGrid::spawnParticles()
{
	for (float y = EPS; y < viewHeight - EPS * 2.f; y += Hrad)
	{
		for (float x = viewWidth / 4; x <= viewWidth / 2; x += Hrad)
		{
			float jitter = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			SPH_Particle* particle = new SPH_Particle();
			particles.push_back(particle);
			particles.back()->init(glm::vec2(x + jitter, y), glm::vec2(0.f, 0.f));
		}
	}
	cout << "Total particles: " << particles.size() << endl;
}

void DFSPH_FluidGrid::loadNeighborhoods()
{
	neighborhoods.clear();
	for (int i = 0; i < particles.size(); ++i)
	{
		// store particle index in respective grid cell
		glm::vec2 pos = particles[i]->pos() / Hrad;
		int x = int(pos.x);
		int y = int(pos.y);
		int posIdx = y * xCellsCount + x;
		if (!neighborhoods.count(posIdx))
			neighborhoods[posIdx] = vector<int>();
		neighborhoods[posIdx].push_back(i);
	}
}

// we simply get all the particles in the current + adjacent cells, particles >Hrad away
// won't affect calculations thanks to the kernel
void DFSPH_FluidGrid::getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx)
{
	neighbors.clear();
	glm::vec2 pos = particles[currparticleIdx]->pos() / Hrad;
	int xidx = int(pos.x);
	int yidx = int(pos.y);
	// we check all adj. + curr grids
	for (int y = yidx - 1; y <= yidx + 1; ++y)
	{
		for (int x = xidx - 1; x <= xidx + 1; ++x)
		{
			int idx = y * xCellsCount + x;
			if (neighborhoods.count(idx))
				neighbors.insert(neighbors.end(), neighborhoods[idx].begin(), neighborhoods[idx].end());
		}
	}
}

void DFSPH_FluidGrid::computeDensitiesAndFactors(float t)
{
	vector<int> neighbors;
	// per particle
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* p_i = particles[i];
		SPH_Particle* p_j;
		getNeighborsInclusive(neighbors, i);
		// compute rho_i
		float rho_i = 0.f;
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos() - p_i->pos());
			if (r < Hrad)
			{
				// rho_i = sum_j(mass_j * W_ij)
				rho_i += MASS * POLY6 * pow(Hrad2 - pow(r, 2), 3.f);
			}
		}
		// compute a_i
		float a_i = 0.f;
		float term_1 = 0.f, term_2 = 0.f;
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos() - p_i->pos());
			if (r < Hrad)
			{
				float m_jW_j = MASS * POLY6_GRAD * pow(H - r, 3.f);
				term_1 += m_jW_j;
				term_2 += pow(m_jW_j, 2);
			}
		}
		// a_i = rho_i / [ |sum_j(m_j * gradW_ij)|^2 + sum_j(|m_j * gradW_ij|^2) ]
		a_i = max(rho_i / term_1 + term_2, pow(10.f, -6.f));
		// set values
		p_i->rho(rho_i);
		p_i->a(a_i);

	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

void DFSPH_FluidGrid::computeNonPressureForces(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* pi = particles[i];
		glm::vec2 fvisc(0.f, 0.f);
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			SPH_Particle* pj = particles[neighbors[j]];
			float r = glm::length(pj->pos() - pi->pos());
			// compute viscosity force contributions
			if (r < Hrad)
				fvisc += VISC * MASS * (pj->v() - pi->v()) / pj->rho() * VISC_LAP * (Hrad - r);
		}
		glm::vec2 fgrav = glm::vec2(0.f, G) * MASS / pi->rho();
		pi->f(fvisc + fgrav);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

void DFSPH_FluidGrid::Update(float deltaTime)
{
	float t = deltaTime * 0.02f;
	loadNeighborhoods();
	computeDensitiesAndFactors(t);
	computeNonPressureForces(t);
}

void DFSPH_FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->draw(mvMat, mvpHandle, particleMesh);

	glm::mat4 mvpMat = mvMat * glm::scale(glm::mat4(1.f), glm::vec3(Hrad, Hrad, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
