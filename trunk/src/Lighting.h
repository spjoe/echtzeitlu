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
private:
	PFNGLGENFRAMEBUFFERSPROC my_glGenFramebuffers;
	PFNGLBINDFRAMEBUFFERPROC my_glBindFramebuffer;
	PFNGLFRAMEBUFFERTEXTURE2DPROC my_glFramebufferTexture2D;
	PFNGLDELETEFRAMEBUFFERSPROC my_glDeleteFramebuffers;

	bool isinit;
	GLuint fbo_tex_color;
	unsigned max_lights;
	
	void init();
	
public:
	Lighting();
	~Lighting();
	
	std::vector<Light> lightlist;
	
	GLuint fbo;
	GLuint shadow_map;
	
	void addLight(glm::vec3 position, glm::vec4 color);
	
	void createShadow(SceneObject* scene, std::vector<Shader*> shaders);
	
};

}//ende namespace echtzeitlu
