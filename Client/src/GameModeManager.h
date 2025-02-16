#pragma once

#include <glm/glm.hpp>
#include "GameMode.h"

constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 600;

constexpr int MAX_FPS = 60;
constexpr float FRAME_TIME = 1.0f / MAX_FPS;
constexpr float BASIC_TIME_SCALE = 1.0f;

constexpr glm::vec3 FPS_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

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
	GameModeType currentGameModeType = GameModeType::None;
	float currentTimeScale = BASIC_TIME_SCALE;
};
