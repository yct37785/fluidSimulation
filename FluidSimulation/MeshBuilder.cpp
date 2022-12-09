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
	else if (type == "blue_quad")
	{
		vector<float> vertices{
		0.1f * H,  0.1f * H,  // top right
		0.f, 0.f, 1.f,
		0.1f * H, -0.1f * H,  // bottom right
		0.f, 0.f, 1.f,
		-0.1f * H, -0.1f * H,  // bottom left
		0.f, 0.f, 1.f,
		-0.1f * H,  0.1f * H,   // top left
		0.f, 0.f, 1.f,
		};
		vector<int> indices{  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		return new Mesh(vertices, indices, GL_STATIC_DRAW);
	}
	else if (type == "triangle")
	{
		vector<float> vertices{
		0.0f,  0.25f,  // top
		1.f, 0.f, 0.f,
		-0.025f, -0.05f,  // bottom left
		1.f, 0.f, 0.f,
		0.025f, -0.05f,  // bottom right
		1.f, 0.f, 0.f,
		};
		vector<int> indices{  // note that we start from 0!
			1, 0, 2   // first triangle
		};
		return new Mesh(vertices, indices, GL_STATIC_DRAW);
	}
	return nullptr;
}

Mesh* MeshBuilder::CreateCustomMesh()
{
	return nullptr;
}
