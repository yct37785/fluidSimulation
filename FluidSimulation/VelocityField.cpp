#include "VelocityField.h"

VelocityField::VelocityField(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	uMarker = MeshBuilder::CreateMesh("purple_marker");
	vMarker = MeshBuilder::CreateMesh("yellow_marker");
	// vel comps: (0.0 -> 1.0, 1.0 -> 2.0 ...)
	// face centers: + 0.5
	u = new QuantityField(xCellsCount + 1, yCellsCount, 0.f, Hoffset, uMarker);
	v = new QuantityField(xCellsCount, yCellsCount + 1, Hoffset, 0.f, vMarker);
}

VelocityField::~VelocityField()
{
	delete u;
	delete v;
	delete uMarker;
	delete vMarker;
}

void VelocityField::advectSelf_semiLagrangian(float t, unordered_map<int, int>& liquidCells)
{
	// get vel at pos(x',y'), trace backwards, get vel at that pos, then apply to (x',y')
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		for (int x = 0; x < xCellsCount + 1; ++x)
		{
			// x
			if (y < yCellsCount)
			{
				glm::vec2 xCompPos = glm::vec2(x, y + Hoffset);
				glm::vec2 vel(u->getQuantityAtIdx(x, y), v->getQuantityAtPos(xCompPos));
				glm::vec2 prevPos = xCompPos - vel * t;
				u->setQuantityAtIdx(x, y, u->getQuantityAtPos(prevPos));
			}
			// no slip
			if (x == 0 || x == xCellsCount)
				u->setQuantityAtIdx(x, y, 0.f);
			// y
			if (x < xCellsCount)
			{
				glm::vec2 yCompPos = glm::vec2(x + Hoffset, y);
				glm::vec2 vel(u->getQuantityAtPos(yCompPos), v->getQuantityAtIdx(x, y));
				glm::vec2 prevPos = yCompPos - vel * t;
				v->setQuantityAtIdx(x, y, v->getQuantityAtPos(prevPos));
			}
			// no slip
			if (y == 0 || y == yCellsCount)
				v->setQuantityAtIdx(x, y, 0.f);
		}
	}
	u->postUpdate();
	v->postUpdate();
}

void VelocityField::applyExternalForces(float t, unordered_map<int, int>& liquidCells)
{
	float maxGravityAcc = 9.81f;
	float gravScale = 0.1f;
	// only updated for vel comps bordering fluid
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			if (liquidCells.count((y - 1) * xCellsCount + x) || liquidCells.count(y * xCellsCount + x))
			{
				float yVel = v->getQuantityAtIdx(x, y);
				v->setQuantityAtIdx(x, y, max(-maxGravityAcc, yVel - 0.981f * t));
			}
			// no slip
			if (y == 0)
				v->setQuantityAtIdx(x, y, 0.f);
		}
	}
	v->postUpdate();
}

glm::vec2 VelocityField::getVelAtPos(glm::vec2 pos)
{
	return glm::vec2(u->getQuantityAtPos(pos), v->getQuantityAtPos(pos));
}

void VelocityField::draw(glm::mat4& mvMat, int mvpHandle)
{
	// u->draw(mvMat, mvpHandle);
	//v->draw(mvMat, mvpHandle);
}
