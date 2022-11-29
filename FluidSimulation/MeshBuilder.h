#pragma once
#include "Mesh.h"
#include "GridMesh.h"

class MeshBuilder {

public:
	MeshBuilder();
	~MeshBuilder();

	static Mesh* CreateMesh(string type);
	static Mesh* CreateCustomMesh();
};