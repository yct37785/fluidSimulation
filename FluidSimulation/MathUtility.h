#pragma once
#include "Includes.h"

float bilinearInterpolate(float x1, float x2, float y1, float y2, glm::vec2 pos, float q11, float q21, float q12, float q22);