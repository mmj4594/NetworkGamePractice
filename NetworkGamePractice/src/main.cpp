#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int VERTEX_SIZE = 3;

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		std::cout << "Pressed Down!\n";
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

// Vertex Data (Square)
float squareVertices[] = {
	// x, y, z
	-0.5f, -0.5f, 0.0f,  // Bottom-left
	 0.5f, -0.5f, 0.0f,  // Bottom-right
	 0.5f,  0.5f, 0.0f,  // Top-right
	-0.5f,  0.5f, 0.0f   // Top-left
};
unsigned int squareIndices[] = {
	0, 1, 2, // Triangle 1
	2, 3, 0  // Triangle 2
};

void setupBuffers(unsigned int& VAO, unsigned int& VBO, unsigned int& EBO)
{
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, VERTEX_SIZE, GL_FLOAT, GL_FALSE, VERTEX_SIZE * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

int main()
{
	// Initialize GLFW
	if (!glfwInit()) {
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

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
		glfwPollEvents();

		// Clear Screen
		glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		// VAO Test
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		// Buffer Swap
		glfwSwapBuffers(window);
	}

	glfwTerminate();
	return 0;
}