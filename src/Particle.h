#pragma once
#include "glm/glm.hpp"

class Particle
{
private:
	glm::vec3 position;
	glm::vec3 oldPos;
	glm::vec3 velocity;
	unsigned int color;
	int energy;
	float size;
};
