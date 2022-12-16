#include "Engine.h"

//void framebuffer_size_callback(GLFWwindow* window, int width, int height)
//{
//    glViewport(0, 0, width, height);
//}

int main()
{
    Engine::instance() ->Init();
    Engine::instance()->Run();
    Engine::instance()->Exit();

    // display memory leak report
    _CrtDumpMemoryLeaks();
    return 0;

    //// working 100% basic rendering for quick debugging
    //glfwInit();
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    //glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //GLFWwindow* window = glfwCreateWindow(800, 600, "LearnOpenGL", NULL, NULL);
    //if (window == NULL)
    //{
    //    std::cout << "Failed to create GLFW window" << std::endl;
    //    glfwTerminate();
    //    return -1;
    //}
    //glfwMakeContextCurrent(window);

    //if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    //{
    //    std::cout << "Failed to initialize GLAD" << std::endl;
    //    return -1;
    //}

    //glViewport(0, 0, 800, 600);

    //glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //Shader* shader = new Shader("../Shaders/vertexshader.cpp", "../Shaders/fragmentshader.cpp");

    //Mesh* mesh = MeshBuilder::CreateMesh("quad");

    //while (!glfwWindowShouldClose(window))
    //{
    //    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    //    glClear(GL_COLOR_BUFFER_BIT);

    //    glUseProgram(shader->getProgram());

    //    glm::mat4 mvMat = glm::mat4(1.f);
    //    int mvpHandle = glGetUniformLocation(shader->getProgram(), "uMVPMatrix");
    //    glUniformMatrix4fv(mvpHandle, 1, GL_FALSE, glm::value_ptr(mvMat));

    //    glBindVertexArray(mesh->getVAO());
    //    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->getEBO());
    //    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    //    glfwSwapBuffers(window);
    //    glfwPollEvents();
    //}

    //glfwTerminate();
    //return 0;
}
