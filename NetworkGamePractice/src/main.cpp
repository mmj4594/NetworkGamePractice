#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>

constexpr int SCREEN_WIDTH = 800;
constexpr int SCREEN_HEIGHT = 600;
constexpr int VERTEX_SIZE = 3;

float player1X = -0.8f, player1Y = -0.8f, player1SpeedY = 0.0f;
float player2X = 0.8f, player2Y = -0.8f, player2SpeedY = 0.0f;
float ballX = 0.0f, ballY = 0.0f, ballSpeedX = 0.01f, ballSpeedY = 0.02f;
int scorePlayer1 = 0, scorePlayer2 = 0;
constexpr float netX = 0.0f, netY = -0.8f, netWidth = 0.05f, netHeight = 1.0f;

constexpr float GRAVITY = -0.0005f;
constexpr float JUMP_SPEED = 0.02f;
bool player1Jumping = false, player2Jumping = false;

void resetRound()
{
	ballX = 0.0f;
	ballY = 0.0f;
	ballSpeedX = 0.01f * (rand() % 2 == 0 ? 1 : -1);
	ballSpeedY = 0.02f;
	player1X = -0.8f;
	player1Y = -0.8f;
	player2X = 0.8f;
	player2Y = -0.8f;
	player1Jumping = player2Jumping = false;
	printf("Current Score: [%d : %d]\n", scorePlayer1, scorePlayer2);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_W && action == GLFW_PRESS && !player1Jumping)
	{
		player1SpeedY = JUMP_SPEED;
		player1Jumping = true;
	}
	if (key == GLFW_KEY_UP && action == GLFW_PRESS && !player2Jumping)
	{
		player2SpeedY = JUMP_SPEED;
		player2Jumping = true;
	}

	if (key == GLFW_KEY_A && action == GLFW_PRESS) player1X -= 0.02f;
	if (key == GLFW_KEY_D && action == GLFW_PRESS) player1X += 0.02f;
	if (key == GLFW_KEY_LEFT && action == GLFW_PRESS) player2X -= 0.02f;
	if (key == GLFW_KEY_RIGHT && action == GLFW_PRESS) player2X += 0.02f;
}

bool checkCollision(float obj1X, float obj1Y, float obj1Width, float obj1Height, float obj2X, float obj2Y, float obj2Width, float obj2Height)
{
	return obj1X < obj2X + obj2Width
		&& obj1X + obj1Width > obj2X
		&& obj1Y < obj2Y + obj2Height
		&& obj1Y + obj1Height > obj2Y;
}

void updatePhysics()
{
	// player1 Jump
	if (player1Jumping)
	{
		player1Y += player1SpeedY;
		player1SpeedY += GRAVITY;
		if (player1Y <= -0.8f)
		{
			player1Y = -0.8f;
			player1Jumping = false;
		}
	}
	// player2 Jump
	if (player2Jumping)
	{
		player2Y += player2SpeedY;
		player2SpeedY += GRAVITY;
		if (player2Y <= -0.8f)
		{
			player2Y = -0.8f;
			player2Jumping = false;
		}
	}

	// ball physics
	ballX += ballSpeedX;
	ballY += ballSpeedY;
	ballSpeedY += GRAVITY;

	// ball - floor
	if (ballY <= -1.0f)
	{
		if (ballX < 0.0f)
		{
			scorePlayer2++;
		}
		else
		{
			scorePlayer1++;
		}
		resetRound();
	}

	// ball - net
	if (checkCollision(ballX, ballY, 0.1f, 0.1f, netX - netWidth / 2, netY, netWidth, netHeight))
	{
		ballSpeedX = -ballSpeedX;
	}

	// ball - player
	if (checkCollision(ballX, ballY, 0.1f, 0.1f, player1X, player1Y, 0.2f, 0.2f))
	{
		ballSpeedY = 0.015f;
		ballSpeedX = 0.01f;
	}
	if (checkCollision(ballX, ballY, 0.1f, 0.1f, player2X, player2Y, 0.2f, 0.2f))
	{
		ballSpeedY = 0.015f;
		ballSpeedX = -0.01f;
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

// Shader
const char* vertexShaderSource =
R"(
	#version 330 core
	layout(location = 0) in vec3 aPos;
	uniform mat4 transform;

	void main()
	{
		gl_Position = transform * vec4(aPos, 1.0);
	}
)";
const char* fragmentShaderSource =
R"(
	#version 330 core
	out vec4 FragColor;
	uniform vec4 color;

	void main()
	{
		FragColor = color;
	}
)";

unsigned int compileShader(unsigned int type, const char* source)
{
	unsigned int shader = glCreateShader(type);
	glShaderSource(shader, 1, &source, nullptr);
	glCompileShader(shader);

	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char infoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, infoLog);
		std::cerr << (type == GL_VERTEX_SHADER ? "Vertex" : "Fragment") << " shader compilation failed\n" << infoLog << std::endl;
	}
	return shader;
}

void renderObject(unsigned int VAO, unsigned int shaderProgram, float x, float y, float width, float height, float r, float g, float b)
{
	glUseProgram(shaderProgram);

	// set color
	int colorLoc = glGetUniformLocation(shaderProgram, "color");
	glUniform4f(colorLoc, r, g, b, 1.0f);

	// transform
	int transformLoc = glGetUniformLocation(shaderProgram, "transform");
	float transform[16] = {
		width, 0.0f,  0.0f, 0.0f,
		0.0f,  height, 0.0f, 0.0f,
		0.0f,  0.0f,  1.0f, 0.0f,
		x,     y,     0.0f, 1.0f
	};
	glUniformMatrix4fv(transformLoc, 1, GL_FALSE, transform);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

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

	static int frameCount = 0;
	static double fpsTimer = glfwGetTime();

	// Main loop
	while (!glfwWindowShouldClose(window))
	{
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

	glfwTerminate();
	return 0;
}