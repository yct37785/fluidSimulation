#include "EulerianFluidScene.h"

EulerianFluidScene::EulerianFluidScene()
{
}

EulerianFluidScene::~EulerianFluidScene()
{
	delete quadMesh;
	delete shader;
	delete fluidGrid;
}

void EulerianFluidScene::Init()
{
	unfreezeFluid = false;
	xCellsCount = 60;
	float accurateYSpaceHeight = (float)xCellsCount * ((float)WINDOWS_HEIGHT / (float)WINDOWS_WIDTH);
	yCellsCount = (int)accurateYSpaceHeight + 1;
	viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	projMat = glm::frustum(-H, (float)xCellsCount * H + H * 2, -H, accurateYSpaceHeight * H + H * 2, 3.f, 7.f);
	quadMesh = MeshBuilder::CreateMesh("quad");
	shader = new Shader("../Shaders/vertexshader.cpp", "../Shaders/fragmentshader.cpp");
	fluidGrid = new MAC_FluidGrid(xCellsCount, yCellsCount);
}

void EulerianFluidScene::Update(bool inputList[INPUT_TOTAL], float deltaTime)
{
	if (spawnFluidTimer > 0.2)
	{
		if (inputList[INPUT_UNFREEZE_FLUID])
		{
			unfreezeFluid = true;
			fluidGrid->spawnFluid();
			spawnFluidTimer = 0.0;
		}
	}
	else
		spawnFluidTimer += (double)deltaTime;
	if (unfreezeFluid)
		fluidGrid->Update(deltaTime);
}

void EulerianFluidScene::Draw()
{
	glUseProgram(shader->getProgram());
	int mvpHandle = glGetUniformLocation(shader->getProgram(), "uMVPMatrix");

	glm::mat4 mvMat = projMat * viewMat;

	glm::mat4 cursorMat = mvMat * 
		glm::translate(glm::mat4(1.f), glm::vec3(cursorPosX, cursorPosY, 0.f)) * 
		glm::scale(glm::mat4(1.f), glm::vec3(10.f, 10.f, 1.f));

	fluidGrid->Draw(mvpHandle, mvMat);
}

void EulerianFluidScene::windowsResize(int width, int height)
{
}

void EulerianFluidScene::mouseCallback(double xpos, double ypos)
{
	float gridSize = (float)WINDOWS_WIDTH / (float)xCellsCount;
	cursorPosX = (float)xpos / gridSize;
	cursorPosY = ((float)WINDOWS_HEIGHT - (float)ypos) / gridSize;
	cursorCellX = (int)cursorPosX;
	cursorCellY = (int)cursorPosY;
}