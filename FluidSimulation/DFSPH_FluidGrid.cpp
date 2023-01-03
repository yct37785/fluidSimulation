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

void DFSPH_FluidGrid::spatialPartitioning()
{
	grids.clear();
	for (int i = 0; i < particles.size(); ++i)
	{
		glm::vec2 pos = particles[i]->pos() / Hrad;
		int x = int(pos.x);
		int y = int(pos.y);
		int idx = y * xCellsCount + x;
		if (!grids.count(idx))
			grids[idx] = vector<int>();
		grids[idx].push_back(i);
	}
}

void DFSPH_FluidGrid::getNeighborsInclusive(vector<int>& neighbors, int curr)
{
	neighbors.clear();
	glm::vec2 pos = particles[curr]->pos() / Hrad;
	int xidx = int(pos.x);
	int yidx = int(pos.y);
	// we check all adj. + curr grids
	for (int y = yidx - 1; y <= yidx + 1; ++y)
	{
		for (int x = xidx - 1; x <= xidx + 1; ++x)
		{
			int idx = y * xCellsCount + x;
			if (grids.count(idx))
				neighbors.insert(neighbors.end(), grids[idx].begin(), grids[idx].end());
		}
	}
}

void DFSPH_FluidGrid::findDensityPressure()
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* pi = particles[i];
		float rho = 0.f;
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			SPH_Particle* pj = particles[neighbors[j]];
			glm::vec2 rij = pj->pos() - pi->pos();
			float r = glm::length(rij);
			if (r < Hrad)
			{
				// rhoi = sum(mj * Wij)
				rho += MASS * POLY6 * pow(Hrad2 - pow(r, 2), 3.f);
			}
		}
		pi->rho(rho);
		// Pi = K(pi - p0)
		pi->p(GAS_CONST * (rho - REST_DENS));
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

void DFSPH_FluidGrid::computeForces()
{
	vector<int> neighbors;
	for (int i = 0; i < particles.size(); ++i)
	{
		SPH_Particle* pi = particles[i];
		glm::vec2 fpress(0.f, 0.f);
		glm::vec2 fvisc(0.f, 0.f);
		getNeighborsInclusive(neighbors, i);
		for (int j = 0; j < neighbors.size(); ++j)
		{
			SPH_Particle* pj = particles[neighbors[j]];
			if (i == neighbors[j])
				continue;
			glm::vec2 rij = pj->pos() - pi->pos();
			float r = glm::length(rij);
			if (r < Hrad)
			{
				// pressure force contributions
				fpress += -glm::normalize(rij) * MASS * (pi->p() + pj->p()) / (2.f * pj->rho()) * SPIKY_GRAD * pow(Hrad - r, 3.f);
				// compute viscosity force contributions
				fvisc += VISC * MASS * (pj->v() - pi->v()) / pj->rho() * VISC_LAP * (Hrad - r);
			}
		}
		glm::vec2 fgrav = glm::vec2(0.f, G) * MASS / pi->rho();
		pi->f(fpress + fvisc + fgrav);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i]->postUpdate();
}

void DFSPH_FluidGrid::integrate(float t)
{
	for (int i = 0; i < particles.size(); ++i)
	{
		// forward Euler integration
		SPH_Particle* p = particles[i];
		p->forwardEuler(t);
		p->postUpdate();

		// enforce boundary conditions
		glm::vec2 v = p->v();
		glm::vec2 pos = p->pos();
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
		p->v(v);
		p->pos(pos);
		p->postUpdate();
	}
}

void DFSPH_FluidGrid::Update(float deltaTime)
{
	float t = deltaTime * 0.02f;
	spatialPartitioning();
	findDensityPressure();
	computeForces();
	integrate(t);
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
