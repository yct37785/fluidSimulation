#pragma once
#include <algorithm>
#include <random>
#include <cmath>
#include <numeric>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
// defines
#define PI 3.141592653589793
using namespace std;

float bilinearInterpolate(float x1, float x2, float y1, float y2, glm::vec2 pos, float q11, float q21, float q12, float q22);