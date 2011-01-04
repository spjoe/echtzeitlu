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
	unsigned int color;
	int energy;
	float size;
};
}//ende namespace echtzeitlu
