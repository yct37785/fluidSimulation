#include "Utility.h"
// texture loading
#include <STB/stb_image.h>

string fileToShaderString(string filename)
{
	string output = "";
	fstream inputFile(filename.c_str(), fstream::in);
	if (!inputFile.is_open())
	{
		LogWarn("Failed to open file!");
		return "";
	}
	string aLine;
	while (getline(inputFile, aLine))
	{
		// read digits
		output += aLine + "\n";
	}
	inputFile.close();
	return output.c_str();
}

int loadShader(string sourceFile, string shaderType, unsigned int& shaderObject)
{
	string shaderSource = fileToShaderString(sourceFile);
	const char* shaderSourceCStr = shaderSource.c_str();

	// compile vertex shader
	if (shaderType == "vertex")
	{
		shaderObject = glCreateShader(GL_VERTEX_SHADER);
	}
	else if (shaderType == "fragment")
	{
		shaderObject = glCreateShader(GL_FRAGMENT_SHADER);
	}
	else
	{
		LogWarn(shaderType + " is not a recognised shader type");
	}

	glShaderSource(shaderObject, 1, &shaderSourceCStr, NULL);

	glCompileShader(shaderObject);

	// shader error checking
	int  success;
	char infoLog[512];
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &success);

	if (!success)
	{
		glGetShaderInfoLog(shaderObject, 512, NULL, infoLog);
		LogWarn(shaderType + " shader compilation failed: " + infoLog);
		return 0;
	}
	return 1;
}

int loadAndLinkShaders(string vertexShaderSource, string fragmentShaderSource, unsigned int& shaderProgram)
{
	unsigned int vertexShader, fragmentShader;

	// compile shaders
	loadShader(vertexShaderSource, "vertex", vertexShader);
	loadShader(fragmentShaderSource, "fragment", fragmentShader);

	// link shaders
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	int  success;
	char infoLog[512];
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success)
	{
		glGetShaderInfoLog(shaderProgram, 512, NULL, infoLog);
		LogWarn(string("shader program linkage failed: ") + infoLog);
		return -1;
	}

	// cleanup
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	return 1;
}

void generateTexture(string src, unsigned int& texture)
{
	string ext = string(src).erase(0, src.find_last_of('.') + 1);

	// generate texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// texture is of s, t, r coordinates/axis
	// set the texture wrapping for each axis
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// texture filtering: decide on how to exactly map texture pixel (texel) to the texture coord
	// min filter: if texture size is downsized
	// max filter: if texture size is upsized
	// GL_TEXTURE_MIN_FILTER for mipmap:
	//	-GL_NEAREST_MIPMAP_NEAREST: use nearest mipmp + use nearest neighbor interpolation for sampling
	// 	-GL_LINEAR_MIPMAP_NEAREST: ^ but using linear interpolation for sampling
	//	-GL_LINEAR_MIPMAP_LINEAR: linearly interpolate btw the 2 mipmaps that closely matches the pixel size + sample 
	// interpolated lvl via linear interpolation
	// 	-GL_NEAREST_MIPMAP_LINEAR: ^ but using nearset neighbor interpolation for sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// load texture image
	int width, height, nrChannels;
	unsigned char* data = stbi_load(src.c_str(), &width, &height, &nrChannels, 0);
	// GLvoid* data = stbi_load(src.c_str(), &width, &height, &nrChannels, 0);

	// generate texture (bounded to current GL_TEXTURE_2D)
	if (data)
	{
		if (ext == "png")
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		}
		else
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		}
		LogInfo("Img w: " + to_string(width) + "  Img h: " + to_string(height) + "  Img ch: " + to_string(nrChannels));
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else
	{
		LogWarn(string("Failed to load texture image: ") + src);
	}

	// free memory
	stbi_image_free(data);
}

void LogInfo(string log)
{
	cout << "Info: " << log << endl;
}

void LogWarn(string log)
{
	cout << "WARN: " << log << endl;
}