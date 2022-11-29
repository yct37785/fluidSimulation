#include "Mesh.h"

Mesh::Mesh(vector<float>& vertices, vector<int>& indices, int usage)
{
	this->vertices = vertices;
	this->indices = indices;
	this->usage = usage;

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(int), indices.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, COORDS_PER_VERTEX * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, COORDS_PER_VERTEX * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
}

Mesh::~Mesh()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
}

unsigned int Mesh::getVAO() const
{
	return VAO;
}

unsigned int Mesh::getEBO() const
{
	return EBO;
}


int Mesh::getTotalVertices() const
{
	return vertices.size() / COORDS_PER_VERTEX;
}

int Mesh::getTotalIndices() const
{
	return indices.size();
}