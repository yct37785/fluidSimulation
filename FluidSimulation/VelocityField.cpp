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
			if (x == 0 || x == xCellsCount - 1)
				curr[y][x].x = 0.f;
			else
				curr[y][x].x = (float)rand() / (RAND_MAX / 1.f) * (rand() % 2 ? -1.f : 1.f);
			if (y == 0 || y == yCellsCount - 1)
				curr[y][x].y = 0.f;
			else
				curr[y][x].y = (float)rand() / (RAND_MAX / 1.f) * (rand() % 2 ? -1.f : 1.f);
			// curr[y][x].x = curr[y][x].y = 0.f;
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

bool VelocityField::outOfRange(int x, int y, int maxx, int maxy)
{
	return x < 0 || x >= maxx || y < 0 || y >= maxy;
}

float VelocityField::bilinearInterpolate(float x1, float x2, float y1, float y2,
	glm::vec2& pos, float q11, float q21, float q12, float q22)
{
	float hx = x2 - x1;
	float hy = y2 - y1;
	float x_1 = x2 == pos.x || hx == 0.f ? 1.f : (x2 - pos.x) / hx;
	float x_2 = pos.x == x1 || hx == 0.f ? 0.f : (pos.x - x1) / hx;
	float r1 = q11 * x_1 + q21 * x_2;
	float r2 = q12 * x_1 + q22 * x_2;
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
	float v = bilinearInterpolate(x1, x2, y1, y2, pos, 22.f, 23.f, 32.f, 33.f);
	if (v == 26.3f)
		successCount++;
	// test 2: inverse of test 1
	pos = glm::vec2(2.4f, 2.3f);
	v = bilinearInterpolate(x1, x2, y1, y2, pos, 22.f, 32.f, 23.f, 33.f);
	if (v == 26.3f)
		successCount++;
	// test 3
	pos = glm::vec2(0.f, 0.f);
	x1 = 0.f, x2 = 0.5f, y1 = 0.f, y2 = 1.f;
	v = bilinearInterpolate(x1, x2, y1, y2, pos, 0.f, 20.f, 0.f, 20.f);
	if (v == 0.f)
		successCount++;
	if (successCount == 3)
		cout << "UT: bilinearInterpolate success" << endl;
	else
		cout << "UT: bilinearInterpolate fail" << endl;
}

void VelocityField::getHalfIndicesCoords(float pos, float& minv, float& maxv)
{
	if (pos - floor(pos) > 0.5f)
	{
		minv = floor(pos) + 0.5f;
		maxv = ceil(pos) + 0.5f;
	}
	else
	{
		minv = floor(pos) - 0.5f;
		maxv = floor(pos) + 0.5f;
	}
}

void VelocityField::UT_getHalfIndicesCoords()
{
	int successTestCount = 0;
	float minv = 0.f, maxv = 0.f;
	// v(2.1): 1.5 - 2.5
	VelocityField::getHalfIndicesCoords(2.1f, minv, maxv);
	if (minv == 1.5f && maxv == 2.5f)
		successTestCount++;
	// v(1.9): 1.5 - 2.5
	VelocityField::getHalfIndicesCoords(1.9f, minv, maxv);
	if (minv == 1.5f && maxv == 2.5f)
		successTestCount++;
	if (successTestCount == 2)
		cout << "UT: getHalfIndicesCoords success" << endl;
	else
		cout << "UT: getHalfIndicesCoords fail" << endl;
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
		cout << "UT: getIndicesCoords success" << endl;
	else
		cout << "UT: getIndicesCoords fail" << endl;
}

// param pos will be in idx space, i.e. pos(1.f) corr. to MAC_space(0.5f), or edge of a MAC cell
// and pos(1.5f) corr. to MAC_space(1.f), or middle of a MAC cell
void VelocityField::getIndices(glm::vec2 pos, char comp, int xCellsCount, int yCellsCount, int& x1, int& x2, int& y1, int& y2)
{
	// ----y+1/2----
	// |           |
	// |           |
	// x-1/2       x+1/2
	// |           |
	// |           |
	// ----y-1/2----

	// ----uy+1----
	// |           |
	// |           |
	// x           x+1
	// |           |
	// |           |
	// -----uy-----

	// Ex: given idx space pos(1.7, 1.7), derive bilinear coords for x and y comp
	// pos(1.7, 1.7) -> MAC_space(1.2, 1.2)
	// X: x1 = 0.5 (1), x2 = 1.5 (2), y1 = 1 (1), y2 = 2 (2)
	// Y: x1 = 1 (1), x2 = 2 (2), y1 = 0.5 (1), y2 = 1.5 (2)

	glm::vec2 offsetPos = pos - 0.0f;
	if (comp == 'x')
	{
		float hfi_x1, hfi_x2;
		getHalfIndicesCoords(offsetPos.x - 0.5f, hfi_x1, hfi_x2);
		x1 = int(hfi_x1 + 0.5f);
		x2 = int(hfi_x2 + 0.5f);
		getIndicesCoords(offsetPos.y - 0.5f, y1, y2);
	}
	else if (comp == 'y')
	{
		float hfi_y1, hfi_y2;
		getHalfIndicesCoords(offsetPos.y - 0.5f, hfi_y1, hfi_y2);
		y1 = int(hfi_y1 + 0.5f);
		y2 = int(hfi_y2 + 0.5f);
		getIndicesCoords(offsetPos.x - 0.5f, x1, x2);
	}
}

void VelocityField::UT_getIndices()
{
	bool fail = false;
	int x1, x2, y1, y2;
	// test 1: middle
	getIndices(glm::vec2(1.7f, 1.7f), 'x', 10, 10, x1, x2, y1, y2);
	if (!(x1 == 1 && x2 == 2 && y1 == 1 && y2 == 2))
		fail = true;
	getIndices(glm::vec2(1.7f, 1.7f), 'y', 10, 10, x1, x2, y1, y2);
	if (!(x1 == 1 && x2 == 2 && y1 == 1 && y2 == 2))
		fail = true;
	// test 2: lower corner
	getIndices(glm::vec2(0.1f, 0.3f), 'x', 10, 10, x1, x2, y1, y2);
	if (!(x1 == 0 && x2 == 1 && y1 == -1 && y2 == 0))
		fail = true;
	getIndices(glm::vec2(0.1f, 0.3f), 'y', 10, 10, x1, x2, y1, y2);
	if (!(x1 == -1 && x2 == 0 && y1 == 0 && y2 == 1))
		fail = true;
	// test 3: lower center
	getIndices(glm::vec2(5.1f, 0.3f), 'x', 10, 10, x1, x2, y1, y2);
	if (!(x1 == 5 && x2 == 6 && y1 == -1 && y2 == 0))
		fail = true;
	getIndices(glm::vec2(5.1f, 0.3f), 'y', 10, 10, x1, x2, y1, y2);
	if (!(x1 == 4 && x2 == 5 && y1 == 0 && y2 == 1))
		fail = true;

	if (!fail)
		cout << "UT: getIndices success" << endl;
	else
		cout << "UT: getIndices fail" << endl;
}

float VelocityField::getVelCompAtPt(glm::vec2 pos, int comp)
{
	int x1, x2, y1, y2;
	getIndicesCoords(pos.x, x1, x2);
	getIndicesCoords(pos.y, y1, y2);
	// VelocityField::getIndices(pos, comp == 0 ? 'x' : 'y', xCellsCount, yCellsCount, x1, x2, y1, y2);
	/*cout << "1: " << x1 << " " << x2 << " " << y1 << " " << y2 << endl;
	VelocityField::getIndices(pos, comp == 0 ? 'x' : 'y', xCellsCount, yCellsCount, x1, x2, y1, y2);
	cout << "2: " << x1 << " " << x2 << " " << y1 << " " << y2 << endl;*/
	//if vel faces boundary
	// no need to clamp index, clamp boundary values to 0 is fine
	float q11 = outOfRange(x1, y1, xCellsCount, yCellsCount) ? 0.f : prev[y1][x1][comp];
	float q21 = outOfRange(x2, y1, xCellsCount, yCellsCount) ? 0.f : prev[y1][x2][comp];
	float q12 = outOfRange(x1, y2, xCellsCount, yCellsCount) ? 0.f : prev[y2][x1][comp];
	float q22 = outOfRange(x2, y2, xCellsCount, yCellsCount) ? 0.f : prev[y2][x2][comp];
	// bilinear interpolate with surrounding 4 cells
	return bilinearInterpolate(x1, x2, y1, y2, pos, q11, q21, q12, q22);
}

void VelocityField::UT_getVelCompAtPt()
{
	int successTestCount = 0;
	VelocityField vf(5, 5);
	// test 1: value at exact grid point
	glm::vec2 vel = vf.prev[3][3];
	float v = vf.getVelCompAtPt(glm::vec2(3.f, 3.f), 0);
	if (vel.x == v)
		successTestCount++;
	// test 2: interpolated value
	vf.setVelByIdx(glm::vec2(22.f, 22.f), 3, 3);
	vf.setVelByIdx(glm::vec2(23.f, 32.f), 4, 3);
	vf.setVelByIdx(glm::vec2(32.f, 23.f), 3, 4);
	vf.setVelByIdx(glm::vec2(33.f, 33.f), 4, 4);
	v = vf.getVelCompAtPt(glm::vec2(3.3f, 3.4f), 0);
	if (v == 26.3f)
		successTestCount++;
	// test 3: y comp
	v = vf.getVelCompAtPt(glm::vec2(3.4f, 3.3f), 1);
	if (v == 26.3f)
		successTestCount++;
	if (successTestCount == 3)
		cout << "UT: getVelCompAtPt success" << endl;
	else
		cout << "UT: getVelCompAtPt fail" << endl;
}

void VelocityField::advectSelf(float t)
{
	// get vel at pos(x',y'), trace backwards, get vel at that pos, then apply to (x',y')
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			glm::vec2 prevPos = glm::vec2((float)x, (float)y) - prev[y][x] * t;
			float xcomp = getVelCompAtPt(prevPos, 0);
			float ycomp = getVelCompAtPt(prevPos, 1);
			curr[y][x] = glm::vec2(xcomp, ycomp);
		}
	}
}

void VelocityField::applyExternalForces(float t)
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// hardcoded gravity
			if (y > 0)
				curr[y][x].y = max(-9.81f, curr[y][x].y - 0.981f * t);
		}
	}
}

void VelocityField::postUpdate()
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			prev[y][x] = curr[y][x];
		}
	}
}

void VelocityField::draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh)
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// get angular dir of velocity from lesser faces
			float angleRad = (float)atan2(-curr[y][x].x, curr[y][x].y);
			float scale = glm::length(curr[y][x]);
			// u
			glm::mat4 mvpMat = mvMat *
				glm::translate(glm::mat4(1.f), glm::vec3(x, y, 0.f)) *
				glm::rotate(glm::mat4(1.f), angleRad, glm::vec3(0.f, 0.f, 1.f)) *
				glm::scale(glm::mat4(1.f), glm::vec3(scale, scale, 1.f));
			glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
			glBindVertexArray(triangleMesh->getVAO());
			glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
		}
	}
}

glm::vec2 VelocityField::getMaxU()
{
	float maxLen = 0.f;
	glm::vec2 maxU(0.f, 0.f);
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			float dist = glm::length(curr[y][x]);
			if (dist > maxLen)
			{
				maxLen = dist;
				maxU = curr[y][x];
			}
		}
	}
	return maxU;
}

glm::vec2 VelocityField::getVelAtPos(glm::vec2 pos)
{
	float x = getVelCompAtPt(pos, 0);
	float y = getVelCompAtPt(pos, 1);
	return glm::vec2(x, y);
}

void VelocityField::setVelByIdx(glm::vec2 vel, int x, int y)
{
	prev[y][x] = curr[y][x] = vel;
}

void VelocityField::runUT()
{
	UT_bilinearInterpolate();
	UT_getHalfIndicesCoords();
	UT_getIndicesCoords();
	UT_getIndices();
	UT_getVelCompAtPt();
}