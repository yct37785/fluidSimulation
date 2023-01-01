#include "MathUtility.h"

float bilinearInterpolate(float x1, float x2, float y1, float y2, glm::vec2 pos, float q11, float q21, float q12, float q22)
{
	// get xlen and ylen
	float hx = x2 - x1;
	float hy = y2 - y1;
	// get x min frac (posx - x1) and x max frac (x2 - posx) for weighing q values
	float x_1 = x2 == pos.x || hx == 0.f ? 1.f : (x2 - pos.x) / hx;
	float x_2 = pos.x == x1 || hx == 0.f ? 0.f : (pos.x - x1) / hx;
	// weigh q11 and q21 values by min and max frac respectively
	// vice versa for q12 and q22
	float r1 = q11 * x_1 + q21 * x_2;
	float r2 = q12 * x_1 + q22 * x_2;
	float y_1 = y2 == pos.y || hy == 0.f ? 1.f : (y2 - pos.y) / hy;
	float y_2 = pos.y == y1 || hy == 0.f ? 0.f : (pos.y - y1) / hy;
	// return y min frac and y max frac weighted with x
	return r1 * y_1 + r2 * y_2;
}