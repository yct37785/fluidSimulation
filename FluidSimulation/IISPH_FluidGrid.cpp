#include "IISPH_FluidGrid.h"

IISPH_FluidGrid::IISPH_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("sph_blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	viewWidth = (float)xCellsCount * Hrad;
	viewHeight = (float)yCellsCount * Hrad;
}

IISPH_FluidGrid::~IISPH_FluidGrid()
{
	for (int i = 0; i < particles.size(); ++i)
		delete particles[i];
}

void IISPH_FluidGrid::updateValues()
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->prev = particles[i]->curr;
}

float IISPH_FluidGrid::W(float r)
{
	return POLY6 * pow(Hrad2 - pow(r, 2), 3.f);
}

float IISPH_FluidGrid::gradW(float r)
{
	return POLY6_GRAD * pow(H - r, 3.f);
}

void IISPH_FluidGrid::spawnParticles()
{
	for (float y = EPS; y < viewHeight - EPS * 2.f; y += Hrad)
	{
		for (float x = viewWidth / 4; x <= viewWidth / 2; x += Hrad)
		{
			float jitter = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			IISPH_Particle* particle = new IISPH_Particle();
			particles.push_back(particle);
			particles.back()->init(glm::vec2(x + jitter, y), glm::vec2(0.f, 0.f));
		}
	}
	cout << "Total particles: " << particles.size() << endl;
}

void IISPH_FluidGrid::loadNeighborhoods()
{
	neighborhoods.clear();
	for (int i = 0; i < particles.size(); ++i)
	{
		// store particle index in respective grid cell
		glm::vec2 pos = particles[i]->prev.pos / Hrad;
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
void IISPH_FluidGrid::getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx)
{
	neighbors.clear();
	glm::vec2 pos = particles[currparticleIdx]->prev.pos / Hrad;
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

/**************************************************************************************
* Predict advection
**************************************************************************************/
void IISPH_FluidGrid::part_1(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p_i = particles[i];
		IISPH_Particle* p_j;
		float rho_i = 0.f;
		float d_ii = 0.f;
		glm::vec2 fvisc(0.f, 0.f);
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute rho(i)
				rho_i += MASS * W(r);
				// compute viscosity force contributions
				fvisc += VISC * MASS * (p_j->prev.v - p_i->prev.v) / p_j->prev.rho * VISC_LAP * (Hrad - r);
				// compute d(ii)
				d_ii += -(MASS / pow(p_i->prev.rho, 2.f)) * gradW(r);
			}
		}
		// compute f_adv(i)
		glm::vec2 fgrav = glm::vec2(0.f, G) * MASS / p_i->prev.rho;
		glm::vec2 fadv = fvisc + fgrav;
		// predict v_adv(i)
		glm::vec2 v_adv_i = p_i->prev.v + t * (fadv / MASS);
		// d(ii)
		d_ii = pow(t, 2.f) * d_ii;
		// set values
		p_i->curr.p_prev = p_i->prev.p;
		p_i->curr.rho = rho_i;
		p_i->curr.v_adv = v_adv_i;
		p_i->curr.d_ii = d_ii;
	}
	updateValues();
}

void IISPH_FluidGrid::part_2(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p_i = particles[i];
		IISPH_Particle* p_j;
		float rho_adv_i = 0.f;
		float a_ii = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute rho_adv(i)
				// vij = vi - vj
				rho_adv_i += MASS * glm::length(p_i->prev.v_adv - p_j->prev.v_adv) * gradW(r);
				// compute d(ji)
				float d_ji = -pow(t, 2.f) * (MASS / p_j->prev.rho) * gradW(r);
				// compute a(ii)
				a_ii += MASS * (p_i->prev.d_ii - d_ji) * gradW(r);
			}
		}
		// rho_adv(i)
		rho_adv_i = p_i->prev.rho + t * rho_adv_i;
		// rho0(i)
		float p0_i = 0.5f * p_i->prev.p_prev;
		// set values
		p_i->curr.rho_adv = rho_adv_i;
		p_i->curr.a_ii = a_ii;
		p_i->curr.p0 = p0_i;
	}
	updateValues();
}

void IISPH_FluidGrid::PredictAdvection(float t)
{
	part_1(t);
	part_2(t);
}

void IISPH_FluidGrid::part_3(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p_i = particles[i];
		IISPH_Particle* p_j;
		float dij_pj = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute d(ij)_p(j)
				dij_pj += -(MASS / pow(p_j->prev.rho, 2.f)) * p_j->prev.p * gradW(r);
			}
		}
		// d(ij)_p(j)
		dij_pj = pow(t, 2.f) * dij_pj;
		// set values
		p_i->curr.dij_pj = dij_pj;
	}
	updateValues();
}

void IISPH_FluidGrid::part_4(float t)
{
	const float RELAX_FACTOR = 0.5f;
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p_i = particles[i];
		IISPH_Particle* p_j;

		// compute p(i) part 1
		float eq_a = (1.f - RELAX_FACTOR) * p_i->prev.p;
		float eq_b = REST_DENS - p_i->prev.rho_adv;
		float eq_c_sum = 0.f;

		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute d(ji)
				float d_ji = -pow(t, 2.f) * (MASS / p_j->prev.rho) * gradW(r);
				// compute p(i) part 2
				float eq_ci = p_j->prev.dij_pj - d_ji * p_i->prev.p;
				float eq_c = p_i->prev.dij_pj - p_j->prev.d_ii * p_j->prev.p - eq_ci;
				eq_c_sum += MASS * eq_c * gradW(r);
			}
		}
		// p(i)
		// set values
		p_i->curr.p = eq_a + (RELAX_FACTOR * 1.f / p_i->prev.a_ii) * (eq_b - eq_c_sum);
	}
	updateValues();
}

void IISPH_FluidGrid::PressureSolve(float t)
{
	avgRho = 0.f;
	float threshold = 0.1f;
	while (avgRho - REST_DENS > threshold)
	{
		part_3(t);
		part_4(t);
	}
}

void IISPH_FluidGrid::Update(float deltaTime)
{
	float t = deltaTime * 0.2f;
	PredictAdvection(t);
	PressureSolve(t);
}

void IISPH_FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->draw(mvMat, mvpHandle, particleMesh);

	glm::mat4 mvpMat = mvMat * glm::scale(glm::mat4(1.f), glm::vec3(Hrad, Hrad, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
