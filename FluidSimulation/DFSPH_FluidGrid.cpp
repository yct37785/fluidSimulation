#include "DFSPH_FluidGrid.h"
const float REST_DEN2 = 300.f;

DFSPH_FluidGrid::DFSPH_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("sph_blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	viewWidth = (float)xCellsCount * Hrad;
	viewHeight = (float)yCellsCount * Hrad;
	first = false;
	avgRho = avgRhoDivergence = 0.f;
}

DFSPH_FluidGrid::~DFSPH_FluidGrid()
{
	for (int i = 0; i < particles.size(); ++i)
		delete particles[i];
}

void DFSPH_FluidGrid::updateValues()
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->prev = particles[i]->curr;
}

float DFSPH_FluidGrid::W(float r)
{
	return POLY6 * pow(Hrad2 - pow(r, 2), 3.f);
}

float DFSPH_FluidGrid::gradW(float r)
{
	return POLY6_GRAD * pow(H - r, 3.f);
}

void DFSPH_FluidGrid::spawnParticles()
{
	for (float y = EPS; y < viewHeight - EPS * 2.f; y += Hrad)
	{
		for (float x = viewWidth / 4; x <= viewWidth / 2; x += Hrad)
		{
			float jitter = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
			DFSPH_Particle* particle = new DFSPH_Particle();
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
void DFSPH_FluidGrid::getNeighborsInclusive(vector<int>& neighbors, int currparticleIdx)
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

float DFSPH_FluidGrid::getTimeStep(float t)
{
	float maxVel = 0.f;
	for (int i = 0; i < particles.size(); ++i)
	{
		float velLen = glm::length(particles[i]->prev.v);
		if (velLen > maxVel)
			maxVel = velLen;
	}
	return min(t, Hrad / maxVel);
}

/**************************************************************************************
* Predict advection
**************************************************************************************/
void DFSPH_FluidGrid::computeDensitiesAndFactors(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		DFSPH_Particle* p_j;
		float rho_i = 0.f;
		float a_i_term_1 = 0.f, a_i_term_2 = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute rho(i)
				rho_i += MASS * W(r);
				// compute a(i)
				//if (i != neighbors[j])
				{
					a_i_term_1 += MASS * gradW(r);
					a_i_term_2 += pow(MASS * gradW(r), 2.f);
				}
			}
		}
		// set values
		p_i->prev.rho = p_i->curr.rho = rho_i;
		float a_i = max(rho_i / (pow(a_i_term_1, 2.f) + a_i_term_2), pow(10.f, -6.f));
		p_i->prev.a = p_i->curr.a = a_i;
		//cout << "rho_i: " << rho_i << endl;
		//cout << "a_i: " << a_i << endl;
	}
}

void DFSPH_FluidGrid::PredictAdvection(float t)
{
	loadNeighborhoods();
	computeDensitiesAndFactors(t);
}

/**************************************************************************************
* Predict velocities
**************************************************************************************/
void DFSPH_FluidGrid::computeNonPressureForces(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		DFSPH_Particle* p_j;
		glm::vec2 f_visc(0.f, 0.f);
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute viscosity force contributions
				f_visc += VISC * MASS * (p_j->prev.v - p_i->prev.v) / p_j->prev.rho * VISC_LAP * (Hrad - r);
			}
		}
		// compute f_adv(i)
		glm::vec2 f_grav = glm::vec2(0.f, G) * MASS / p_i->prev.rho;
		glm::vec2 f_adv = f_visc + f_grav;
		//cout << "fadv: " << f_adv.x << ", " << f_adv.y << endl;
		// set values
		p_i->prev.f_adv = p_i->curr.f_adv = f_adv;
	}
}

void DFSPH_FluidGrid::advanceVelocities(float t)
{
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		p_i->prev.v_adv = p_i->curr.v_adv = p_i->prev.v + t * p_i->prev.f_adv / MASS;
		p_i->prev.v_adv_initial = p_i->curr.v_adv_initial = p_i->prev.v_adv;
		//cout << "v*: " << p_i->prev.v_adv.x << ", " << p_i->prev.v_adv.y << endl;
	}
}

void DFSPH_FluidGrid::PredictVelocities(float t)
{
	computeNonPressureForces(t);
	advanceVelocities(t);
}

/**************************************************************************************
* Correct density error
**************************************************************************************/
void DFSPH_FluidGrid::predictDensity(float t)
{
	avgRho = 0.f;
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		DFSPH_Particle* p_j;
		float rho_adv_i = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			//if (i == neighbors[j]) continue;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute rho(i)
				rho_adv_i += MASS * glm::dot((p_i->prev.v_adv - p_j->prev.v_adv), glm::vec2(gradW(r)));
			}
		}
		// rho(i)
		/*if (t * rho_adv_i > 0.f)
			cout << i << ": " << t * rho_adv_i << endl;*/
		rho_adv_i = p_i->prev.rho + t * rho_adv_i;
		/*if (i == 200)
			cout << "rho_i: " << rho_i << endl;*/
		avgRho += rho_adv_i;
		//cout << "rho_adv_i: " << rho_adv_i << endl;
		// update values
		p_i->prev.rho_adv = p_i->curr.rho_adv = rho_adv_i;
	}
	avgRho = avgRho / (float)particles.size();
	//cout << "avgRho: " << avgRho << endl;
}

// note that we must predict densities without shifting from the curr positions
void DFSPH_FluidGrid::adaptVelocities(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		DFSPH_Particle* p_j;
		float sum = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			//if (i == neighbors[j]) continue;
			// compute k(i)
			float k_i = ((p_i->prev.rho_adv - REST_DEN2) / pow(t, 2.f)) * p_i->prev.a;
			//cout << "p_i->prev.rho: " << p_i->prev.rho << endl;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute k(j)
				float k_j = ((p_j->prev.rho_adv - REST_DEN2) / pow(t, 2.f)) * p_j->prev.a;
				sum += MASS * (k_i / p_i->prev.rho + k_j / p_j->prev.rho) * gradW(r);
			}
		}
		// adapt velocities by advancing with predicted change
		p_i->prev.v_adv = p_i->curr.v_adv = p_i->prev.v_adv - t * sum;
		//cout << "v*: " << p_i->prev.v.x << ", " << p_i->prev.v.y << endl;
	}
}

void DFSPH_FluidGrid::CorrectDensityError(float t)
{
	float thres = REST_DEN2 * 0.01f;
	avgRho = 0.f;
	while (abs(avgRho - REST_DEN2) > thres)
	{
		predictDensity(t);
		adaptVelocities(t);
	}
	//cout << "Density err: " << abs(avgRho - REST_DEN2) << ", " << avgRho << endl;
}

/**************************************************************************************
* Update positions
**************************************************************************************/
void DFSPH_FluidGrid::UpdatePositions(float t)
{
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		p_i->prev.pos = p_i->curr.pos = p_i->prev.pos + t * p_i->prev.v_adv;
	}
}

/**************************************************************************************
* Correct divergence error
**************************************************************************************/
void DFSPH_FluidGrid::computeDensityDivergence(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		DFSPH_Particle* p_j;
		float sum = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute rho_mat(i)
				sum += MASS * glm::dot((p_i->prev.v_adv - p_j->prev.v_adv), glm::vec2(gradW(r)));
			}
		}
		// adapt velocities
		p_i->prev.rho_mat = p_i->curr.rho_mat = sum;
		avgRhoDivergence += sum;
		//cout << "rho_mat: " << sum << endl;
	}
	avgRhoDivergence /= (float)particles.size();
	//cout << "avgRhoDivergence: " << avgRhoDivergence << endl;
}

void DFSPH_FluidGrid::adaptVelocitiesDivSolver(float t)
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p_i = particles[i];
		DFSPH_Particle* p_j;
		float sum = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			if (i == neighbors[j]) continue;
			// compute k(i)
			float kv_i = (1.f / t) * p_i->prev.rho_mat * p_i->prev.a;
			//cout << "p_i->prev.rho: " << p_i->prev.rho << endl;
			p_j = particles[neighbors[j]];
			float r = glm::length(p_j->prev.pos - p_i->prev.pos);
			if (r < Hrad)
			{
				// compute k(j)
				float kv_j = (1.f / t) * p_j->prev.rho_mat * p_j->prev.a;
				sum += MASS * (kv_i / p_i->prev.rho + kv_j / p_j->prev.rho) * gradW(r);
			}
		}
		// adapt velocities by advancing with predicted change
		p_i->prev.v_adv = p_i->curr.v_adv = p_i->prev.v_adv - t * sum;
		//cout << "v*: " << p_i->prev.v.x << ", " << p_i->prev.v.y << endl;
	}
}

void DFSPH_FluidGrid::CorrectDivergenceError(float t)
{
	float thres = 0.1f;
	avgRhoDivergence = 10000.f;
	while (avgRhoDivergence > thres)
	{
		computeDensityDivergence(t);
		adaptVelocitiesDivSolver(t);
	}
}

void DFSPH_FluidGrid::IntegrateVelocityAndBounds()
{
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->prev.v = particles[i]->curr.v = particles[i]->prev.v_adv;
	// boundary handling
	for (int i = 0; i < particles.size(); ++i)
	{
		DFSPH_Particle* p = particles[i];
		glm::vec2 v = p->prev.v;
		glm::vec2 pos = p->prev.pos;
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
		p->prev.v = p->curr.v = v;
		p->prev.pos = p->curr.pos = pos;
	}
}

/**************************************************************************************
* Update
**************************************************************************************/
void DFSPH_FluidGrid::Update(float deltaTime)
{
	float t = deltaTime * 0.2f;
	if (!first && particles.size() > 0)
	{
		PredictAdvection(t);
		first = true;
	}
	PredictVelocities(t);
	CorrectDensityError(t);
	UpdatePositions(t);
	PredictAdvection(t);
	CorrectDivergenceError(t);
	IntegrateVelocityAndBounds();
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
