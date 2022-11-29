#include "FluidScene.h"

FluidScene::FluidScene()
{
}

FluidScene::~FluidScene()
{
	delete quadMesh;
	delete shader;
}

void FluidScene::Init()
{
	viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	projMat = glm::frustum(0.f, (float)X_CELLS_COUNT, 0.f, (float)Y_CELLS_COUNT, 3.f, 7.f);
	cursorPosMat = glm::translate(glm::mat4(1.f), glm::vec3(50.f, 50.f, 0.f));
	glm::mat4 scale = glm::scale(glm::mat4(1.f), glm::vec3(10.f, 10.f, 1.f));
	cursorPosMat = cursorPosMat * scale;
	quadMesh = MeshBuilder::CreateMesh("quad");
	shader = new Shader("../Shaders/vertexshader.cpp", "../Shaders/fragmentshader.cpp");
}

void FluidScene::Update(bool inputList[INPUT_TOTAL], float deltaTime)
{
}

void FluidScene::Draw()
{
	glUseProgram(shader->getProgram());

	glm::mat4 mvMat = glm::mat4(1.f);

	int mvpHandle = glGetUniformLocation(shader->getProgram(), "uMVPMatrix");
	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));

	glBindVertexArray(quadMesh->getVAO());
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, quadMesh->getEBO());
	glDrawElements(GL_TRIANGLES, quadMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}

void FluidScene::windowsResize()
{
}

void FluidScene::mouse_callback(double xpos, double ypos)
{
}
