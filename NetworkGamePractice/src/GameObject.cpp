#include "GameObject.h"

void GameObject::setSpeed(glm::vec2 newSpeed)
{
	speed = newSpeed;

	// max/min speed
	if (speed.x < minSpeed.x) speed.x = minSpeed.x;
	if (speed.y < minSpeed.y) speed.y = minSpeed.y;
	if (speed.x > maxSpeed.x) speed.x = maxSpeed.x;
	if (speed.y > maxSpeed.y) speed.y = maxSpeed.y;
}

void GameObject::reset()
{
	position = initialPosition;
	glm::vec2 speed = glm::vec2(0.f);
}

void GameObject::updatePosition(float elapsedTime)
{
	position += (speed * elapsedTime);
}

void GameObject::updateSpeed(float elapsedTime)
{
	setSpeed(speed + (acc * elapsedTime));
}