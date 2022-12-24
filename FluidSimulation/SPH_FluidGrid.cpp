#include "SPH_FluidGrid.h"

SPH_FluidGrid::SPH_FluidGrid(int xCellsCount, int yCellsCount)
{
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	particleMesh = MeshBuilder::CreateMesh("sph_blue_marker");

	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;

	float mass = 10.f;
	float xmin = 0.3f;
	float xmax = 0.7f;
	float ymin = 0.3f;
	float ymax = 0.7f;
	float space = 0.6f;
	for (float y = 0.f + (float)yCellsCount * Hrad * ymin; y < (float)yCellsCount * Hrad * ymax; y += space * Hrad)
	{
		for (float x = 0.f + (float)xCellsCount * Hrad * xmin; x < (float)xCellsCount * Hrad * xmax; x += space * Hrad)
		{
			particles.push_back(SPH_Particle());
			particles.back().init(mass, glm::vec2(x, y), glm::vec2(0.f, 0.f));
		}
	}
	cout << "Total particles: " << particles.size() << endl;

	W_Scaler = 315.f / (64.f * PI * pow(Hrad, 9));
	dW_Scaler = -(45.f / (PI * pow(Hrad, 6)));
}

SPH_FluidGrid::~SPH_FluidGrid()
{
}

// smoothing kernel, returns 0 -> 1
// param r: dist btw particle i and j
float SPH_FluidGrid::cal_W(float r)
{
	float distVar = Hrad * Hrad - r * r;
	distVar = pow(distVar, 3);
	return W_Scaler * distVar;
}

// gradient of smoothing kernel
// param r: dist btw particle i and j
float SPH_FluidGrid::cal_dW(float r)
{
	float distVar = pow(Hrad - r, 2);
	return dW_Scaler * distVar;
}

void SPH_FluidGrid::getMaxValues()
{
	maxVel = maxA = maxC = 0.f;
	for (int i = 0; i < particles.size(); ++i)
	{
		float velMag = glm::length(particles[i].getVel());
		float accMag = particles[i].getAcceleration();
		float soundSpeed = sqrt((1.f * particles[i].getPressure()) / particles[i].getDensity());
		soundSpeed = isinf(soundSpeed) ? 0.f : soundSpeed;
		if (velMag > maxVel)
			maxVel = velMag;
		if (accMag > maxA)
			maxA = accMag;
		if (soundSpeed > maxC)
			maxC = soundSpeed;
	}
	//cout << maxVel << " " << maxA << " " << maxC << endl;
}

float SPH_FluidGrid::getTimestamp()
{
	getMaxValues();
	float C = 1.f;
	float Ch = C * Hrad;
	float maxVel_t = Ch / max(maxVel, 0.0000001f);
	float maxA_t = sqrt(Hrad / max(maxA, 0.0000001f));
	float maxC_t = Ch / max(maxC, 0.0000001f);
	return max(max(maxVel_t, maxA_t), maxC_t);
}

void SPH_FluidGrid::findNeighbors()
{
	for (int i = 0; i < particles.size(); ++i)
	{
		particles[i].neighbors.clear();
		particles[i].neighborDist.clear();
		for (int j = 0; j < particles.size(); ++j)
		{
			if (i == j)
				continue;
			// ! change to square pls
			float r = glm::length(particles[j].getPos() - particles[i].getPos());
			// only consider neighbors within < 2 * Hrad dist
			if (r < Hrad * 2.f)
			{
				particles[i].neighbors.push_back(j);
				particles[i].neighborDist.push_back(r);
			}
		}
	}
}

void SPH_FluidGrid::findDensity()
{
	for (int i = 0; i < particles.size(); ++i)
	{
		// pi = sum(mj * Wij)
		float density = 0.f;
		for (int j = 0; j < particles[i].neighbors.size(); ++j)
		{
			int nIdx = particles[i].neighbors[j];
			density += particles[nIdx].getMass() * cal_W(particles[i].neighborDist[j]);
		}
		particles[i].setDensity(density);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i].postUpdate();
}

void SPH_FluidGrid::findPressure()
{
	// Pi = K(pi - p0)
	for (int i = 0; i < particles.size(); ++i)
	{
		float pressure = max(P0, K * (particles[i].getDensity() - P0));
		particles[i].setPressure(pressure);
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i].postUpdate();
}

void SPH_FluidGrid::applyAcceleration()
{
	for (int i = 0; i < particles.size(); ++i)
	{
		// a = -sum(mj/mi * (Pi + Pj) / (2 * pi * pj) * dWij * r^^ij
		glm::vec2 acc(0.f);
		for (int j = 0; j < particles[i].neighbors.size(); ++j)
		{
			int nIdx = particles[i].neighbors[j];
			float m_Var = particles[nIdx].getMass() / particles[i].getMass();
			float P_Div = (particles[i].getPressure() + particles[nIdx].getPressure()) /
				(2.f * particles[i].getDensity() * particles[nIdx].getDensity());
			float dW = cal_dW(particles[i].neighborDist[j]);
			glm::vec2 rij = (particles[nIdx].getPos() - particles[i].getPos()) / particles[i].neighborDist[j];
			acc += m_Var * P_Div * dW * rij;
		}
		acc = -acc;
		particles[i].accelerateVel(acc);
		particles[i].applyGravity();
	}
	for (int i = 0; i < particles.size(); ++i)
		particles[i].postUpdate();
}

void SPH_FluidGrid::Update(float deltaTime)
{
	float t = getTimestamp() * deltaTime * 0.0000001f;
	findNeighbors();
	findDensity();
	findPressure();
	applyAcceleration();
	// update particles by vel
	for (int i = 0; i < particles.size(); ++i)
		particles[i].updatePosByVel(t);
	for (int i = 0; i < particles.size(); ++i)
		particles[i].postUpdate();
}

void SPH_FluidGrid::Draw(int mvpHandle, glm::mat4& mvMat)
{
	glm::mat4 mvpMat;
	for (int i = 0; i < particles.size(); ++i)
	{
		mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(particles[i].getPos(), 0.f));
		glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
		glBindVertexArray(particleMesh->getVAO());
		glDrawElements(GL_TRIANGLES, particleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
	}

	mvpMat = mvMat * glm::scale(glm::mat4(1.f), glm::vec3(Hrad, Hrad, 0.f));
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}
