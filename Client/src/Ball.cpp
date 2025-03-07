#include "SharedData.h"
#include <random>
#include "Ball.h"
#include "GameMode_Local.h"

LogCategory LogBall("Ball");

Ball::Ball(glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	setColor(BALL_COLOR);
}

void Ball::beginPlay()
{
	setSpeedRange(Config::Get().BALL_SPEED_RANGE);
	setAcc(glm::vec2(0.f, Config::Get().GRAVITY));
}

void Ball::reset()
{
	__super::reset();
	if (GameMode::Get<GameMode_Local>()->lastRoundWinnerPlayerID == -1)
	{
		std::random_device randomDevice;
		std::mt19937 MTEngine(randomDevice());
		std::uniform_int_distribution<int> randomDistribution(1, 2);

		float ballX = randomDistribution(MTEngine) % 2 == 0 ? Config::Get().INITIAL_BALL_POSITION.x : Config::Get().SCREEN_WIDTH - Config::Get().INITIAL_BALL_POSITION.x;
		setPosition(glm::vec2(ballX, Config::Get().INITIAL_BALL_POSITION.y));
	}
	else
	{
		setPosition(glm::vec2(GameMode::Get<GameMode_Local>()->lastRoundWinnerPlayerID == 1 ? Config::Get().INITIAL_BALL_POSITION.x : Config::Get().SCREEN_WIDTH - Config::Get().INITIAL_BALL_POSITION.x, Config::Get().INITIAL_BALL_POSITION.y));
	}
}
