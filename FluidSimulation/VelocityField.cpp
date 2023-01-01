#include "VelocityField.h"

VelocityField::VelocityField(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	uMarker = MeshBuilder::CreateMesh("purple_marker");
	vMarker = MeshBuilder::CreateMesh("yellow_marker");
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

void VelocityField::draw(glm::mat4& mvMat, int mvpHandle)
{
	u->draw(mvMat, mvpHandle);
	v->draw(mvMat, mvpHandle);
}
