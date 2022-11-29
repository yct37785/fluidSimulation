#include "MeshBuilder.h"

MeshBuilder::MeshBuilder()
{
}

MeshBuilder::~MeshBuilder()
{
}

Mesh* MeshBuilder::CreateMesh(string type)
{
	if (type == "quad")
	{
		vector<float> vertices{
		0.5f,  0.5f,  // top right
		1.f, 1.f, 0.f,
		0.5f, -0.5f,  // bottom right
		1.f, 1.f, 0.f,
		-0.5f, -0.5f,  // bottom left
		1.f, 1.f, 0.f,
		-0.5f,  0.5f,   // top left 
		1.f, 1.f, 0.f,
		};
		vector<int> indices{  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		return new Mesh(vertices, indices, GL_STATIC_DRAW);
	}
	return nullptr;
}

Mesh* MeshBuilder::CreateCustomMesh()
{
	return nullptr;
}
