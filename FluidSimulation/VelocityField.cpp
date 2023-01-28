#include "VelocityField.h"

VelocityField::VelocityField()
{
	uMarker = MeshBuilder::CreateMesh("purple_marker");
	vMarker = MeshBuilder::CreateMesh("yellow_marker");
	// vel comps: (0.0 -> 1.0, 1.0 -> 2.0 ...)
	// face centers: + 0.5
	u = new QuantityField(p_GridInfo->xCells + 1, p_GridInfo->yCells, 0.f, Hoffset, uMarker);
	v = new QuantityField(p_GridInfo->xCells, p_GridInfo->yCells + 1, Hoffset, 0.f, vMarker);
}

VelocityField::~VelocityField()
{
	delete u;
	delete v;
	delete uMarker;
	delete vMarker;
}

void VelocityField::advectSelf_semiLagrangian(float t, std::unordered_map<int, int>& liquidCells)
{
	// get vel at pos(x',y'), trace backwards, get vel at that pos, then apply to (x',y')
	for (int y = 0; y < p_GridInfo->yCells + 1; ++y)
	{
		for (int x = 0; x < p_GridInfo->xCells + 1; ++x)
		{
			// x
			if (y < p_GridInfo->yCells)
			{
				glm::vec2 xCompPos = glm::vec2(x, y + Hoffset);
				glm::vec2 vel(u->getQuantityAtIdx(x, y), v->getQuantityAtPos(xCompPos));
				glm::vec2 prevPos = xCompPos - vel * t;
				u->setQuantityAtIdx(x, y, u->getQuantityAtPos(prevPos));
			}
			// no slip
			if (x == 0 || x == p_GridInfo->xCells)
				u->setQuantityAtIdx(x, y, 0.f);
			// y
			if (x < p_GridInfo->xCells)
			{
				glm::vec2 yCompPos = glm::vec2(x + Hoffset, y);
				glm::vec2 vel(u->getQuantityAtPos(yCompPos), v->getQuantityAtIdx(x, y));
				glm::vec2 prevPos = yCompPos - vel * t;
				v->setQuantityAtIdx(x, y, v->getQuantityAtPos(prevPos));
			}
			// no slip
			if (y == 0 || y == p_GridInfo->yCells)
				v->setQuantityAtIdx(x, y, 0.f);
		}
	}
	u->postUpdate();
	v->postUpdate();
}

void VelocityField::applyExternalForces(float t, std::unordered_map<int, int>& liquidCells)
{
	float maxGravityAcc = 9.81f;
	float gravScale = 0.1f;
	// only updated for vel comps bordering fluid
	for (int y = 0; y < p_GridInfo->yCells + 1; ++y)
	{
		for (int x = 0; x < p_GridInfo->xCells; ++x)
		{
			if (liquidCells.count((y - 1) * p_GridInfo->xCells + x) || liquidCells.count(y * p_GridInfo->xCells + x))
			{
				float yVel = v->getQuantityAtIdx(x, y);
				v->setQuantityAtIdx(x, y, std::max(-maxGravityAcc, yVel - 0.981f * t));
			}
			// no slip
			if (y == 0)
				v->setQuantityAtIdx(x, y, 0.f);
		}
	}
	v->postUpdate();
}

void VelocityField::extrapolate(std::unordered_map<int, int>& liquidCells)
{
	for (int y = 0; y < p_GridInfo->yCells; ++y)
	{
		for (int x = 0; x < p_GridInfo->xCells; ++x)
		{
			// for vel comps not bordering fluid cells
			// set to average of neighboring cells that are fluid
			// x
			if (x > 0)
			{
				if (!liquidCells.count(y * p_GridInfo->xCells + x - 1) && !liquidCells.count(y * p_GridInfo->xCells + x))
					u->setQuantityAtIdx(x, y, u->getNeighboringAvg(x, y));
			}
			// y
			if (y > 0)
			{
				if (!liquidCells.count((y - 1) * p_GridInfo->xCells + x) && !liquidCells.count(y * p_GridInfo->xCells + x))
					v->setQuantityAtIdx(x, y, v->getNeighboringAvg(x, y));
			}
		}
	}
	u->postUpdate();
	v->postUpdate();
}

float VelocityField::getDerivative(int x1, int y1, int x2, int y2, char comp)
{
	float v2 = 0.f, v1 = 0.f;
	if (comp == 'x')
	{
		v1 = u->getQuantityAtIdx(x1, y1);
		v2 = u->getQuantityAtIdx(x2, y2);
	}
	else if (comp == 'y')
	{
		v1 = v->getQuantityAtIdx(x1, y1);
		v2 = v->getQuantityAtIdx(x2, y2);
	}
	return (v2 - v1) / H;
}

glm::vec2 VelocityField::getVelAtPos(glm::vec2 pos)
{
	return glm::vec2(u->getQuantityAtPos(pos), v->getQuantityAtPos(pos));
}

void VelocityField::addToCompByIdx(int x, int y, char comp, float add)
{
	if (comp == 'x')
		u->setQuantityAtIdx(x, y, u->getQuantityAtIdx(x, y) + add);
	else if (comp == 'y')
		v->setQuantityAtIdx(x, y, v->getQuantityAtIdx(x, y) + add);
}

void VelocityField::postUpdate()
{
	u->postUpdate();
	v->postUpdate();
}

void VelocityField::draw(glm::mat4& mvMat, int mvpHandle)
{
	// u->draw(mvMat, mvpHandle);
	//v->draw(mvMat, mvpHandle);
}
