#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>
#include <windows.h>
#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <cstring>
#include <string>
#include <cstdarg>
#include "nlohmann/json.hpp"

#pragma region System
constexpr int MAX_BUFFER_SIZE = 1024;

struct LogCategory
{
	LogCategory() {}
	LogCategory(std::string _categoryName) : categoryName(_categoryName) {}

	std::string categoryName;
};
static LogCategory LogNetwork("Network");
static LogCategory LogConfig("Config");
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

#pragma region Config
class Config
{
public:
	static Config& Get()
	{
		static Config instance;
		return instance;
	}

public:
#pragma region Config-System
	int SCREEN_WIDTH = 900;
	int SCREEN_HEIGHT = 600;

	int CLIENT_MAX_FPS = 60;
	int SERVER_MAX_FPS = 60;
	float CLIENT_FRAME_TIME = 1.0f / CLIENT_MAX_FPS;
	float SERVER_FRAME_TIME = 1.0f / SERVER_MAX_FPS;
#pragma endregion

#pragma region Config-Game
	int MAX_SCORE = 5;
	float BASIC_TIME_SCALE = 1.0f;
	float GAME_WAIT_TIME = 5.0f * BASIC_TIME_SCALE;
	float ROUND_END_TIME_SCALE = BASIC_TIME_SCALE * 0.15f;
	float ROUND_WAIT_TIME = 2.0f * BASIC_TIME_SCALE;
	float ROUND_END_TIME = 2.0f * ROUND_END_TIME_SCALE;

	float GRAVITY = -1000.f;
	glm::vec2 BALL_SPEED_RANGE = glm::vec2(-2000.f, 2000.f);
	float BALL_SPIKE_SPEED = 500.f;
	glm::vec2 PLAYER_SPEED_RANGE = glm::vec2(-400.f, 400.f);
	glm::vec2 PLAYER_SLIDING_SPEED_RANGE = glm::vec2(-1000.f, 1000.f);
	float PLYAER_JUMP_SPEED = 750.0f;
	float PLAYER_SLIDING_SPEED = 870.f;
	float PLAYER_SLIDING_DURATION = 0.7f;
	float PLAYER_FRICTION = 2000.f;
	float PLAYER_MOVE_ACC = 1000.f;
	float NORMAL_COLLISION_IMPACT_FACTOR = 9.3f;

	int NET_WIDTH = 6;
	int NET_HEIGHT = 200;
	int BLOCK_THICKNESS = 30;

	glm::vec2 PLAYER_SIZE = glm::vec2(75.f, 75.f);
	glm::vec2 BALL_SIZE = glm::vec2(60.f, 60.f);

	glm::vec2 INITIAL_PLAYER1_POSITION = glm::vec2(BLOCK_THICKNESS / 2 + 15 + (PLAYER_SIZE.x / 2), BLOCK_THICKNESS / 2 + PLAYER_SIZE.y / 2);
	glm::vec2 INITIAL_PLAYER2_POSITION = glm::vec2(SCREEN_WIDTH - (BLOCK_THICKNESS / 2 + 15 + (PLAYER_SIZE.x / 2)), BLOCK_THICKNESS / 2 + PLAYER_SIZE.y / 2);
	glm::vec2 INITIAL_BALL_POSITION = glm::vec2(BLOCK_THICKNESS / 2 + 55 + (BALL_SIZE.x / 2), SCREEN_HEIGHT - BLOCK_THICKNESS / 2 - BALL_SIZE.y / 2);
	glm::vec2 INITIAL_NET_POSITION = glm::vec2(SCREEN_WIDTH / 2, NET_HEIGHT / 2);
	glm::vec2 INITIAL_LEFT_WALL_POSITION = glm::vec2(0.f, SCREEN_HEIGHT / 2);
	glm::vec2 INITIAL_RIGHT_WALL_POSITION = glm::vec2(SCREEN_WIDTH, SCREEN_HEIGHT / 2);
	glm::vec2 INITIAL_FLOOR_POSITION = glm::vec2(SCREEN_WIDTH / 2, 0.f);
	glm::vec2 INITIAL_CEIL_POSITION = glm::vec2(SCREEN_WIDTH / 2, SCREEN_HEIGHT);
#pragma endregion

#pragma region Config-Network
	std::string SERVERHOST;
	int SERVERPORT = 9000;
#pragma endregion
};

inline std::string getCurrentExeDir()
{
	// change the working directory to the location where .exe file is located.
	char buffer[MAX_PATH];
	GetModuleFileNameA(NULL, buffer, MAX_PATH);

	std::string exePath = buffer;
	size_t pos = exePath.find_last_of("\\/");
	std::string exeDir = exePath.substr(0, pos);
	return exeDir;
}

inline bool loadConfig(const std::string& fileName)
{
	std::string filePath = getCurrentExeDir() + "\\" + fileName;
	std::ifstream file(filePath);
	if (!file.is_open())
	{
		LOG(LogConfig, LogVerbosity::Error, "Failed to open Config File. Path: %s", (filePath).c_str());
		return false;
	}

	nlohmann::json configJson;
	file >> configJson;

	// Read values in .config file.
	try
	{
		Config::Get().SCREEN_WIDTH = configJson["system"]["screenWidth"];
		Config::Get().SCREEN_HEIGHT = configJson["system"]["screenHeight"];
		Config::Get().CLIENT_MAX_FPS = configJson["system"]["clientMaxFPS"];

		Config::Get().MAX_SCORE = configJson["game"]["maxScore"];
		Config::Get().BASIC_TIME_SCALE = configJson["game"]["timeScale"];
		Config::Get().GRAVITY = configJson["game"]["gravity"];
		float playerSize = configJson["game"]["playerSize"];
		Config::Get().PLAYER_SIZE = glm::vec2(playerSize);
		float ballSize = configJson["game"]["ballSize"];
		Config::Get().BALL_SIZE = glm::vec2(ballSize);

		Config::Get().SERVERHOST = configJson["network"]["serverHost"];
		Config::Get().SERVERPORT = configJson["network"]["serverPort"];
	}
	catch (const nlohmann::json::exception& e)
	{
		LOG(LogConfig, LogVerbosity::Error, "json Parsing Error: %s", e.what());
		return false;
	}

	// Calculate another config values
	{
		Config::Get().CLIENT_FRAME_TIME = 1.0f / Config::Get().CLIENT_MAX_FPS;
		Config::Get().SERVER_FRAME_TIME = 1.0f / Config::Get().SERVER_MAX_FPS;

		Config::Get().GAME_WAIT_TIME = 5.0f * Config::Get().BASIC_TIME_SCALE;
		Config::Get().ROUND_END_TIME_SCALE = Config::Get().BASIC_TIME_SCALE * 0.15f;
		Config::Get().ROUND_WAIT_TIME = 2.0f * Config::Get().BASIC_TIME_SCALE;
		Config::Get().ROUND_END_TIME = 2.0f * Config::Get().ROUND_END_TIME_SCALE;
		Config::Get().INITIAL_PLAYER1_POSITION = glm::vec2(Config::Get().BLOCK_THICKNESS / 2 + 15 + (Config::Get().PLAYER_SIZE.x / 2), Config::Get().BLOCK_THICKNESS / 2 + Config::Get().PLAYER_SIZE.y / 2);
		Config::Get().INITIAL_PLAYER2_POSITION = glm::vec2(Config::Get().SCREEN_WIDTH - (Config::Get().BLOCK_THICKNESS / 2 + 15 + (Config::Get().PLAYER_SIZE.x / 2)), Config::Get().BLOCK_THICKNESS / 2 + Config::Get().PLAYER_SIZE.y / 2);
		Config::Get().INITIAL_BALL_POSITION = glm::vec2(Config::Get().BLOCK_THICKNESS / 2 + 55 + (Config::Get().BALL_SIZE.x / 2), Config::Get().SCREEN_HEIGHT - Config::Get().BLOCK_THICKNESS / 2 - Config::Get().BALL_SIZE.y / 2);
		Config::Get().INITIAL_NET_POSITION = glm::vec2(Config::Get().SCREEN_WIDTH / 2, Config::Get().NET_HEIGHT / 2);
		Config::Get().INITIAL_LEFT_WALL_POSITION = glm::vec2(0.f, Config::Get().SCREEN_HEIGHT / 2);
		Config::Get().INITIAL_RIGHT_WALL_POSITION = glm::vec2(Config::Get().SCREEN_WIDTH, Config::Get().SCREEN_HEIGHT / 2);
		Config::Get().INITIAL_FLOOR_POSITION = glm::vec2(Config::Get().SCREEN_WIDTH / 2, 0.f);
		Config::Get().INITIAL_CEIL_POSITION = glm::vec2(Config::Get().SCREEN_WIDTH / 2, Config::Get().SCREEN_HEIGHT);
	}

	return true;
}

#pragma endregion