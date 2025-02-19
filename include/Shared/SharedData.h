#pragma once

#include <glm/glm.hpp>
#include <cstring>

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

#pragma region System
constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 600;

constexpr int MAX_FPS = 60;
constexpr float FRAME_TIME = 1.0f / MAX_FPS;
constexpr float BASIC_TIME_SCALE = 1.0f;
#pragma endregion
