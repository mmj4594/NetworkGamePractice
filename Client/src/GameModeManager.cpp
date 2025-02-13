#include <iostream>
#include "GameModeManager.h"

GameModeManager& GameModeManager::Get()
{
	static GameModeManager instance;
	return instance;
}

void GameModeManager::beginPlay()
{
	changeGameMode(GameModeType::Lobby);

	// 임시 코드. 변경 필요.
	changeGameMode(GameModeType::Local);
}

void GameModeManager::endPlay()
{
	if (currentGameMode)
		currentGameMode->endPlay();
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
		currentGameMode = GameMode::Get<GameMode_Lobby>();
		break;
	case GameModeType::Local:
		currentGameMode = GameMode::Get<GameMode_Local>();
		break;
	case GameModeType::Online:
		currentGameMode = GameMode::Get<GameMode_Online>();
		break;
	default:
		std::cerr << "GameModeManager::changeGameMode: Unhandled Game Mode Type!" << std::endl;
	}

	if (currentGameMode)
		currentGameMode->beginPlay();
}
