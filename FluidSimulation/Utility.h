#pragma once

// common headers
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <stdlib.h>     /* srand, rand */
#include <stdio.h>
#include <time.h>       /* time */
#include <iterator>
#include <bitset>
#include <iomanip>
#include <map>
#include <unordered_map>
// Core C++, OpenGL, texture and any other platform specific libs
// memory leak detection
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
// OpenGL specific
#include <glad/glad.h>  // !include before GLFW!
#include <GLFW/glfw3.h>
// math
#include "MathUtility.h"
// defines
const static int WINDOWS_WIDTH = 800;
const static int WINDOWS_HEIGHT = 800;
// common fluid
const float G = -9.81f;	// gravity = -9.81m/s^2, adjustable depending on visual
const float Kcfl = 1.f;	// timestep scale, 1 - 5
// eulerian
const float H = 1.f;	// width/height of a grid cell
const float Hoffset = H * 0.5f;
// lagrangian
const float REST_DENS = 300.f;  // rest density
const float GAS_CONST = 2000.f; // const for equation of state
const float Hrad = 16.f;           // kernel radius
const float Hrad2 = pow(Hrad, 2);        // radius^2 for optimization
const float MASS = 2.5f;        // assume all particles have the same mass
const float VISC = 200.f;       // viscosity constant
const float POLY6 = 4.f / (PI * pow(Hrad, 8.f));
const float SPIKY_GRAD = -10.f / (PI * pow(Hrad, 5.f));
const float VISC_LAP = 40.f / (PI * pow(Hrad, 5.f));
const float EPS = Hrad; // boundary epsilon
const float BOUND_DAMPING = -0.5f;

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