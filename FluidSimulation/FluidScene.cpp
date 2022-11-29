#include "FluidScene.h"

FluidScene::FluidScene()
{
}

FluidScene::~FluidScene()
{
	delete quadMesh;
	delete gridMesh;
	delete shader;
}

void FluidScene::Init()
{
	xCellsCount = 100;
	float accurateYSpaceHeight = (float)xCellsCount * ((float)WINDOWS_HEIGHT / (float)WINDOWS_WIDTH);
	yCellsCount = (int)accurateYSpaceHeight + 1;
	viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	projMat = glm::frustum(0.f, (float)xCellsCount, 0.f, accurateYSpaceHeight, 3.f, 7.f);
	quadMesh = MeshBuilder::CreateMesh("quad");
	gridMesh = new GridMesh(xCellsCount, yCellsCount, 0, 0);
	shader = new Shader("../Shaders/vertexshader.cpp", "../Shaders/fragmentshader.cpp");
}

void FluidScene::Update(bool inputList[INPUT_TOTAL], float deltaTime)
{
}

void FluidScene::Draw()
{
	glUseProgram(shader->getProgram());
	int mvpHandle = glGetUniformLocation(shader->getProgram(), "uMVPMatrix");

	glm::mat4 mvMat = projMat * viewMat;

	glm::mat4 cursorMat = mvMat * 
		glm::translate(glm::mat4(1.f), glm::vec3(cursorPosX, cursorPosY, 0.f)) * 
		glm::scale(glm::mat4(1.f), glm::vec3(10.f, 10.f, 1.f));

	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));
	glBindVertexArray(gridMesh->getVAO());
	glDrawElements(GL_TRIANGLES, gridMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);

	glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(cursorMat));
	glBindVertexArray(quadMesh->getVAO());
	glDrawElements(GL_TRIANGLES, quadMesh->getTotalIndices(), GL_UNSIGNED_INT, 0);
}

void FluidScene::windowsResize(int width, int height)
{
}

void FluidScene::mouseCallback(double xpos, double ypos)
{
	float gridSize = (float)WINDOWS_WIDTH / (float)xCellsCount;
	cursorPosX = (float)xpos / gridSize;
	cursorPosY = ((float)WINDOWS_HEIGHT - (float)ypos) / gridSize;
	cursorCellX = (int)cursorPosX;
	cursorCellY = (int)cursorPosY;
}
