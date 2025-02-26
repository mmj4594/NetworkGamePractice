#include <iostream>
#include "GameMode_Lobby.h"
#include "GameMode_Local.h"
#include "GameMode_Online.h"
#include "GameModeManager.h"

LogCategory LogGameModeManager("GameModeManager");

GameModeManager& GameModeManager::Get()
{
	static GameModeManager instance;
	return instance;
}

void GameModeManager::beginPlay()
{
	LOG(LogGameModeManager, LogVerbosity::Log, "Game Mode Manger is Started");
	changeGameMode(GameModeType::Lobby);
}

void GameModeManager::endPlay()
{
	if (currentGameMode)
		currentGameMode->endPlay();

	currentGameMode = nullptr;
	LOG(LogGameModeManager, LogVerbosity::Log, "Game Mode Manger is Terminated");
}

void GameModeManager::tick(float elapsedTime)
{
	if (currentGameMode)
		currentGameMode->tick(elapsedTime);
}

void GameModeManager::renderFrame(float elapsedTime)
{
	if (currentGameMode)
		currentGameMode->renderFrame(elapsedTime);
}

void GameModeManager::onKey(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	if (currentGameMode)
		currentGameMode->onKey(window, key, scancode, action, mods);
}

void GameModeManager::changeGameMode(GameModeType newGameMode)
{
	if(currentGameMode)
		currentGameMode->endPlay();

	currentGameModeType = newGameMode;
	switch (newGameMode)
	{
	case GameModeType::Lobby:
		LOG(LogGameModeManager, LogVerbosity::Log, "Change Game Mode to Lobby");
		currentGameMode = GameMode::Get<GameMode_Lobby>();
		break;
	case GameModeType::Local:
		LOG(LogGameModeManager, LogVerbosity::Log, "Change Game Mode to Local");
		currentGameMode = GameMode::Get<GameMode_Local>();
		break;
	case GameModeType::Online:
		LOG(LogGameModeManager, LogVerbosity::Log, "Change Game Mode to Online");
		currentGameMode = GameMode::Get<GameMode_Online>();
		break;
	default:
		LOG(LogGameModeManager, LogVerbosity::Error, "changeGameMode: Unhandled Game Mode Type!");
	}

	if (currentGameMode)
		currentGameMode->beginPlay();
}
