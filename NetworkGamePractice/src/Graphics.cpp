#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Graphics.h"
#include "Game.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glfwSetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Game::Get().onKey(window, key, scancode, action, mods);
}

Graphics& Graphics::Get()
{
	static Graphics instance;
	return instance;
}

bool Graphics::initializeGraphics()
{
	// Initialize GLFW
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return false;
	}

	// Set OpenGL version (3.3 Core Profile)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create a window
	glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "OpenGL Window", nullptr, nullptr);
	if (!window)
	{
		std::cerr << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return false;
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
		return false;
	}

	// Set Viewport Size
	glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT, -1.0f, 1.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	// Allow 2D Texture
	glEnable(GL_TEXTURE_2D);

	// Allow alpha blending
	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// load font
	std::string fontPath = getCurrentExeDir() + "\\fonts\\arial.ttf";
	if (!loadFont(fontPath.c_str()))
	{
		return false;
	}

	// VAO/VBO/EBO Setting	
	setupObjectBuffers();
	setupTextBuffers();

	// Shader Compile (object)
	const char* objectVertexShaderSource =
	R"(
		#version 330 core
		layout(location = 0) in vec3 aPos;
		uniform mat4 transform;
		uniform mat4 projection;

		void main()
		{
			gl_Position = projection * transform * vec4(aPos, 1.0);
		}
	)";
	const char* objectFragmentShaderSource =
	R"(
		#version 330 core
		out vec4 FragColor;
		uniform vec3 objectColor;

		void main()
		{
			FragColor = vec4(objectColor, 1.0);
		}
	)";
	unsigned int objectVertexShader = compileShader(GL_VERTEX_SHADER, objectVertexShaderSource);
	unsigned int objectFragmentShader = compileShader(GL_FRAGMENT_SHADER, objectFragmentShaderSource);

	// Create Shader Program (object)
	objectShaderProgram = glCreateProgram();
	glAttachShader(objectShaderProgram, objectVertexShader);
	glAttachShader(objectShaderProgram, objectFragmentShader);
	glLinkProgram(objectShaderProgram);
	glDeleteShader(objectVertexShader);
	glDeleteShader(objectFragmentShader);

	objectTransformLoc = glGetUniformLocation(objectShaderProgram, "transform");
	objectProjectionLoc = glGetUniformLocation(objectShaderProgram, "projection");
	objectColorLoc = glGetUniformLocation(objectShaderProgram, "objectColor");

	// Shader Compile (text)
	const char* textVertexShaderSource =
	R"(
		#version 330 core
		layout(location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
		out vec2 TexCoords;

		uniform mat4 transform;
		uniform mat4 projection;

		void main()
		{
			gl_Position = projection * transform * vec4(vertex.xy, 0.0, 1.0);
			TexCoords = vertex.zw;
		}
	)";
	const char* textFragmentShaderSource =
	R"(
		#version 330 core
		in vec2 TexCoords;
		out vec4 FragColor;

		uniform sampler2D text;
		uniform vec3 textColor;

		void main()
		{
			vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
			FragColor = vec4(textColor, 1.0) * sampled;
		}
	)";
	unsigned int textVertexShader = compileShader(GL_VERTEX_SHADER, textVertexShaderSource);
	unsigned int textFragmentShader = compileShader(GL_FRAGMENT_SHADER, textFragmentShaderSource);

	// Create Shader Program (text)
	textShaderProgram = glCreateProgram();
	glAttachShader(textShaderProgram, textVertexShader);
	glAttachShader(textShaderProgram, textFragmentShader);
	glLinkProgram(textShaderProgram);
	glDeleteShader(textVertexShader);
	glDeleteShader(textFragmentShader);

	textTransformLoc = glGetUniformLocation(textShaderProgram, "transform");
	textProjectionLoc = glGetUniformLocation(textShaderProgram, "projection");
	textColorLoc = glGetUniformLocation(textShaderProgram, "textColor");

	return true;
}

void Graphics::setupObjectBuffers()
{
	// Vertex Data (Square)
	constexpr float squareVertices[] = {
		// x, y, z
		-0.5f, -0.5f, 0.0f,  // Bottom-left
		 0.5f, -0.5f, 0.0f,  // Bottom-right
		 0.5f,  0.5f, 0.0f,  // Top-right
		-0.5f,  0.5f, 0.0f   // Top-left
	};
	constexpr unsigned int squareIndices[] = {
		0, 1, 2, // Triangle 1
		2, 3, 0  // Triangle 2
	};

	glGenVertexArrays(1, &objectVAO);
	glBindVertexArray(objectVAO);

	glGenBuffers(1, &objectVBO);
	glBindBuffer(GL_ARRAY_BUFFER, objectVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(squareVertices), squareVertices, GL_STATIC_DRAW);

	glGenBuffers(1, &objectEBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, objectEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(squareIndices), squareIndices, GL_STATIC_DRAW);

	// aPos
	glVertexAttribPointer(0, APOS_SIZE, GL_FLOAT, GL_FALSE, APOS_SIZE * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// aTexCoords
	glVertexAttribPointer(1, ATEXCOORDS_SIZE, GL_FLOAT, GL_FALSE, ATEXCOORDS_SIZE * sizeof(float), (void*)(APOS_SIZE * sizeof(float)));
	glEnableVertexAttribArray(1);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Graphics::setupTextBuffers()
{
	glGenVertexArrays(1, &textVAO);
	glBindVertexArray(textVAO);

	glGenBuffers(1, &textVBO);
	glBindBuffer(GL_ARRAY_BUFFER, textVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	// vertex
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

	// Unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

unsigned int Graphics::compileShader(unsigned int type, const char* source)
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

std::string Graphics::getCurrentExeDir()
{
	// change the working directory to the location where .exe file is located.
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	std::string exePath = buffer;
	size_t pos = exePath.find_last_of("\\/");
	std::string exeDir = exePath.substr(0, pos);
	return exeDir;
}

bool Graphics::loadFont(const char* fontPath)
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "Graphics::loadFont: Failed to Initialize FreeType!" << std::endl;
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontPath, 0, &face))
	{
		std::cerr << "Graphics::loadFont: Failed to Load Font File!" << std::endl;
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, 48);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cerr << "Graphics::loadFont: Failed to load Glyph: " << c << std::endl;
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		if (texture == 0)
		{
			std::cerr << "Graphics::loadFont: Failed to generate texture for character: " << c << std::endl;
			continue;
		}
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, face->glyph->bitmap.width, face->glyph->bitmap.rows, 0, GL_RED, GL_UNSIGNED_BYTE, face->glyph->bitmap.buffer);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		FontCharacter character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			static_cast<unsigned int>(face->glyph->advance.x)
		};
		FontCharacters.insert(std::pair<char, FontCharacter>(c, character));
	}
	glBindTexture(GL_TEXTURE_2D, 0);

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
	return true;
}

bool Graphics::shouldClose()
{
	return glfwWindowShouldClose(window);
}

void Graphics::renderFrame(double elapsedTime)
{
	// Clear Screen
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// Object Rendering
	renderObject(Game::Get().player1);
	renderObject(Game::Get().player2);
	renderObject(Game::Get().ball);
	renderObject(Game::Get().net);
	renderObject(Game::Get().leftWall);
	renderObject(Game::Get().rightWall);
	renderObject(Game::Get().floor);
	renderObject(Game::Get().ceil);

	// Text Rendering
	renderText("Hello World", 100.f, 100.f, 1.f, glm::vec3(1.0f, 1.0f, 1.0f));

	// Buffer Swap
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Graphics::renderObject(GameObject targetObject)
{
	if (objectShaderProgram == 0 || objectVAO == 0)
	{
		std::cerr << "Graphics::renderObject: Shader program or VAO is not initialized\n";
		return;
	}

	glUseProgram(objectShaderProgram);

	// set color
	glUniform3f(objectColorLoc, targetObject.getColor().r, targetObject.getColor().g, targetObject.getColor().b);

	// transform
	glm::mat4 transform = glm::mat4(1.0f);
	transform = glm::translate(transform, glm::vec3(targetObject.getPosition(), 0.0f));
	transform = glm::scale(transform, glm::vec3(targetObject.getWidth(), targetObject.getHeight(), 1.0f));
	glUniformMatrix4fv(objectTransformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// projection
	glm::mat4 projection = glm::ortho(0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT);
	glUniformMatrix4fv(objectProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glBindVertexArray(objectVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void Graphics::renderText(std::string text, float x, float y, float scale, glm::vec3 color)
{
	if (textShaderProgram == 0 || textVAO == 0)
	{
		std::cerr << "Graphics::renderText: Shader program or VAO is not initialized\n";
		return;
	}

	glUseProgram(textShaderProgram);

	// set color
	glUniform3f(textColorLoc, color.x, color.y, color.z);

	// transform
	glm::mat4 transform = glm::mat4(1.0f);
	glUniformMatrix4fv(textTransformLoc, 1, GL_FALSE, glm::value_ptr(transform));

	// projection
	glm::mat4 projection = glm::ortho(0.f, (float)SCREEN_WIDTH, 0.f, (float)SCREEN_HEIGHT);
	glUniformMatrix4fv(textProjectionLoc, 1, GL_FALSE, glm::value_ptr(projection));

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(textVAO);

	for (char c : text)
	{
		if (FontCharacters.find(c) == FontCharacters.end())
		{
			std::cerr << "Graphics::renderText: Cannot find " << c << " in FontCharacters!" << std::endl;
			continue;
		}

		FontCharacter fontCharacter = FontCharacters[c];
		if (fontCharacter.TextureID == 0)
		{
			std::cerr << "Graphics::renderText: Texture not generated for character " << c << std::endl;
			continue;
		}

		float xpos = x + fontCharacter.Bearing.x * scale;
		float ypos = y - (fontCharacter.Size.y - fontCharacter.Bearing.y) * scale;
		float w = fontCharacter.Size.x * scale;
		float h = fontCharacter.Size.y * scale;

		float vertices[6][4] = {
			{ xpos, ypos + h, 0.0f, 0.0f },
			{ xpos, ypos, 0.0f, 1.0f },
			{ xpos + w, ypos, 1.0f, 1.0f },

			{ xpos, ypos + h, 0.0f, 0.0f },
			{ xpos + w, ypos, 1.0f, 1.0f },
			{ xpos + w, ypos + h, 1.0f, 0.0f }
		};

		glBindTexture(GL_TEXTURE_2D, fontCharacter.TextureID);
		glBindBuffer(GL_ARRAY_BUFFER, textVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		x += (fontCharacter.Advance >> 6) * scale;
	}

	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}
