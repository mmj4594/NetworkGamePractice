#pragma once

#include "SharedData.h"
#include <glm/glm.hpp>
#include "GameMode.h"

constexpr glm::vec3 FPS_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 SCORE_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 READY_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 GAME_SET_TEXT_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 EXCEPTION_TEXT_COLOR = glm::vec3(1.0f, 0.7f, 0.7f);
constexpr glm::vec3 P1_COLOR = glm::vec3(1.0f, 0.3f, 0.3f);
constexpr glm::vec3 P2_COLOR = glm::vec3(0.3f, 0.3f, 1.0f);
constexpr glm::vec3 BALL_COLOR = glm::vec3(1.0f, 1.0f, 0.3f);
constexpr glm::vec3 NET_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);
constexpr glm::vec3 BLOCK_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

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
