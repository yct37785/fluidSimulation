#pragma once
#include "Includes.h"
#include "Enums.h"
#include "MathUtility.h"

// general
std::string fileToShaderString(std::string filename);

// shaders
int loadShader(std::string sourceFile, std::string shaderType, unsigned int& shaderObject);
int loadAndLinkShaders(std::string vertexShaderSource, std::string fragmentShaderSource, unsigned int& shaderProgram);

// texture
void generateTexture(std::string src, unsigned int& texture);

// logging
void LogInfo(std::string log);
void LogWarn(std::string log);

//
using vec = std::vector<double>;	// vector
using matrix = std::vector<vec>;	// matrix (=collection of (row) vectors)
const double NEARZERO = 1.0e-10;