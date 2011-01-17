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
	
	GLuint shadow_fbo;
	GLuint shadow_map;
	
	GLuint light_fbo;
	GLuint light_map;
	
	std::vector<Light> lightlist;
	
	
	void init();

	float totaltime;
	
public:
	Lighting();
	~Lighting();
	
	
	void addLight(glm::vec3 position, glm::vec4 color);
	
	void changePosition(unsigned id, glm::vec3 position);
	
	void createShadow(SceneObject* scene, std::vector<Shader*> shaders);
	
	void createLight(std::vector<Shader*> shaders);

	void update(float dTime);
	
};

}//ende namespace echtzeitlu
