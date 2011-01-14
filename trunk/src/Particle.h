#pragma once
#include "glm/glm.hpp"


namespace echtzeitlu{
struct tShape
{
	glm::vec4 vertex[4];
};
struct Particle
{
	glm::vec4 position;
	glm::vec4 oldPos;
	glm::vec4 velocity;
	glm::vec4 color;
	glm::vec4 g;
	float energy;
	float size;
	bool alive;
};
}//ende namespace echtzeitlu
