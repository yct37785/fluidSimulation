#pragma once
#include "Utility.h"

class Shader
{
	unsigned int program;

public:
	Shader(string vertexShaderPath, string fragmentShaderPath);
	~Shader();

	int getProgram();
};