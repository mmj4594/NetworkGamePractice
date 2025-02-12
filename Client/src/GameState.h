#pragma once

struct GLFWwindow;

class GameState
{
public:
	template<typename T>
	static T* Get()
	{
		static T instance;
		return dynamic_cast<T*>(&instance);
	}
public:
	virtual void beginPlay() = 0;
	virtual void tick(float elapsedTime) = 0;
	virtual void onKey(GLFWwindow* window, int key, int scancode, int action, int mods) = 0;
};
