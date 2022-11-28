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
static int windowsWidth = 1280;
static int windowsHeight = 720;

// general
string fileToShaderString(string filename);

// shaders
int loadShader(string sourceFile, string shaderType, unsigned int& shaderObject);
int loadAndLinkShaders(string vertexShaderSource, string fragmentShaderSource, unsigned int& shaderProgram);

// texture
void generateTexture(string src, unsigned int& texture);

// enum
enum INPUT_TYPES {
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