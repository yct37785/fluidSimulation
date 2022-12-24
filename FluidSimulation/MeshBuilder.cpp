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
	else if (type == "blue_marker")
	{
		vector<float> vertices{
		0.1f * H,  0.1f * H,  // top right
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		0.1f * H, -0.1f * H,  // bottom right
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		-0.1f * H, -0.1f * H,  // bottom left
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		-0.1f * H,  0.1f * H,   // top left
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		};
		vector<int> indices{  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		return new Mesh(vertices, indices, GL_STATIC_DRAW);
	}
	else if (type == "sph_blue_marker")
	{
		float rad = Hrad / 2.f * 0.7f;
		vector<float> vertices{
		rad,  rad,  // top right
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		rad, -rad,  // bottom right
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		-rad, -rad,  // bottom left
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		-rad,  rad,   // top left
		0.f / 255.f, 57.f / 255.f, 230.f / 255.f,
		};
		vector<int> indices{  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		return new Mesh(vertices, indices, GL_STATIC_DRAW);
	}
	else if (type == "purple_marker")
	{
		vector<float> vertices{
		0.1f * H,  0.1f * H,  // top right
		1.f, 0.f, 1.f,
		0.1f * H, -0.1f * H,  // bottom right
		1.f, 0.f, 1.f,
		-0.1f * H, -0.1f * H,  // bottom left
		1.f, 0.f, 1.f,
		-0.1f * H,  0.1f * H,   // top left
		1.f, 0.f, 1.f,
		};
		vector<int> indices{  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		return new Mesh(vertices, indices, GL_STATIC_DRAW);
	}
	else if (type == "yellow_marker")
	{
		vector<float> vertices{
		0.1f * H,  0.1f * H,  // top right
		1.f, 1.f, 0.f,
		0.1f * H, -0.1f * H,  // bottom right
		1.f, 1.f, 0.f,
		-0.1f * H, -0.1f * H,  // bottom left
		1.f, 1.f, 0.f,
		-0.1f * H,  0.1f * H,   // top left
		1.f, 1.f, 0.f,
		};
		vector<int> indices{  // note that we start from 0!
			0, 1, 3,   // first triangle
			1, 2, 3    // second triangle
		};
		return new Mesh(vertices, indices, GL_STATIC_DRAW);
	}
	else if (type == "orange_marker")
	{
		vector<float> vertices{
		0.1f * H,  0.1f * H,  // top right
		255.f / 255.f, 128.f / 255.f, 0.f / 255.f,
		0.1f * H, -0.1f * H,  // bottom right
		255.f / 255.f, 128.f / 255.f, 0.f / 255.f,
		-0.1f * H, -0.1f * H,  // bottom left
		255.f / 255.f, 128.f / 255.f, 0.f / 255.f,
		-0.1f * H,  0.1f * H,   // top left
		255.f / 255.f, 128.f / 255.f, 0.f / 255.f,
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
