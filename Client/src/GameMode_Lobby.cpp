#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <string>
#include "GameMode_Lobby.h"
#include "GameModeManager.h"
#include "Graphics.h"

void GameMode_Lobby::beginPlay()
{
	selectedGameMode = GameModeType::Local;
}

void GameMode_Lobby::endPlay()
{
}

void GameMode_Lobby::tick(float elapsedTime)
{
}

void GameMode_Lobby::renderFrame(float elapsedTime)
{
	// Text Rendering
	std::ostringstream fpsString;
	fpsString << std::fixed << std::setprecision(1) << (1.0f / elapsedTime);
	Graphics::Get().renderText((fpsString.str() + " FPS").c_str(), 20.f, 570.f, 0.25f, FPS_TEXT_COLOR);

	selectedGameMode == GameModeType::Local
		? Graphics::Get().renderText("> Local Mode <", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 25.f, SELECTED_GAMEMODE_SIZE, SELECTED_GAMEMODE_COLOR, true)
		: Graphics::Get().renderText("Local Mode", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 + 25.f, UNSELECTED_GAMEMODE_SIZE, UNSELECTED_GAMEMODE_COLOR, true);
	selectedGameMode == GameModeType::Online
		? Graphics::Get().renderText("> Online Mode <", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 25.f, SELECTED_GAMEMODE_SIZE, SELECTED_GAMEMODE_COLOR, true)
		: Graphics::Get().renderText("Online Mode", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 25.f, UNSELECTED_GAMEMODE_SIZE, UNSELECTED_GAMEMODE_COLOR, true);
	Graphics::Get().renderText("Press Up/Down to move, Enter to choose.", SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2 - 100.f, 0.5f, UNSELECTED_GAMEMODE_COLOR, true);
}

void GameMode_Lobby::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	std::cout << key << " " << action << std::endl;
	if (key == GLFW_KEY_ENTER && action == GLFW_PRESS)
	{
		GameModeManager::Get().changeGameMode(selectedGameMode);
	}
	else if (key == GLFW_KEY_UP && action == GLFW_PRESS)
	{
		switch (selectedGameMode)
		{
		case GameModeType::Online:
			selectedGameMode = GameModeType::Local;
			break;
		}
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS)
	{
		switch (selectedGameMode)
		{
		case GameModeType::Local:
			selectedGameMode = GameModeType::Online;
			break;
		}
	}
}
