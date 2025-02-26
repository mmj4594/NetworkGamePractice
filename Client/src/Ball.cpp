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
	setSpeedRange(BALL_SPEED_RANGE);
	setAcc(glm::vec2(0.f, GRAVITY));
}

void Ball::reset()
{
	__super::reset();
	if (GameMode::Get<GameMode_Local>()->lastRoundWinnerPlayerID == -1)
	{
		std::random_device randomDevice;
		std::mt19937 MTEngine(randomDevice());
		std::uniform_int_distribution<int> randomDistribution(1, 2);

		float ballX = randomDistribution(MTEngine) % 2 == 0 ? INITIAL_BALL_POSITION.x : SCREEN_WIDTH - INITIAL_BALL_POSITION.x;
		setPosition(glm::vec2(ballX, INITIAL_BALL_POSITION.y));
	}
	else
	{
		setPosition(glm::vec2(GameMode::Get<GameMode_Local>()->lastRoundWinnerPlayerID == 1 ? INITIAL_BALL_POSITION.x : SCREEN_WIDTH - INITIAL_BALL_POSITION.x, INITIAL_BALL_POSITION.y));
	}
}
