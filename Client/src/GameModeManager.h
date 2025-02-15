#pragma once

#include "GameMode.h"
#include "GameMode_Lobby.h"
#include "GameMode_Local.h"
#include "GameMode_Online.h"

constexpr int MAX_FPS = 60;
constexpr float FRAME_TIME = 1.0f / MAX_FPS;
constexpr float BASIC_TIME_SCALE = 1.0f;

struct GLFWwindow;

class GameModeManager
{
public:
	static GameModeManager& Get();

public:
	void beginPlay();
	void endPlay();
	void tick(float elapsedTime);
	void renderFrame(float elapsedTime);
	void onKey(GLFWwindow* window, int key, int scancode, int action, int mods);
	void changeGameMode(GameModeType newGameMode);

public:
	GameMode* currentGameMode = nullptr;
	GameModeType currentGameModeType = GameModeType::Lobby;
	float currentTimeScale = BASIC_TIME_SCALE;
};
