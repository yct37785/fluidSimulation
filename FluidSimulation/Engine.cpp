#include "Engine.h"
#include <STB/stb_image.h>
Engine* Engine::engine = NULL;
GLFWwindow* Engine::window = NULL;
// insert scenes here
#include "Scene_EulerianFluid.h"
#include "Scene_SpatialHashing.h"

void Engine::initGlfw()
{
	glfwInit();
	// set OpenGL ver
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	// core profile: excudes backwards-compatible features we no longer need
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	//glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

int Engine::createWindow()
{
	window = glfwCreateWindow(WINDOWS_WIDTH, WINDOWS_HEIGHT, "LearnOpenGL", NULL, NULL);
	if (window == NULL)
	{
		LogWarn("Failed to create GLFW window");
		terminateOpenGL();
		return 0;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);	// register callback for on windows resize event
	return 1;
}

int Engine::initGlad()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		LogWarn("Failed to initialize GLAD");
		return 0;
	}
	return 1;
}

void Engine::createViewport()
{
	glViewport(0, 0, WINDOWS_WIDTH, WINDOWS_HEIGHT);
}

void Engine::initInput()
{
	//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	glfwSetCursorPosCallback(window, mouse_callback);
}

void Engine::initOpenGLSettings()
{
	// opengl rendering configurations
	// depth ordering on OpenGL
	glEnable(GL_DEPTH_TEST);
	// alpha blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	// cull face (discard unseen faces, save 50% on fragment shader runs)
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glFrontFace(GL_CW);
}

void Engine::renderLoop()
{
	// OSEngine vars
	lastFrame = glfwGetTime();

	while (!glfwWindowShouldClose(window))
	{
		// delta time
		float currFrame = glfwGetTime();
		deltaTime = currFrame - lastFrame;
		lastFrame = currFrame;

		// input
		processInput(window);

		// engine update
		scene->Update(inputList, deltaTime);
		// draw
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		scene->Draw();

		// read: double buffers to prevent flickering issues due to physical constraints of drawing a buffer to screen
		// resulting in flickering
		glfwSwapBuffers(window);

		// check if events triggered (inputs etc), then updates windows state and activate callbacks
		glfwPollEvents();
	}
}

int Engine::terminateOpenGL()
{
	glfwDestroyWindow(window);
	glfwTerminate();
	delete Engine::instance();
	return 1;
}

void Engine::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
	//Engine::instance()->onWindowSizeUpdate(width, height);
	engine->scene->windowsResize(width, height);
}

void Engine::processInput(GLFWwindow* window)
{
	// reset all keys
	for (int i = 0; i < INPUT_TOTAL; ++i)
	{
		inputList[i] = false;
	}

	// set input
	if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
		inputList[INPUT_UNFREEZE_FLUID] = true;
	} if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
		inputList[INPUT_UNFREEZE_FLUID_2] = true;
	} if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		inputList[INPUT_FWD] = true;
	} if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		inputList[INPUT_BWD] = true;
	} if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		inputList[INPUT_LEFT] = true;
	} if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		inputList[INPUT_RIGHT] = true;
	} if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
		inputList[INPUT_UP] = true;
	} if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
		inputList[INPUT_DW] = true;
	} if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
		inputList[INPUT_ENABLE_WIREFRAME] = true;
	} if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		inputList[INPUT_DISABLE_WIREFRAME] = true;
	} if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
		inputList[INPUT_QUIT] = true;
	}

	// process inputs
	if (inputList[INPUT_QUIT])
	{
		glfwSetWindowShouldClose(window, true);
	}
}

void Engine::mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	engine->scene->mouseCallback(xpos, ypos);
}

Engine::Engine()
{
}

Engine::~Engine()
{
}

Engine* Engine::instance()
{
	if (!engine)
	{
		engine = new Engine();
	}
	return engine;
}

void Engine::Init()
{
	srand(time(0));
	stbi_set_flip_vertically_on_load(true);
	initGlfw();
	createWindow();
	initGlad();
	createViewport();
	initInput();
	initOpenGLSettings();
	LogInfo("OSEngine initialized");
	deltaTime = lastFrame = 0.f;
	for (int i = 0; i < INPUT_TOTAL; ++i)
	{
		inputList[i] = false;
	}
	scene = new Scene_EulerianFluid();
	scene->Init();
}

void Engine::Run()
{
	renderLoop();
}

void Engine::Exit()
{
	delete scene;
	terminateOpenGL();
}