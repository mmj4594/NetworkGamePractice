#include "Ball.h"
#include "GameState_Local.h"

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
	if (GameState::Get<GameState_Local>().lastRoundWinnerPlayerID == -1)
	{
		setPosition(glm::vec2((rand() % 2 == 0 ? getInitialPosition().x : SCREEN_WIDTH - getInitialPosition().x), getInitialPosition().y));
	}
	else
	{
		setPosition(glm::vec2(GameState::Get<GameState_Local>().lastRoundWinnerPlayerID == 1 ? getInitialPosition().x : SCREEN_WIDTH - getInitialPosition().x, getInitialPosition().y));
	}
}
