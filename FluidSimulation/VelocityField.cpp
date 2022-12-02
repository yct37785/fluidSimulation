#include "VelocityField.h"

VelocityField::VelocityField(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this ->yCellsCount = yCellsCount;
	prev = new glm::vec2*[yCellsCount];
	curr = new glm::vec2*[yCellsCount];
	for (int y = 0; y < yCellsCount; ++y)
	{
		prev[y] = new glm::vec2[xCellsCount];
		curr[y] = new glm::vec2[xCellsCount];
		for (int x = 0; x < xCellsCount; ++x)
		{
			curr[y][x].x = (float)rand() / (RAND_MAX / 2.f) * (rand() % 2 ? -1.f : 1.f);
			curr[y][x].y = (float)rand() / (RAND_MAX / 2.f) * (rand() % 2 ? -1.f : 1.f);
			prev[y][x] = curr[y][x];
		}
	}
}

VelocityField::~VelocityField()
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		delete[] prev[y];
		delete[] curr[y];
	}
	delete[] prev;
	delete[] curr;
}

float VelocityField::bilinearInterpolate(float x1, float x2, float y1, float y2,
	glm::vec2& pos, int comp, glm::vec2 q11, glm::vec2 q21, glm::vec2 q12, glm::vec2 q22)
{
	float hx = x2 - x1;
	float hy = y2 - y1;
	float x_1 = x2 == pos.x || hx == 0.f ? 1.f : (x2 - pos.x) / hx;
	float x_2 = pos.x == x1 || hx == 0.f ? 0.f : (pos.x - x1) / hx;
	float r1 = q11[comp] * x_1 + q21[comp] * x_2;
	float r2 = q12[comp] * x_1 + q22[comp] * x_2;
	float y_1 = y2 == pos.y || hy == 0.f ? 1.f : (y2 - pos.y) / hy;
	float y_2 = pos.y == y1 || hy == 0.f ? 0.f : (pos.y - y1) / hy;
	return r1 * y_1 + r2 * y_2;
}

void VelocityField::UT_bilinearInterpolate()
{
	int successCount = 0;
	// test 1
	glm::vec2 pos(2.3f, 2.4f);
	float x1 = 2.f, x2 = 3.f, y1 = 2.f, y2 = 3.f;
	glm::vec2 q11(22.f, 0.f);
	glm::vec2 q21(23.f, 0.f);
	glm::vec2 q12(32.f, 0.f);
	glm::vec2 q22(33.f, 0.f);
	float v = bilinearInterpolate(x1, x2, y1, y2, pos, 0, q11, q21, q12, q22);
	if (v == 26.3f)
		successCount++;
	// test 2: inverse of test 1
	pos = glm::vec2(2.4f, 2.3f);
	q11 = glm::vec2(22.f, 0.f);
	q21 = glm::vec2(32.f, 0.f);
	q12 = glm::vec2(23.f, 0.f);
	q22 = glm::vec2(33.f, 0.f);
	v = bilinearInterpolate(x1, x2, y1, y2, pos, 0, q11, q21, q12, q22);
	if (v == 26.3f)
		successCount++;
	if (successCount == 2)
		cout << "UT: bilinearInterpolate success" << endl;
	else
		cout << "UT: bilinearInterpolate fail" << endl;
}

void VelocityField::getIndicesCoords(float pos, int& minv, int& maxv)
{
	minv = (int)floor(pos);
	maxv = (int)ceil(pos);
}

void VelocityField::UT_getIndicesCoords()
{
	int successTestCount = 0;
	int minv = 0, maxv = 0;
	// v(2.1): 2 - 3
	VelocityField::getIndicesCoords(2.1f, minv, maxv);
	if (minv == 2 && maxv == 3)
		successTestCount++;
	// v(1.9): 1 - 2
	VelocityField::getIndicesCoords(1.9f, minv, maxv);
	if (minv == 1 && maxv == 2)
		successTestCount++;
	if (successTestCount == 2)
		cout << "UT: getHalfIndicesCoords success" << endl;
	else
		cout << "UT: getHalfIndicesCoords fail" << endl;
}

float VelocityField::getVelCompAtPt(glm::vec2 pos, int comp)
{
	int x1, x2, y1, y2;
	VelocityField::getIndicesCoords(pos.x, x1, x2);
	VelocityField::getIndicesCoords(pos.y, y1, y2);
	// bilinear interpolate with surrounding 4 cells
	return bilinearInterpolate(x1, x2, y1, y2, pos, comp, curr[y1][x1], curr[y1][x2], curr[y2][x1], curr[y2][x2]);
}

void VelocityField::UT_getVelCompAtPt()
{
	int successTestCount = 0;
	VelocityField vf(5, 5);
	// test 1: value at exact grid point
	glm::vec2 vel = vf.getVelByIdx(3, 3);
	float v = vf.getVelCompAtPt(glm::vec2(3.f, 3.f), 0);
	if (vel.x == v)
		successTestCount++;
	// test 2: interpolated value
	glm::vec2 q11(22.f, 0.f);
	glm::vec2 q21(23.f, 0.f);
	glm::vec2 q12(32.f, 0.f);
	glm::vec2 q22(33.f, 0.f);
	vf.setVelByIdx(glm::vec2(22.f, 0.f), 3, 3);
	vf.setVelByIdx(glm::vec2(23.f, 0.f), 4, 3);
	vf.setVelByIdx(glm::vec2(32.f, 0.f), 3, 4);
	vf.setVelByIdx(glm::vec2(33.f, 0.f), 4, 4);
	v = vf.getVelCompAtPt(glm::vec2(3.3f, 3.4f), 0);
	if (v == 26.3f)
		successTestCount++;

	if (successTestCount == 2)
		cout << "UT: getVelCompAtPt success" << endl;
	else
		cout << "UT: getVelCompAtPt fail" << endl;
}

glm::vec2 VelocityField::getVelAtPos(glm::vec2 pos)
{
	return glm::vec2();
}

glm::vec2 VelocityField::getVelByIdx(int x, int y)
{
	return prev[y][x];
}

void VelocityField::setVelByIdx(glm::vec2 vel, int x, int y)
{
	prev[y][x] = curr[y][x] = vel;
}

void VelocityField::runUT()
{
	UT_bilinearInterpolate();
	UT_getIndicesCoords();
	UT_getVelCompAtPt();
}