#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"

const glm::vec3 BLOCK_COLOR = glm::vec3(0.0f, 0.0f, 0.0f);

class Block : public GameObject
{
public:
	Block() {}
	Block(glm::vec2 position_, float width_, float height_);
};
