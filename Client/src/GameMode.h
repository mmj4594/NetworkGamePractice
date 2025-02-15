#pragma once

struct GLFWwindow;

enum class GameModeType
{
	Lobby,
	Local,
	Online
};

class GameMode
{
public:
	template<typename T>
	static T* Get()
	{
		static T* instance;
		if (instance == nullptr)
			instance = new T();
		return instance;
	}

public:
	virtual void beginPlay() = 0;
	virtual void endPlay() = 0;
	virtual void tick(float elapsedTime) = 0;
	virtual void renderFrame(float elapsedTime) = 0;
	virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
};
