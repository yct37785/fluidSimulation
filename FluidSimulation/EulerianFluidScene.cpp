#include "EulerianFluidScene.h"

EulerianFluidScene::EulerianFluidScene()
{
}

EulerianFluidScene::~EulerianFluidScene()
{
	delete shader;
	delete fluidGrid;
}

void EulerianFluidScene::Init()
{
	xCellsCount = 30;
	float accurateYSpaceHeight = (float)xCellsCount * ((float)WINDOWS_HEIGHT / (float)WINDOWS_WIDTH);
	yCellsCount = (int)accurateYSpaceHeight + 1;
	viewMat = glm::lookAt(glm::vec3(0.f, 0.f, 3.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f));
	projMat = glm::frustum(-H, (float)xCellsCount * H + H * 2, -H, accurateYSpaceHeight * H + H * 2, 3.f, 7.f);
	shader = new Shader("../Shaders/vertexshader.cpp", "../Shaders/fragmentshader.cpp");
	fluidGrid = new Eulerian_FluidGrid(xCellsCount, yCellsCount);
}

void EulerianFluidScene::Update(bool inputList[INPUT_TOTAL], float deltaTime)
{
	if (spawnParticlesTimer > 0.2)
	{
		if (inputList[INPUT_UNFREEZE_FLUID])
		{
			fluidGrid->spawnParticles();
			spawnParticlesTimer = 0.0;
		}
	}
	else
		spawnParticlesTimer += (double)deltaTime;
	fluidGrid->Update(deltaTime);
}

void EulerianFluidScene::Draw()
{
	glUseProgram(shader->getProgram());
	int mvpHandle = glGetUniformLocation(shader->getProgram(), "uMVPMatrix");

	glm::mat4 mvMat = projMat * viewMat;

	fluidGrid->Draw(mvMat, mvpHandle);
}

void EulerianFluidScene::windowsResize(int width, int height)
{
}

void EulerianFluidScene::mouseCallback(double xpos, double ypos)
{
}
