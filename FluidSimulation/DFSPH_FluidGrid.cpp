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

void DFSPH_FluidGrid::computeDensitiesAndFactors(float t, bool factor)
{
	avgRho = 0.f;
	vector<int> neighbors;
	// per particle
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* p_i = particles[i];
		SPH_Particle* p_j;
		getNeighborsInclusive(neighbors, i);
		// compute rho_i (inclusive of i)
		float rho_i = 0.f;
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos() - p_i->pos());
			if (r < Hrad)
			{
				// EQ: rho_i = sum_j(mass_j * W_ij)
				rho_i += MASS * POLY6 * pow(Hrad2 - pow(r, 2), 3.f);
			}
		}
		// compute a_i
		if (factor)
		{
			float a_i = 0.f;
			float term_1 = 0.f, term_2 = 0.f;
			for (int j = 0; j < neighbors.size(); ++j)
			{
				p_j = particles[neighbors[j]];
				float r = glm::length(p_j->pos() - p_i->pos());
				if (r < Hrad)
				{
					// EQ: a_i = rho_i / [ |sum_j(m_j * gradW_ij)|^2 + sum_j(|m_j * gradW_ij|^2) ]
					float m_jgradW_j = MASS * POLY6_GRAD * pow(H - r, 3.f);
					term_1 += m_jgradW_j;
					term_2 += pow(m_jgradW_j, 2.f);
				}
			}
			a_i = max(rho_i / (pow(term_1, 2.f) + term_2), pow(10.f, -6.f));
			p_i->a(a_i);
		}
		// set values
		p_i->rho(rho_i);
		avgRho += rho_i;
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
	avgRho /= (float)particles.size();
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
		// EQ: v*i = v_i + t * fadv_i / m_i
		glm::vec2 v_i = particles[i]->v();
		particles[i]->v(v_i + t * particles[i]->f() / MASS);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

void DFSPH_FluidGrid::computeUpdatedDensities(float t)
{
	avgRho = 0.f;
	vector<int> neighbors;
	// per particle
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* p_i = particles[i];
		SPH_Particle* p_j;
		getNeighborsInclusive(neighbors, i);
		// compute rho*i (inclusive of i)
		float rhoMat = 0.f;
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos() - p_i->pos());
			if (r < Hrad)
			{
				// EQ: rho*i = sum_j(mass_j * (v_i - v_j) * gradW_ij)
				float velLen = glm::length(p_i->v() - p_j->v());
				if (isinf(velLen))
					velLen = 0.f;
				rhoMat += MASS * velLen * POLY6_GRAD * pow(H - r, 3.f);
			}
		}
		// set values
		float rho_i = p_i->rho() + t * rhoMat;
		p_i->rho(rho_i);
		avgRho += rho_i;
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
	avgRho /= (float)particles.size();
}

void DFSPH_FluidGrid::CorrectDensityError(float t)
{
	vector<int> neighbors;
	float thres = 0.01f;
	// while avg(Drho / Dt) > threshold nV
	int counter = 0;
	//while (abs(avgRho - REST_DENS) > thres)
	while(counter < 1)
	{
		// for all particles
		for (int i = 0; i < particles.size(); ++i)
		{
			SPH_Particle* p_i = particles[i];
			// v*i = v_i - t * sum_j(m_j * ( kv_i/rho_i + kv_j/rho_j ) * gradW_ij)
			float acceleration = 0.f;
			getNeighborsInclusive(neighbors, i);
			for (int j = 0; j < neighbors.size(); ++j)
			{
				SPH_Particle* p_j = particles[neighbors[j]];
				float r = glm::length(p_j->pos() - p_i->pos());
				// stiffness params
				float kv_i = (p_i->rho() - REST_DENS) / pow(t, 2) * p_i->a();
				float kv_j = (p_j->rho() - REST_DENS) / pow(t, 2) * p_j->a();
				// acc
				acceleration += MASS * ((kv_i / p_i->rho()) + (kv_j / p_j->rho())) * POLY6_GRAD * pow(H - r, 3.f);
			}
			p_i->v(p_i->v() - t * acceleration);
		}
		for (int i = 0; i < particles.size(); ++i)
			particles[i]->postUpdate();
		computeUpdatedDensities(t);
		counter++;
	}
}

void DFSPH_FluidGrid::forwardParticles(float t)
{
	for (int i = 0; i < particles.size(); ++i)
	{
		glm::vec2 x_i = particles[i]->pos();
		particles[i]->pos(x_i + t * particles[i]->v());
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

float DFSPH_FluidGrid::computeDensityMat()
{
	float sum = 0.f;
	vector<int> neighbors;
	// find mat derivative of density
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* p_i = particles[i];
		// Drho_i / Dt = sum_j(m_j(v_i - v_j) * gradW_ij)
		float rho_i_mat = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			SPH_Particle* p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos() - p_i->pos());
			if (r < Hrad)
				rho_i_mat += MASS * glm::length(p_i->v() - p_j->v()) * POLY6_GRAD * pow(H - r, 3.f);
		}
		p_i->rho_mat(rho_i_mat);
		sum += rho_i_mat;
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
	return sum / (float)particles.size();
}

void DFSPH_FluidGrid::correctDivergenceError(float t)
{
	vector<int> neighbors;
	float avgMat = computeDensityMat();
	float thres = 0.0001f;
	// while avg(Drho / Dt) > threshold nV
	while (avgMat > thres)
	{
		// for all particles
		for (int i = 0; i < particles.size(); ++i)
		{
			SPH_Particle* p_i = particles[i];
			// v*i = v_i - t * sum_j(m_j * ( kv_i/rho_i + kv_j/rho_j ) * gradW_ij)
			float acceleration = 0.f;
			getNeighborsInclusive(neighbors, i);
			for (int j = 0; j < neighbors.size(); ++j)
			{
				SPH_Particle* p_j = particles[neighbors[j]];
				float r = glm::length(p_j->pos() - p_i->pos());
				// stiffness params
				float kv_i = 1 / t * p_i->rho_mat() * p_i->a();
				float kv_j = 1 / t * p_j->rho_mat() * p_j->a();
				// acc
				acceleration += MASS * ((kv_i / p_i->rho()) + (kv_j / p_j->rho())) * POLY6_GRAD * pow(H - r, 3.f);
			}
			p_i->v(p_i->v() - t * acceleration);
		}
		for (int i = 0; i < particles.size(); ++i)
			particles[i]->postUpdate();
		avgMat = computeDensityMat();
	}
}

void DFSPH_FluidGrid::ComputeParticleValues(float t)
{
	loadNeighborhoods();
	computeDensitiesAndFactors(t, true);
}

void DFSPH_FluidGrid::VelocityUpdate(float t)
{
	computeNonPressureForces(t);
	predictVelocities(t);
}

void DFSPH_FluidGrid::Update(float deltaTime)
{
	float t = deltaTime * 0.2f;
	ComputeParticleValues(t);
	VelocityUpdate(t);
	//forwardParticles(t);
	// todo: timestep CFL
	CorrectDensityError(t);
	forwardParticles(t);
	//cout << "asdasd" << endl;
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
