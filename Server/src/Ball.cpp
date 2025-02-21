#include "SharedData.h"
#include "Ball.h"
#include "Game.h"

Ball::Ball(glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
}

void Ball::beginPlay()
{
	setSpeedRange(BALL_SPEED_RANGE);
	setAcc(glm::vec2(0.f, GRAVITY));
}

void Ball::reset()
{
	__super::reset();
	if (Game::Get().lastRoundWinnerPlayerID == -1)
	{
		setPosition(glm::vec2((rand() % 2 == 0 ? INITIAL_BALL_POSITION.x : SCREEN_WIDTH - INITIAL_BALL_POSITION.x), INITIAL_BALL_POSITION.y));
	}
	else
	{
		setPosition(glm::vec2(Game::Get().lastRoundWinnerPlayerID == 1 ? INITIAL_BALL_POSITION.x : SCREEN_WIDTH - INITIAL_BALL_POSITION.x, INITIAL_BALL_POSITION.y));
	}
}
