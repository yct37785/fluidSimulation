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
				float m_jgradW_j = MASS * POLY6_GRAD * pow(H - r, 3.f);
				term_1 += m_jgradW_j;
				term_2 += pow(m_jgradW_j, 2);
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
		SPH_Particle* p_i = particles[i];
		glm::vec2 fvisc(0.f, 0.f);
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			SPH_Particle* p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos() - p_i->pos());
			// compute viscosity force contributions
			if (r < Hrad)
				fvisc += VISC * MASS * (p_j->v() - p_i->v()) / p_j->rho() * VISC_LAP * (Hrad - r);
		}
		// compute external force contributions
		glm::vec2 fgrav = glm::vec2(0.f, G) * MASS / p_i->rho();
		p_i->f(fvisc + fgrav);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

void DFSPH_FluidGrid::predictVelocities(float t)
{
	for (int i = 0; i < particles.size(); ++i)
	{
		// v*i = v_i + t * fadv_i / m_i
		glm::vec2 v_i = particles[i]->v();
		particles[i]->v(v_i + t * particles[i]->f() / MASS);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

void DFSPH_FluidGrid::correctDensityError(float t)
{
	vector<int> neighbors;
	// find mat derivative of density
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* p_i = particles[i];
		// Drho_i / Dt = sum_j(m_j(v_i - v_j) * gradW_ij)
		float rho_i_matDiv = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			SPH_Particle* p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos() - p_i->pos());
			if (r < Hrad)
				rho_i_matDiv += MASS * glm::length(p_i->v() - p_j->v()) * POLY6_GRAD * pow(H - r, 3.f);
		}
		p_i->rho_matDiv(rho_i_matDiv);
	}
	float avgDiv = 1000.f;
	float thres = 0.0001f;
	// while avg(Drho / Dt) > threshold nV
	while (avgDiv > thres)
	{
		// for all particles
		for (int i = 0; i < particles.size(); ++i)
		{
			//// stiffness params
			//float kv_i = 1 / t * rho_i_matDiv * p_i->a();
			//float kv_j = 1 / t * rho_i_matDiv * p_i->a();
		}
	}
}

void DFSPH_FluidGrid::Update(float deltaTime)
{
	float t = deltaTime * 0.02f;
	loadNeighborhoods();
	computeDensitiesAndFactors(t);
	computeNonPressureForces(t);
	// todo: timestep CFL
	predictVelocities(t);
	correctDensityError(t);
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
