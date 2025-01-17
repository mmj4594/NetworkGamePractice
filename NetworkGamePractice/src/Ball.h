#pragma once
#include "GameObject.h"
#include <glm/glm.hpp>

const glm::vec3 BALL_COLOR = glm::vec3(1.0f, 1.0f, 0.0f);

class Ball : public GameObject
{
public:
	Ball() {}
	Ball(glm::vec2 position_, float width_, float height_);
	void beginPlay() override;
};

