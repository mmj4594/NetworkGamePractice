#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"

class Block : public GameObject
{
public:
	Block() {}
	Block(glm::vec2 position_, float width_, float height_);
};
