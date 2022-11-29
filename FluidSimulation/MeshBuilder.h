#pragma once
#include "Mesh.h"

class MeshBuilder {

public:
	MeshBuilder();
	~MeshBuilder();

	static Mesh* CreateMesh(string type);
	static Mesh* CreateCustomMesh();
};