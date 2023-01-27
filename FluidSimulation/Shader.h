#pragma once
#include "Utility.h"

class Shader
{
	unsigned int program;

public:
	Shader(std::string vertexShaderPath, std::string fragmentShaderPath);
	~Shader();

	int getProgram();
};