#include "GameObject.h"

void GameObject::setPosition(glm::vec2 newPosition)
{
	position = newPosition;
}

void GameObject::reset()
{
	position = initialPosition;
	glm::vec2 speed = glm::vec2(0.f);
}
