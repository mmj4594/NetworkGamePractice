#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <glm/glm.hpp>
#include <iostream>
#include <cstring>
#include <string>
#include <cstdarg>

#pragma region System
constexpr int SCREEN_WIDTH = 900;
constexpr int SCREEN_HEIGHT = 600;

constexpr int CLIENT_MAX_FPS = 60;
constexpr int SERVER_MAX_FPS = 60;
constexpr float CLIENT_FRAME_TIME = 1.0f / CLIENT_MAX_FPS;
constexpr float SERVER_FRAME_TIME = 1.0f / SERVER_MAX_FPS;
constexpr float BASIC_TIME_SCALE = 1.0f;

constexpr int MAX_BUFFER_SIZE = 1024;

struct LogCategory
{
	LogCategory() {}
	LogCategory(std::string _categoryName) : categoryName(_categoryName) {}

	std::string categoryName;
};
static LogCategory LogNetwork("Network");
enum class LogVerbosity
{
	Log,
	Warning,
	Error,
};
inline void printLog(LogCategory category, LogVerbosity verbosity, const char* format, ...)
{
	// Get Verbose as String
	std::string verbosityString;
	switch (verbosity)
	{
	case LogVerbosity::Log:
		verbosityString = "Log";
		break;
	case LogVerbosity::Warning:
		verbosityString = "Warning";
		break;
	case LogVerbosity::Error:
		verbosityString = "Error";
		break;
	}

	// Handle Format
	char messageBuffer[MAX_BUFFER_SIZE];
	va_list args;
	va_start(args, format);
	vsnprintf(messageBuffer, sizeof(messageBuffer), format, args);
	va_end(args);

	std::string logMessage = category.categoryName + ": " + verbosityString + ": " + messageBuffer + "\n";
	std::cout << logMessage;
}
#define LOG(Category, Verbosity, Format, ...) printLog(Category, Verbosity, Format, ##__VA_ARGS__)
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

constexpr glm::vec2 PLAYER_SIZE = glm::vec2(75.f, 75.f);
constexpr glm::vec2 BALL_SIZE = glm::vec2(60.f, 60.f);

constexpr glm::vec2 INITIAL_PLAYER1_POSITION = glm::vec2(BLOCK_THICKNESS / 2 + 15 + (PLAYER_SIZE.x / 2), BLOCK_THICKNESS / 2 + PLAYER_SIZE.y / 2);
constexpr glm::vec2 INITIAL_PLAYER2_POSITION = glm::vec2(SCREEN_WIDTH - (BLOCK_THICKNESS / 2 + 15 + (PLAYER_SIZE.x / 2)), BLOCK_THICKNESS / 2 + PLAYER_SIZE.y / 2);
constexpr glm::vec2 INITIAL_BALL_POSITION = glm::vec2(BLOCK_THICKNESS / 2 + 55 + (BALL_SIZE.x / 2), SCREEN_HEIGHT - BLOCK_THICKNESS / 2 - BALL_SIZE.y / 2);
constexpr glm::vec2 INITIAL_NET_POSITION = glm::vec2(SCREEN_WIDTH / 2, NET_HEIGHT / 2);
constexpr glm::vec2 INITIAL_LEFT_WALL_POSITION = glm::vec2(0.f, SCREEN_HEIGHT / 2);
constexpr glm::vec2 INITIAL_RIGHT_WALL_POSITION = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT / 2);
constexpr glm::vec2 INITIAL_FLOOR_POSITION = glm::vec2(SCREEN_WIDTH / 2, 0.f);
constexpr glm::vec2 INITIAL_CEIL_POSITION = glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT);

constexpr int MAX_SCORE = 5;
constexpr float GAME_WAIT_TIME = 5.0f;
constexpr float ROUND_END_TIME_SCALE = 0.15f;
constexpr float ROUND_WAIT_TIME = 2.0f;
constexpr float ROUND_END_TIME = 2.0f * ROUND_END_TIME_SCALE;

enum class GameStateType
{
	None,
	Ready,
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
enum class MessageType
{
	None = 0,
	// Client is Connected to Server
	Connected,
	// Client is Disconnected from Server
	Disconnected,
	// Replicate GameState
	ReplicateGameState,
	// Input of Player
	PlayerInput,
};

struct MessageHeader
{
	MessageType type;
	int size;
};

template <typename T>
inline void serialize(const T& data, char* buffer)
{
	std::memcpy(buffer, &data, sizeof(T));
}
template <typename T>
inline void deserialize(const char* buffer, T& data)
{
	std::memcpy(&data, buffer, sizeof(T));
}

struct ConnectMessage
{
	ConnectMessage() {}

	int connectedPlayerID = -1;
};
struct DisconnectMessage
{
};
struct ReplicatedGameState
{
	ReplicatedGameState() {}

	glm::vec2 player1Position = glm::vec2(0);
	glm::vec2 player2Position = glm::vec2(0);
	glm::vec2 ballPosition = glm::vec2(0);
	int scorePlayer1 = 0;
	int scorePlayer2 = 0;
	int lastRoundWinnerPlayerID = -1;
	GameStateType currentGameState = GameStateType::None;
	RoundStateType currentRoundState = RoundStateType::None;
};
struct PlayerInput
{
	PlayerInput() {}
	PlayerInput(int _key, int _scancode, int _action, int _mods) : key(_key), scancode(_scancode), action(_action), mods(_mods) {}

	int key = 0;
	int scancode = 0;
	int action = 0;
	int mods = 0;
};

template <typename T>
inline void sendMessage(SOCKET targetSocket, MessageType type, const T& message)
{
	int dataSize = sizeof(T);
	int totalSize = sizeof(MessageHeader) + dataSize;

	// Fill Header
	char* buffer = new char[totalSize];
	MessageHeader* header = reinterpret_cast<MessageHeader*>(buffer);
	header->type = type;
	header->size = dataSize;

	// Serialize and Send Message
	serialize(message, buffer + sizeof(MessageHeader));
	int sendByte = send(targetSocket, buffer, totalSize, 0);
	delete[] buffer;

	if (sendByte == SOCKET_ERROR)
	{
		int error = WSAGetLastError();
		if (error == WSAECONNRESET || error == WSAENOTCONN)
		{
			LOG(LogNetwork, LogVerbosity::Warning, "sendMessage: Socket has Disconnected");
		}
		else
		{
			LOG(LogNetwork, LogVerbosity::Error, "sendMessage: Failed to send Message! Error: %d", error);
		}
	}
}
#pragma endregion