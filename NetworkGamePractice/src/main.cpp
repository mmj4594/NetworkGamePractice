#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include "graphics.h"
#include "game.h"

int main()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return -1;
	}

	// Set OpenGL version (3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
	glfwSetKeyCallback(window, key_callback);

	// Initialize GLEW
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		glfwTerminate();
		return -1;
	}

	// Set Viewport Size
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	// VAO/VBO/EBO Setting
	unsigned int VAO, VBO, EBO;
	setupBuffers(VAO, VBO, EBO);

	// Shader Compile
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	// Create Shader Program
	unsigned int shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		// Apply 60FPS
		static int frameCount = 0;
		static double fpsTimer = glfwGetTime();
		const double currentTime = glfwGetTime();
		const double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;
		lag += elapsedTime * timeScale;
		while (lag >= FRAME_TIME)
		{
			frameCount++;
			lag -= FRAME_TIME;
			updatePhysics();

			// Clear Screen
			glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Object Rendering
			renderObject(VAO, shaderProgram, player1X, player1Y, 0.2f, 0.2f, 1.0f, 0.0f, 0.0f);
			renderObject(VAO, shaderProgram, player2X, player2Y, 0.2f, 0.2f, 0.0f, 0.0f, 1.0f);
			renderObject(VAO, shaderProgram, ballX, ballY, 0.1f, 0.1f, 1.0f, 1.0f, 0.0f);
			renderObject(VAO, shaderProgram, netX, netY, netWidth, netHeight, 1.0f, 1.0f, 1.0f);

			// Buffer Swap
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		// print FPS
		if (currentTime - fpsTimer >= 1.0)
		{
			printf("FPS: %d\n", frameCount);
			frameCount = 0;
			fpsTimer += 1.0;
		}
	}

	glfwTerminate();
	return 0;
}