#include "SPHFluidScene.h"

SPHFluidScene::SPHFluidScene()
{
}

SPHFluidScene::~SPHFluidScene()
{
	delete fluidGrid;
	delete quadMesh;
	delete shader;
}

void SPHFluidScene::Init()
{
	xCellsCount = 30;
	float accurateYSpaceHeight = (float)xCellsCount * ((float)WINDOWS_HEIGHT / (float)WINDOWS_WIDTH);
	yCellsCount = (int)accurateYSpaceHeight + 1;
	viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	projMat = glm::frustum(-Hrad, (float)xCellsCount * Hrad + Hrad * 2, -Hrad, accurateYSpaceHeight * Hrad + Hrad * 2, 3.f, 7.f);
	quadMesh = MeshBuilder::CreateMesh("quad");
	shader = new Shader("../Shaders/vertexshader.cpp", "../Shaders/fragmentshader.cpp");
	fluidGrid = new SPH_FluidGrid(xCellsCount, yCellsCount);
	spawnParticles = false;
}

void SPHFluidScene::Update(bool inputList[INPUT_TOTAL], float deltaTime)
{
	if (!spawnParticles && inputList[INPUT_UNFREEZE_FLUID])
	{
		spawnParticles = true;
		fluidGrid->spawnParticles();
	}
	fluidGrid->Update(deltaTime);
}

void SPHFluidScene::Draw()
{
	glUseProgram(shader->getProgram());
	int mvpHandle = glGetUniformLocation(shader->getProgram(), "uMVPMatrix");

	glm::mat4 mvMat = projMat * viewMat;

	glm::mat4 cursorMat = mvMat *
		glm::translate(glm::mat4(1.f), glm::vec3(cursorPosX, cursorPosY, 0.f)) *
		glm::scale(glm::mat4(1.f), glm::vec3(10.f, 10.f, 1.f));

	fluidGrid->Draw(mvpHandle, mvMat);
}

void SPHFluidScene::windowsResize(int width, int height)
{
}

void SPHFluidScene::mouseCallback(double xpos, double ypos)
{
	float gridSize = (float)WINDOWS_WIDTH / (float)xCellsCount;
	cursorPosX = (float)xpos / gridSize;
	cursorPosY = ((float)WINDOWS_HEIGHT - (float)ypos) / gridSize;
	cursorCellX = (int)cursorPosX;
	cursorCellY = (int)cursorPosY;
}
