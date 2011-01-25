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
	glm::mat4 tview;
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
	GLuint light_vao_id;
	GLuint light_vbo_id;
	glm::vec4 light_verts[8];
	unsigned light_idxs[12];
	
	GLuint gauss_vao_id;
	GLuint gauss_vbo_id[2];
	glm::vec4 gauss_verts[4];
	glm::vec2 gauss_texs[4];
	unsigned gauss_idxs[6];
	
	std::vector<Light> lightlist;
	
	
	void init();

	float totaltime;
	
public:
	Lighting();
	~Lighting();
	
	
	void addLight(glm::vec3 position, glm::vec4 color);
	
	void changePosition(unsigned id, glm::vec3 position);
	
	void Render();
	
	void createShadow(SceneObject* scene, std::vector<Shader*> shaders);
	
	void createLightMap(SceneObject* scene);
	
	void addLightMap(GLuint tex1);

	void update(float dTime);
	
};

}//ende namespace echtzeitlu
