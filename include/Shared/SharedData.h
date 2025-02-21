#pragma once

#include <glm/glm.hpp>
#include <cstring>

#pragma region System
constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 600;

constexpr int CLIENT_MAX_FPS = 60;
constexpr int SERVER_MAX_FPS = 30;
constexpr float CLIENT_FRAME_TIME = 1.0f / CLIENT_MAX_FPS;
constexpr float SERVER_FRAME_TIME = 1.0f / SERVER_MAX_FPS;
constexpr float BASIC_TIME_SCALE = 1.0f;
#pragma endregion

#pragma region Game
constexpr float GRAVITY = -1000.f;
constexpr glm::vec2 BALL_SPEED_RANGE = glm::vec2(-2000.f, 2000.f);
constexpr float BALL_SPIKE_SPEED = 500.f;
constexpr glm::vec2 PLAYER_SPEED_RANGE = glm::vec2(-400.f, 400.f);
constexpr glm::vec2 PLAYER_SLIDING_SPEED_RANGE = glm::vec2(-1000.f, 1000.f);
constexpr float PLYAER_JUMP_SPEED = 750.0f;
constexpr float PLAYER_SLIDING_SPEED = 870.f;
constexpr float PLAYER_SLIDING_DURATION = 0.7f;
constexpr float PLAYER_FRICTION = 2000.f;
constexpr float PLAYER_MOVE_ACC = 1000.f;
constexpr float NORMAL_COLLISION_IMPACT_FACTOR = 9.3f;

constexpr float NET_WIDTH = 6.f;
constexpr float NET_HEIGHT = 200.f;
constexpr float BLOCK_THICKNESS = 30.f;

constexpr glm::vec2 INITIAL_PLAYER1_POSITION = glm::vec2(70.f, BLOCK_THICKNESS / 2 + 37.5f);
constexpr glm::vec2 INITIAL_PLAYER2_POSITION = glm::vec2(SCREEN_WIDTH - 70.f, BLOCK_THICKNESS / 2 + 37.5f);
constexpr glm::vec2 INITIAL_BALL_POSITION = glm::vec2(100.f, SCREEN_HEIGHT - BLOCK_THICKNESS / 2 - 30.f);
constexpr glm::vec2 INITIAL_NET_POSITION = glm::vec2(SCREEN_WIDTH / 2, NET_HEIGHT / 2);
constexpr glm::vec2 INITIAL_LEFT_WALL_POSITION = glm::vec2(0.f, SCREEN_HEIGHT / 2);
constexpr glm::vec2 INITIAL_RIGHT_WALL_POSITION = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT / 2);
constexpr glm::vec2 INITIAL_FLOOR_POSITION = glm::vec2(SCREEN_WIDTH / 2, 0.f);
constexpr glm::vec2 INITIAL_CEIL_POSITION = glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT);

constexpr int MAX_SCORE = 15;
constexpr float ROUND_END_TIME_SCALE = 0.15f;
constexpr float ROUND_WAIT_TIME = 2.0f;
constexpr float ROUND_END_TIME = 2.0f * ROUND_END_TIME_SCALE;

enum class GameStateType
{
	None,
	Playing,
	End,
};
enum class RoundStateType
{
	None,
	Ready,
	Playing,
	End,
};

enum class SpikeDirectionType
{
	None,
	Front,
	Up,
	Down,
	Front_Up,
	Front_Down,
};
#pragma endregion

#pragma region Network
constexpr int BUFFER_SIZE = 1024;

struct PlayerInput
{
	PlayerInput() {}
	PlayerInput(int _key, int _scancode, int _action, int _mods) : key(_key), scancode(_scancode), action(_action), mods(_mods) {}

	int key = 0;
	int scancode = 0;
	int action = 0;
	int mods = 0;
};
inline void serialize(const PlayerInput& data, char* buffer)
{
	std::memcpy(buffer, &data, sizeof(PlayerInput));
}
inline void deserialize(const char* buffer, PlayerInput& data)
{
	std::memcpy(&data, buffer, sizeof(PlayerInput));
}

struct ReplicatedGameState
{
	ReplicatedGameState() {}

	glm::vec2 player1Position = glm::vec2(0);
	glm::vec2 player2Position = glm::vec2(0);
	glm::vec2 ballPosition = glm::vec2(0);
	int scorePlayer1 = 0;
	int scorePlayer2 = 0;
	GameStateType currentGameState = GameStateType::None;
	RoundStateType currentRoundState = RoundStateType::None;
};
inline void serialize(const ReplicatedGameState& data, char* buffer)
{
	std::memcpy(buffer, &data, sizeof(ReplicatedGameState));
}
inline void deserialize(const char* buffer, ReplicatedGameState& data)
{
	std::memcpy(&data, buffer, sizeof(ReplicatedGameState));
}
#pragma endregion