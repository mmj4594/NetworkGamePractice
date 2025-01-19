#pragma once

#include <glm/glm.hpp>
#include "GameObject.h"

const glm::vec3 NET_COLOR = glm::vec3(1.0f, 1.0f, 1.0f);

class Net : public GameObject
{
public:
	Net() {}
	Net(glm::vec2 position_, float width_, float height_);
};

