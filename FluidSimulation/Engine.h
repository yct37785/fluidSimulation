#pragma once
#include "FluidScene.h"

class Engine {
	FluidScene* fluidScene;

	GLFWwindow* window;

	bool inputList[INPUT_TOTAL];
	float deltaTime, lastFrame;

	// init
	void initGlfw();
	int createWindow();
	int initGlad();
	void createViewport();
	void initInput();
	void initOpenGLSettings();

	// render loop
	void renderLoop();

	// exit
	int terminateOpenGL();

	// event handling
	static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
	void processInput(GLFWwindow* window);
	static void mouse_callback(GLFWwindow* window, double xpos, double ypos);

public:
	Engine();
	~Engine();

	void Init();
	void Run();
	void Exit();
};