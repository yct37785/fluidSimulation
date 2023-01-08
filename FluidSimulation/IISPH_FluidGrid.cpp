#include "IISPH_FluidGrid.h"

IISPH_FluidGrid::IISPH_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("sph_blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	viewWidth = (float)xCellsCount * Hrad;
	viewHeight = (float)yCellsCount * Hrad;
	avgRho = 0.f;
}

IISPH_FluidGrid::~IISPH_FluidGrid()
{
	for (int i = 0; i < particles.size(); ++i)
		delete particles[i];
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
		glm::vec2 pos = particles[i]->pos / Hrad;
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
	glm::vec2 pos = particles[currparticleIdx]->pos / Hrad;
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
void IISPH_FluidGrid::computeDensity(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p_i = particles[i];
		IISPH_Particle* p_j;
		float rho_i = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos - p_i->pos);
			if (r < Hrad)
			{
				// compute rho(i)
				rho_i += MASS * W(r);
			}
		}
		// set values
		p_i->rho = p_i->rho = rho_i;
	}
}

void IISPH_FluidGrid::part_1(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p_i = particles[i];
		IISPH_Particle* p_j;
		float d_ii = 0.f;
		glm::vec2 fvisc(0.f, 0.f);
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos - p_i->pos);
			if (r < Hrad)
			{
				// compute viscosity force contributions
				if (i != neighbors[j])
					fvisc += VISC * MASS * (p_j->v - p_i->v) / p_j->rho * VISC_LAP * (Hrad - r);
				// compute d(ii)
				d_ii += -(MASS / pow(p_i->rho, 2.f)) * gradW(r);
			}
		}
		// compute f_adv(i)
		glm::vec2 fgrav = glm::vec2(0.f, G) * MASS / p_i->rho;
		glm::vec2 fadv = fvisc + fgrav;
		//cout << "fadv: " << fadv.x << ", " << fadv.y << endl;
		// predict v_adv(i)
		glm::vec2 v_adv_i = p_i->v + t * (fadv / MASS);
		//cout << "v_adv_i: " << v_adv_i.x << ", " << v_adv_i.y << endl;
		// d(ii)
		d_ii = pow(t, 2.f) * d_ii;
		//cout << "d_ii: " << d_ii << endl;
		// set values
		p_i->p_prev = p_i->p;
		p_i->v_adv = v_adv_i;
		p_i->d_ii = d_ii;
	}
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
			float r = glm::length(p_j->pos - p_i->pos);
			if (r < Hrad)
			{
				// compute rho_adv(i)
				// vij = vi - vj
				rho_adv_i += MASS * glm::length(p_i->v_adv - p_j->v_adv) * gradW(r);
				// compute d(ji)
				float d_ji = -pow(t, 2.f) * (MASS / p_j->rho) * gradW(r);
				// compute a(ii)
				a_ii += MASS * (p_i->d_ii - d_ji) * gradW(r);
			}
		}
		// rho_adv(i)
		rho_adv_i = p_i->rho + t * rho_adv_i;
		//cout << "rho_adv_i: " << rho_adv_i << endl;
		// rho0(i)
		float p0_i = 0.5f * p_i->p_prev;
		//cout << "p0_i: " << p0_i << endl;
		//cout << "a_ii: " << a_ii << endl;
		// set values
		p_i->rho_adv = rho_adv_i;
		p_i->a_ii = a_ii;
		p_i->p0 = p0_i;
	}
}

void IISPH_FluidGrid::PredictAdvection(float t)
{
	computeDensity(t);
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
			float r = glm::length(p_j->pos - p_i->pos);
			if (r < Hrad)
			{
				// compute d(ij)_p(j)
				dij_pj += -(MASS / pow(p_j->rho, 2.f)) * p_j->p * gradW(r);
			}
		}
		// d(ij)_p(j)
		dij_pj = pow(t, 2.f) * dij_pj;
		//cout << "dij_pj: " << dij_pj << endl;
		// set values
		p_i->dij_pj = dij_pj;
	}
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
		float eq_a = (1.f - RELAX_FACTOR) * p_i->p;
		float eq_b = REST_DENS - p_i->rho_adv;
		float eq_c_sum = 0.f;

		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos - p_i->pos);
			if (r < Hrad)
			{
				// compute d(ji)
				float d_ji = -pow(t, 2.f) * (MASS / p_j->rho) * gradW(r);
				// compute p(i) part 2
				float eq_ci = p_j->dij_pj - d_ji * p_i->p;
				float eq_c = p_i->dij_pj - p_j->d_ii * p_j->p - eq_ci;
				eq_c_sum += MASS * eq_c * gradW(r);
			}
		}
		// p(i)
		// set values
		p_i->p = eq_a + (RELAX_FACTOR * (1.f / p_i->a_ii)) * (eq_b - eq_c_sum);
		if (isinf(p_i->p)) p_i->p = 0.f;
		//cout << "p_i: " << p_i->p << endl;
	}
}

void IISPH_FluidGrid::PressureSolve(float t)
{
	float threshold = 0.1f;
	//while (abs(avgRho - REST_DENS) > threshold)
	for (int i = 0; i < 20; ++i)
	{
		part_3(t);
		part_4(t);
	}
}

void IISPH_FluidGrid::Integration(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p_i = particles[i];
		IISPH_Particle* p_j;
		float fpress = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->pos - p_i->pos);
			if (r < Hrad)
			{
				// compute fpress(i)
				fpress += MASS * ((p_i->p / pow(p_i->rho, 2.f)) + (p_j->p / pow(p_j->rho, 2.f))) * gradW(r);
			}
		}
		// fpress(i)
		fpress = -MASS * fpress;
		// set values
		glm::vec2 v_i = p_i->v_adv + t * (fpress / MASS);
		p_i->pos = p_i->pos + t * v_i;
		p_i->v = v_i;
	}
	// bounds
	// boundary handling
	for (int i = 0; i < particles.size(); ++i)
	{
		IISPH_Particle* p = particles[i];
		glm::vec2 v = p->v;
		glm::vec2 pos = p->pos;
		if (pos.x - EPS < 0.f)
		{
			v.x *= BOUND_DAMPING;
			pos.x = EPS;
		}
		if (pos.x + EPS > viewWidth)
		{
			v.x *= BOUND_DAMPING;
			pos.x = viewWidth - EPS;
		}
		if (pos.y - EPS < 0.f)
		{
			v.y *= BOUND_DAMPING;
			pos.y = EPS;
		}
		if (pos.y + EPS > viewHeight)
		{
			v.y *= BOUND_DAMPING;
			pos.y = viewHeight - EPS;
		}
		p->v = v;
		p->pos = pos;
	}
}

void IISPH_FluidGrid::Update(float deltaTime)
{
	float t = deltaTime * 0.2f;
	loadNeighborhoods();
	PredictAdvection(t);
	PressureSolve(t);
	Integration(t);
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
