#include "Ball.h"

Ball::Ball(glm::vec2 position_, float width_, float height_) : GameObject(position_, width_, height_)
{
	setColor(BALL_COLOR);
}
