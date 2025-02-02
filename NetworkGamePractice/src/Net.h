#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"

class Net : public GameObject
{
public:
	Net() {}
	Net(glm::vec2 position_, float width_, float height_);
};

