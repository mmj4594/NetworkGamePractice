#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Graphics.h"
#include "GameModeManager.h"

LogCategory LogGraphics("Graphics");

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glfwSetWindowSize(window, SCREEN_WIDTH, SCREEN_HEIGHT);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	GameModeManager::Get().onKey(window, key, scancode, action, mods);
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
		LOG(LogGraphics, LogVerbosity::Error, "initializeGraphics: Failed to initialize GLFW!");
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
		LOG(LogGraphics, LogVerbosity::Error, "initializeGraphics: Failed to create GLFW window!");
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
		LOG(LogGraphics, LogVerbosity::Error, "initializeGraphics: Failed to initialize GLEW!");
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

	// Load Shader (object)
	std::string objectVertexShaderPath = getCurrentExeDir() + "\\shaders\\ObjectVertexShader.glsl";
	std::string objectFragmentShaderPath = getCurrentExeDir() + "\\shaders\\ObjectFragmentShader.glsl";
	objectShaderProgram = loadShader(objectVertexShaderPath.c_str(), objectFragmentShaderPath.c_str());
	objectTransformLoc = glGetUniformLocation(objectShaderProgram, "transform");
	objectProjectionLoc = glGetUniformLocation(objectShaderProgram, "projection");
	objectColorLoc = glGetUniformLocation(objectShaderProgram, "objectColor");

	// Load Shader (text)
	std::string textVertexShaderPath = getCurrentExeDir() + "\\shaders\\TextVertexShader.glsl";
	std::string textFragmentShaderPath = getCurrentExeDir() + "\\shaders\\TextFragmentShader.glsl";
	textShaderProgram = loadShader(textVertexShaderPath.c_str(), textFragmentShaderPath.c_str());
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

unsigned int Graphics::loadShader(const char* vertexPath, const char* fragmentPath)
{
	std::ifstream vertexShaderFile;
	std::ifstream fragmentShaderFile;
	vertexShaderFile.open(vertexPath);
	fragmentShaderFile.open(fragmentPath);

	std::stringstream vertexShaderStream, fragmentShaderStream;
	vertexShaderStream << vertexShaderFile.rdbuf();
	fragmentShaderStream << fragmentShaderFile.rdbuf();

	vertexShaderFile.close();
	fragmentShaderFile.close();

	std::string vertexShaderCode = vertexShaderStream.str();
	std::string fragmentShaderCode = fragmentShaderStream.str();
	unsigned int vertexShader = compileShader(GL_VERTEX_SHADER, vertexShaderCode.c_str());
	unsigned int fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderCode.c_str());

	// Create Shader Program
	unsigned int resultShaderProgram = glCreateProgram();
	glAttachShader(resultShaderProgram, vertexShader);
	glAttachShader(resultShaderProgram, fragmentShader);
	glLinkProgram(resultShaderProgram);
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);

	return resultShaderProgram;
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

		if (type == GL_VERTEX_SHADER)
			LOG(LogGraphics, LogVerbosity::Error, "compileShader: Vertex shader compilation failed!");
		else
			LOG(LogGraphics, LogVerbosity::Error, "compileShader: Fragment shader compilation failed!");
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
		LOG(LogGraphics, LogVerbosity::Error, "loadFont: Failed to Initialize FreeType!");
		return false;
	}

	FT_Face face;
	if (FT_New_Face(ft, fontPath, 0, &face))
	{
		LOG(LogGraphics, LogVerbosity::Error, "loadFont: Failed to Load Font File!");
		return false;
	}

	FT_Set_Pixel_Sizes(face, 0, TEXT_SIZE);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	for (unsigned char c = 0; c < 128; c++)
	{
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			LOG(LogGraphics, LogVerbosity::Error, "loadFont: Failed to load Glyph: %c", c);
			continue;
		}

		unsigned int texture;
		glGenTextures(1, &texture);
		if (texture == 0)
		{
			LOG(LogGraphics, LogVerbosity::Error, "loadFont: Failed to generate texture for character: %c", c);
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

void Graphics::renderFrame(float elapsedTime)
{
	// Clear Screen
	glClearColor(BACKGROUND_COLOR.x, BACKGROUND_COLOR.y, BACKGROUND_COLOR.z, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (GameModeManager::Get().currentGameMode)
		GameModeManager::Get().currentGameMode->renderFrame(elapsedTime);

	// Buffer Swap
	glfwSwapBuffers(window);
	glfwPollEvents();
}

void Graphics::renderObject(GameObject targetObject)
{
	if (objectShaderProgram == 0 || objectVAO == 0)
	{
		LOG(LogGraphics, LogVerbosity::Error, "renderObject: Shader program or VAO is not initialized!");
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

void Graphics::renderText(std::string text, float x, float y, float scale, glm::vec3 color, bool centerAligned)
{
	if (textShaderProgram == 0 || textVAO == 0)
	{
		LOG(LogGraphics, LogVerbosity::Error, "renderText: Shader program or VAO is not initialized!");
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

	if (centerAligned)
	{
		float textWidth = 0.f;
		for (char c : text)
		{
			if (FontCharacters.find(c) != FontCharacters.end())
			{
				textWidth += (FontCharacters[c].Advance >> 6) * scale;
			}
		}

		x -= textWidth / 2.0f;
	}

	for (char c : text)
	{
		if (FontCharacters.find(c) == FontCharacters.end())
		{
			LOG(LogGraphics, LogVerbosity::Error, "renderText: Cannot find %c in FontCharacters!", c);
			continue;
		}

		FontCharacter fontCharacter = FontCharacters[c];
		if (fontCharacter.TextureID == 0)
		{
			LOG(LogGraphics, LogVerbosity::Error, "renderText: Texture not generated for character %c", c);
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
