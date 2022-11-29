#pragma once
#include "Utility.h"

class Mesh {
public:
	static const int COORDS_PER_VERTEX = 5;

protected:
	unsigned int VBO, EBO, VAO;
    vector<float> vertices;
	vector<int> indices;
    int usage;

public:
	Mesh(vector<float>& vertices, vector<int>& indices, int usage);
	~Mesh();

	unsigned int getVAO() const;
	unsigned int getEBO() const;
	int getTotalVertices() const;
	int getTotalIndices() const;
};
