#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Graphics.h"
#include "Game.h"

std::string getCurrentExeDir()
{
	// change the working directory to the location where .exe file is located.
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	std::string exePath = buffer;
	size_t pos = exePath.find_last_of("\\/");
	std::string exeDir = exePath.substr(0, pos);
	return exeDir;
}

int main()
{
	FT_Library ft;
	if (FT_Init_FreeType(&ft))
	{
		std::cerr << "ERROR::FREETYPE: Could not initialize FreeType Library" << std::endl;
		return -1;
	}

	char buffer[MAX_PATH];
	GetCurrentDirectoryA(MAX_PATH, buffer);
	std::cout << "Current Working Directory: " << buffer << std::endl;

	FT_Face face;
	std::string fontPath = getCurrentExeDir() + "/fonts/arial.ttf";
	if (FT_New_Face(ft, fontPath.c_str(), 0, &face))
	{
		std::cerr << "ERROR::FREETYPE: Failed to load font" << std::endl;
	}

	std::cout << "FreeType successfully initialized and font loaded!" << std::endl;

	FT_Done_Face(face);
	FT_Done_FreeType(ft);

	if (Graphics::Get().initializeGraphics() == false)
	{
		std::cerr << "Failed to initialize Graphics!" << std::endl;
		return -1;
	}

	Game::Get().beginPlay();
	Game::Get().resetRound();

	// Main loop
	while (!Graphics::Get().shouldClose())
	{
		static int fpsCount = 0;
		static double fpsTimer = 0.0;
		static double tickTimer = 0.0;
		static double renderTimer = 0.0;

		static double previousTime = 0.0;
		const double currentTime = glfwGetTime();
		const double elapsedTime = currentTime - previousTime;
		previousTime = currentTime;

		fpsTimer += elapsedTime;
		tickTimer += (elapsedTime * TIME_SCALE);
		renderTimer += elapsedTime;

		while (tickTimer >= FRAME_TIME)
		{
			Game::Get().tick(static_cast<float>(tickTimer));
			tickTimer -= FRAME_TIME;
		}

		while (renderTimer >= FRAME_TIME)
		{
			Graphics::Get().renderFrame(renderTimer);
			fpsCount++;
			renderTimer -= FRAME_TIME;
		}

		while (fpsTimer >= 1.0)
		{
			printf("FPS: %d\n", fpsCount);
			fpsCount = 0;
			fpsTimer -= 1.0;
		}
	}

	glfwTerminate();
	return 0;
}