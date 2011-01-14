/**
* @file Particle.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date Januar 2010
* @version unstable
* @brief One Particle in a Particle System
* @namespace echtzeitlu
*/

#pragma once
#include "glm/glm.hpp"


namespace echtzeitlu{
struct Particle
{
	glm::vec4 position; //the current Position of the Particle in Particle System Space
	glm::vec4 oldPos; //the last Position of the Particle in Particle System Space
	glm::vec4 velocity; //the current speed of the Particle
	glm::vec4 color; //the current color of the Particle
	glm::vec4 g; //the current acceleration of the particle
	float energy; //A Value that describse how long the particle will survive
	float size;// The size of the Particle
	bool alive;// Is the Particle alive (draw) or not.
};
}//ende namespace echtzeitlu
