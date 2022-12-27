#include "MAC_VelocityField.h"

MAC_VelocityField::MAC_VelocityField(int xCellsCount, int yCellsCount)
{
	this->xCellsCount = xCellsCount;
	this->yCellsCount = yCellsCount;
	x_prev = new float* [yCellsCount];
	x_curr = new float* [yCellsCount];
	y_prev = new float* [yCellsCount + 1];
	y_curr = new float* [yCellsCount + 1];
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		y_prev[y] = new float[xCellsCount];
		y_curr[y] = new float[xCellsCount];
		for (int x = 0; x < xCellsCount; ++x)
		{
			y_prev[y][x] = y_curr[y][x] = (float)rand() / (RAND_MAX / 1.f) * (rand() % 2 ? -1.f : 1.f);
			y_prev[y][x] = y_curr[y][x] = 0.f;
		}
		if (y < yCellsCount)
		{
			x_prev[y] = new float[xCellsCount + 1];
			x_curr[y] = new float[xCellsCount + 1];
			for (int x = 0; x < xCellsCount + 1; ++x)
			{
				x_prev[y][x] = x_curr[y][x] = (float)rand() / (RAND_MAX / 1.f) * (rand() % 2 ? -1.f : 1.f);
				x_prev[y][x] = x_curr[y][x] = 0.f;
			}
		}
	}
	xMarker = MeshBuilder::CreateMesh("purple_marker");
	yMarker = MeshBuilder::CreateMesh("yellow_marker");
}

MAC_VelocityField::~MAC_VelocityField()
{
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		if (y < yCellsCount)
		{
			delete[] x_prev[y];
			delete[] x_curr[y];
		}
		delete[] y_prev[y];
		delete[] y_curr[y];
	}
	delete[] x_prev;
	delete[] y_prev;
	delete[] x_curr;
	delete[] y_curr;
	delete xMarker;
	delete yMarker;
}

bool MAC_VelocityField::outOfRange(int x, int y, int maxx, int maxy)
{
	return x < 0 || x >= maxx || y < 0 || y >= maxy;
}

void MAC_VelocityField::getIndicesCoords(float pos, int& minv, int& maxv)
{
	// eg. 0.2 -> idx(-1, 0)
	minv = (int)floor(pos);
	maxv = (int)ceil(pos);
}

float MAC_VelocityField::bilinearInterpolate(float x1, float x2, float y1, float y2,
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

float MAC_VelocityField::getVelCompAtPos(glm::vec2& pos, char comp)
{
	glm::vec2 normPos = pos / H;
	// get indexes
	int x1, x2, y1, y2;
	if (comp == 'x')
		normPos.y -= 0.5f;
	else
		normPos.x -= 0.5f;
	getIndicesCoords(normPos.x, x1, x2);
	getIndicesCoords(normPos.y, y1, y2);
	// get values
	int x_extra = 0, y_extra = 0;
	float** prev;
	if (comp == 'x')
	{
		x_extra++;
		prev = x_prev;
	}
	else
	{
		y_extra++;
		prev = y_prev;
	}
	// no slip: out of bounds vel comps = 0
	float q11 = outOfRange(x1, y1, xCellsCount + x_extra, yCellsCount + y_extra) ? 0.f : prev[y1][x1];
	float q21 = outOfRange(x2, y1, xCellsCount + x_extra, yCellsCount + y_extra) ? 0.f : prev[y1][x2];
	float q12 = outOfRange(x1, y2, xCellsCount + x_extra, yCellsCount + y_extra) ? 0.f : prev[y2][x1];
	float q22 = outOfRange(x2, y2, xCellsCount + x_extra, yCellsCount + y_extra) ? 0.f : prev[y2][x2];
	// bilinear
	return bilinearInterpolate(x1 * H, x2 * H, y1 * H, y2 * H, normPos, q11, q21, q12, q22);
}

glm::vec2 MAC_VelocityField::getVelAtPos(glm::vec2& pos)
{
	float xComp = getVelCompAtPos(pos, 'x');
	float yComp = getVelCompAtPos(pos, 'y');
	return glm::vec2(xComp, yComp);
}

float MAC_VelocityField::getCompByIdx(int x, int y, char comp)
{
	if (comp == 'x')
	{
		if (x >= 0 && x < xCellsCount + 1 && y >= 0 && y < yCellsCount)
			return x_prev[y][x];
		else
			return -1.f;
	}
	else
	{
		if (y >= 0 && y < yCellsCount + 1 && x >= 0 && x < xCellsCount)
			return y_prev[y][x];
		else
			return -1.f;
	}
}

void MAC_VelocityField::addToCompByIdx(int x, int y, char comp, float v)
{
	if (comp == 'x')
		x_curr[y][x] += v;
	else
		y_curr[y][x] += v;
}

void MAC_VelocityField::setCompByIdx(int x, int y, char comp, float v)
{
	if (comp == 'x')
		x_curr[y][x] = v;
	else
		y_curr[y][x] = v;
}

bool MAC_VelocityField::isLiquidCell(int x, int y, unordered_map<int, int>& liquidCells)
{
	return !outOfRange(x, y, xCellsCount, yCellsCount) && liquidCells.count(y * xCellsCount + x);
}

void MAC_VelocityField::advectSelf(float t, unordered_map<int, int>& liquidCells)
{
	// get vel at pos(x',y'), trace backwards, get vel at that pos, then apply to (x',y')
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		for (int x = 0; x < xCellsCount + 1; ++x)
		{
			// x
			if (y < yCellsCount)
			{
				//if (liquidCells.count(y * xCellsCount + x - 1) || liquidCells.count(y * xCellsCount + x))
				{
					glm::vec2 xCompPos = glm::vec2(x, y + Hoffset);
					glm::vec2 vel = getVelAtPos(xCompPos);
					glm::vec2 prevPos = xCompPos - vel * t;
					x_curr[y][x] = getVelCompAtPos(prevPos, 'x');
				}
				//else
				//	x_curr[y][x] = 0.f;
				// no slip
				if (x == 0 || x == xCellsCount)
					x_curr[y][x] = 0.f;
			}
			// y
			if (x < xCellsCount)
			{
				//if (liquidCells.count((y - 1) * xCellsCount + x) || liquidCells.count(y * xCellsCount + x))
				{
					glm::vec2 yCompPos = glm::vec2(x + Hoffset, y);
					glm::vec2 vel = getVelAtPos(yCompPos);
					glm::vec2 prevPos = yCompPos - vel * t;
					y_curr[y][x] = getVelCompAtPos(prevPos, 'y');
				}
				//else
				//	y_curr[y][x] = 0.f;
				// no slip
				if (y == 0 || y == yCellsCount)
					y_curr[y][x] = 0.f;
			}
		}
	}
}

void MAC_VelocityField::applyExternalForces(float t, unordered_map<int, int>& liquidCells)
{
	float maxGravityAcc = 9.81f;
	float gravScale = 0.1f;
	unordered_map<int, bool> updatedVels;
	// only updated for vels bordering fluid (both x and y)
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// is fluid
			if (liquidCells.count(y * xCellsCount + x))
			{
				if (!updatedVels.count(y * xCellsCount + x))
				{
					// if no acc. then will have no splashes
					y_curr[y][x] = max(-maxGravityAcc, y_curr[y][x] - 0.981f * t);
					updatedVels[y * xCellsCount + x] = true;
				}
				if (!updatedVels.count((y + 1) * xCellsCount + x))
				{
					y_curr[y + 1][x] = max(-maxGravityAcc, y_curr[y + 1][x] - 0.981f * t);
					updatedVels[(y + 1) * xCellsCount + x] = true;
				}
			}
		}
	}
	// clamp
	for (int x = 0; x < xCellsCount; ++x)
		y_curr[0][x] = 0.f;
}

void MAC_VelocityField::postUpdate()
{
	for (int y = 0; y < yCellsCount + 1; ++y)
	{
		for (int x = 0; x < xCellsCount + 1; ++x)
		{
			// x
			if (y < yCellsCount)
				x_prev[y][x] = x_curr[y][x];
			// y
			if (x < xCellsCount)
				y_prev[y][x] = y_curr[y][x];
		}
	}
}

void MAC_VelocityField::draw(glm::mat4& mvMat, int mvpHandle, Mesh* triangleMesh)
{
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			// get angular dir of velocity from lesser faces
			float angleRad = (float)atan2(-x_curr[y][x], y_curr[y][x]);
			//float scale = glm::length(glm::vec2(x_curr[y][x], y_curr[y][x]));
			float scale = H;
			// u
			glm::mat4 mvpMat = mvMat *
				glm::translate(glm::mat4(1.f), glm::vec3(x * H + Hoffset, y * H + Hoffset, 0.f)) *
				glm::rotate(glm::mat4(1.f), angleRad, glm::vec3(0.f, 0.f, 1.f)) *
				glm::scale(glm::mat4(1.f), glm::vec3(scale, scale, 1.f));
			glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
			glBindVertexArray(triangleMesh->getVAO());
			glDrawElements(GL_TRIANGLES, triangleMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
		}
	}
	//for (int y = 0; y < yCellsCount + 1; ++y)
	//{
	//	for (int x = 0; x < xCellsCount + 1; ++x)
	//	{
	//		// x
	//		if (y < yCellsCount)
	//		{
	//			// float x_scale = x_curr[y][x];
	//			float x_scale = x_curr[y][x] == 0.f ? 0.f : 1.f;
	//			glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(x * H, y * H + Hoffset, 0.f))
	//				* glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));
	//			glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	//			glBindVertexArray(xMarker->getVAO());
	//			glDrawElements(GL_TRIANGLES, xMarker->getTotalIndices(), GL_UNSIGNED_INT, 0);
	//		}
	//		// y
	//		if (x < xCellsCount)
	//		{
	//			// float y_scale = y_curr[y][x];
	//			float y_scale = y_curr[y][x] == 0.f ? 0.f : 1.f;
	//			glm::mat4 mvpMat = mvMat * glm::translate(glm::mat4(1.f), glm::vec3(x * H + Hoffset, y * H, 0.f))
	//				* glm::scale(glm::mat4(1.f), glm::vec3(1.f, 1.f, 1.f));;
	//			glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvpMat));
	//			glBindVertexArray(yMarker->getVAO());
	//			glDrawElements(GL_TRIANGLES, yMarker->getTotalIndices(), GL_UNSIGNED_INT, 0);
	//		}
	//	}
	//}
}

float MAC_VelocityField::getMaxU()
{
	float max_len = 0.f;
	for (int y = 0; y < yCellsCount; ++y)
	{
		for (int x = 0; x < xCellsCount; ++x)
		{
			if (y < yCellsCount && max_len < x_curr[y][x])
				max_len = x_curr[y][x];
			if (x < xCellsCount && max_len < y_curr[y][x])
				max_len = y_curr[y][x];
		}
	}
	return max_len;
}