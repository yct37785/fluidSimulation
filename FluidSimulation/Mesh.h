#pragma once
#include "Utility.h"

class Mesh {
public:
	static const int COORDS_PER_VERTEX = 5;

protected:
	unsigned int VBO, EBO, VAO;
    std::vector<float> vertices;
	std::vector<int> indices;
    int usage;

	void initMesh(std::vector<float>& vertices, std::vector<int>& indices, int usage);

public:
	Mesh();
	Mesh(std::vector<float>& vertices, std::vector<int>& indices, int usage);
	~Mesh();

	unsigned int getVAO() const;
	unsigned int getEBO() const;
	int getTotalVertices() const;
	int getTotalIndices() const;
};
