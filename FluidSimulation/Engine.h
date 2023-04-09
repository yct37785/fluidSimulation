#pragma once
#include "Scene.h"

class Engine {
	static Engine* engine;
	static GLFWwindow* window;

	Scene* scene;

	bool inputList[INPUT_TOTAL];
	float deltaTime, lastFrame;

	// init
	Engine();
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
	~Engine();
	static Engine* instance();

	void Init();
	void Run();
	void Exit();
};