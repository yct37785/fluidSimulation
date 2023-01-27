#include "Shader.h"

Shader::Shader(std::string vertexShaderPath, std::string fragmentShaderPath)
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