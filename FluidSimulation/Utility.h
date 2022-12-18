#pragma once

// common headers
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <stdlib.h>     /* srand, rand */
#include <stdio.h>
#include <time.h>       /* time */
#include <iterator>
#include <bitset>
#include <iomanip>
#include <random>
#include <map>
#include <unordered_map>
#include <cmath>
#include <numeric>
// Core C++, OpenGL, texture and any other platform specific libs
// memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// OpenGL specific
#include <glad/glad.h>  // !include before GLFW!
#include <GLFW/glfw3.h>
// glm
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
using namespace std;
// defines
#define PI 3.141592653589793
const static int WINDOWS_WIDTH = 800;
const static int WINDOWS_HEIGHT = 800;
// fluid defines
const float G = -9.81f;	// gravity = -9.81m/s^2, adjustable depending on visual
const float Kcfl = 1.f;	// timestep scale, 1 - 5
const float H = 1.f;	// width/height of a grid cell
const float Hoffset = H * 0.5f;
// general
string fileToShaderString(string filename);

// shaders
int loadShader(string sourceFile, string shaderType, unsigned int& shaderObject);
int loadAndLinkShaders(string vertexShaderSource, string fragmentShaderSource, unsigned int& shaderProgram);

// texture
void generateTexture(string src, unsigned int& texture);

// enum
enum INPUT_TYPES {
	INPUT_UNFREEZE_FLUID,
	INPUT_FWD,
	INPUT_BWD,
	INPUT_LEFT,
	INPUT_RIGHT,
	INPUT_UP,
	INPUT_DW,
	INPUT_ENABLE_WIREFRAME,
	INPUT_DISABLE_WIREFRAME,
	INPUT_QUIT,
	INPUT_TOTAL
};

// logging
void LogInfo(string log);
void LogWarn(string log);

//
using vec = vector<double>;	// vector
using matrix = vector<vec>;	// matrix (=collection of (row) vectors)
const double NEARZERO = 1.0e-10;