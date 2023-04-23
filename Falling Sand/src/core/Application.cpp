#include "Application.h"
#include <iostream>
#include <sstream>
#include <random>
#include <vector>

#include <vertex/IndexBuffer.h>
#include <vertex/VertexArray.h>
#include <vertex/VertexBuffer.h>
#include <vertex/VertexBufferLayout.h>

#include <core/Window.h>
#include <core/ParticleBuffer.h>

// static variable definitions
std::unique_ptr<Application> Application::s_Instance = nullptr;
GLFWwindow* Application::p_Window = nullptr;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

// information for quad that we render the fractal onto
float vertices[] = {
		 1.0f, 1.0f, 1.0f, 1.0f,   // top right
		 1.0f, -1.0f, 1.0f, 0.0f,  // bottom right
		 -1.0f, -1.0f, 0.0f, 0.0f,   // bottom left
		 -1.0f, 1.0f, 0.0f, 1.0f  // top left 
};
unsigned int indices[] = {  // note that we start from 0!
	0, 1, 3,  // first Triangle
	1, 2, 3   // second Triangle
};

Application::Application()
{
    
}

void Application::Run()
{
	srand(static_cast<unsigned int> (time(NULL)));

	if (!glfwInit()) {
		std::cout << "Failed to intialise GLFW! Aborting..." << std::endl;
		std::exit(-1);
	}

	// initialise window
	// ----------------
	Window::Init("Falling Sand", SCREEN_WIDTH, SCREEN_HEIGHT);
	p_Window = Window::GetWindow();

	glfwSetFramebufferSizeCallback(p_Window, framebuffer_size_callback);

	// initialise GLAD
	// ---------------
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialise GLAD! Aborting..." << std::endl;
		std::exit(-1);
	}

	// create buffers for rendering the quad
	VertexBufferLayout layout;
	layout.AddAttribute<float>(2);
	layout.AddAttribute<float>(2);

	VertexBuffer VBO(vertices, sizeof(vertices));
	IndexBuffer EBO(indices, sizeof(indices));

	VertexArray VAO;
	VAO.AddBuffer(VBO, layout);

	VAO.Bind();
	EBO.Bind();

	Shader shader("res/shaders/default.shader");
	shader.Bind();	
	
	glEnable(GL_TEXTURE_2D);
	// The texture we're going to render to
	unsigned int texture;
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
	// set the texture wrapping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	// set texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCREEN_WIDTH, SCREEN_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, particle_buffer.pixels);
	
	// application loop
	// ---------------
	while (!glfwWindowShouldClose(p_Window)) {
		InputHandle();

		// render
		// ------
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		particle_buffer.UpdateParticles();
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, GL_RGBA, GL_UNSIGNED_BYTE, particle_buffer.pixels);

		// draw quad to render too - main framebuffer
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// glfw: swap buffers and poll IO events (key presses, mouse interactions etc.)
		// -------------------------------------------------------------------------------
		glfwSwapBuffers(p_Window);
		glfwPollEvents();
	}
	glfwTerminate();

}

void Application::InputHandle()
{
	
	if (glfwGetKey(p_Window, GLFW_KEY_0) == GLFW_PRESS) {
		double xpos, ypos;
		glfwGetCursorPos(p_Window, &xpos, &ypos);

		int width, height;
		glfwGetWindowSize(p_Window, &width, &height);

		int x = (xpos / width) * SCREEN_WIDTH;
		int y = ((height - ypos) / height) * SCREEN_HEIGHT;

		particle_buffer.CreateParticle(x, y);
	}
}

std::unique_ptr<Application>& Application::GetInstance() {
    if (Application::s_Instance == nullptr) {
        Application::s_Instance = std::unique_ptr<Application>(new Application);
    }
    return Application::s_Instance;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	// make sure the viewport matches the new window dimensions; note that width and 
	// height will be significantly larger than specified on retina displays.
	glViewport(0, 0, width, height);
}