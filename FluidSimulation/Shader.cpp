#include "Shader.h"

Shader::Shader(string vertexShaderPath, string fragmentShaderPath)
{
    loadAndLinkShaders(vertexShaderPath, fragmentShaderPath, program);
}

Shader::~Shader()
{
	glDeleteShader(program);
}

int Shader::getProgram()
{
	return program;
}