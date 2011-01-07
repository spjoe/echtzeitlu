#pragma once
#include "glm/glm.hpp"
#include "SceneObject.h"
#include "shader.hpp"
#include "common.hpp"
#include <vector>

namespace echtzeitlu{

struct Light
{
	glm::vec3 position;
	glm::vec4 color;
	glm::mat4 bias;
	glm::mat4 proj;
	glm::mat4 view;
	GLuint texShadowMap;
};

class Lighting
{
public:
	std::vector<Light> lightlist;
	
	void addLight(glm::vec3 position, glm::vec4 color);
	
	void createShadow(SceneObject* scene, Shader* shader);
	
};

}//ende namespace echtzeitlu
