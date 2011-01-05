#pragma once
#include "glm/glm.hpp"
#include "SceneObject.h"
#include "common.hpp"
#include <vector>

namespace echtzeitlu{

struct Light
{
	glm::vec3 position;
	glm::vec4 color;
	glm::mat4 proj;
	glm::mat4 model;
	GLuint texShadowMap;
};

class Lighting
{
private:
	std::vector<Light> lightlist;
	
public:
	void addLight(glm::vec3 position, glm::vec4 color);
	
	void createShadowMaps(SceneObject* scene);
	
};

}//ende namespace echtzeitlu
