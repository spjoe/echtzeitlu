#include "Lighting.h"
#include "shader.hpp"
#include "camera.h"

#include <stdio.h>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/type_ptr.hpp> 

using namespace echtzeitlu;

extern glm::vec4 light_color;
extern glm::vec4 ambient_color;
extern Camera m_camera_1;
extern int width;
extern int height;
extern Shader *defaultColorShader;
extern Shader *simpleShader;

int fbo_res = 1024;

Lighting::Lighting()
{
	isinit = false;
	init();
}

Lighting::~Lighting()
{
	PFNGLISFRAMEBUFFERPROC my_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)glfwGetProcAddress("glIsFramebuffer");
	PFNGLDELETEFRAMEBUFFERSPROC my_glDeleteFramebuffer = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffer");
	
	if(my_glIsFramebuffer(fbo))
		my_glDeleteFramebuffers(1, &fbo);
}

void Lighting::addLight(glm::vec3 position, glm::vec4 color)
{
	Light light;
	light.position = position;
	light.color = color;
	float aspect = float(width)/float(height);
	
	light.bias = glm::mat4(	0.5, 0.0, 0.0, 0.0, 
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0 );
	light.proj = glm::perspective(60.0f, aspect, 0.1f, 100.0f);
	light.view = glm::lookAt(light.position, glm::vec3(0,0,0), glm::vec3(0,0,1));
	
	glGenTextures(1, &light.texShadowMap);
	glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, fbo_res, fbo_res, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	
	lightlist.push_back(light);
}

void Lighting::init()
{
	my_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
	my_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
	my_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2D");
	my_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");

	glGenTextures(1, &fbo_tex_color);
	glBindTexture(GL_TEXTURE_2D, fbo_tex_color);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo_res, fbo_res, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	get_errors("Lighting::init() A");
	
	my_glGenFramebuffers(1,&fbo);
	my_glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex_color, 0);
// 	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.texShadowMap, 0);
	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	get_errors("Lighting::init() B");
	
	isinit = true;
}

void Lighting::createShadow(SceneObject* scene, Shader* shader)
{
	// TODO Render to fbo-texture (pbuffer)
	
	if(lightlist.empty())
		return;
	
	Light light = lightlist[0]; // HACK assume one light for testing
	
	Camera cam_tmp = m_camera_1;
	
	m_camera_1.intrinsic = light.proj;
	m_camera_1.extrinsic = light.view;
	
	my_glBindFramebuffer(GL_FRAMEBUFFER, fbo);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.texShadowMap, 0);
	glViewport(0,0,fbo_res,fbo_res);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	scene->drawSimple();
	get_errors("Lighting::createShadow() B");
	
	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glViewport(0,0,width,height);

	get_errors("Lighting::createShadow() C");
	
	
	shader->bind();
		GLint light_position_uniform = shader->get_uniform_location( "light_position");
		GLint light_color_uniform    = shader->get_uniform_location( "light_color");
		GLint ambient_color_uniform  = shader->get_uniform_location( "ambient_color");
		get_errors("Lighting::createShadow() D");
		glUniform3fv(light_position_uniform, 1, glm::value_ptr(light.position));
		glUniform4fv(light_color_uniform,    1, glm::value_ptr(light_color));
		glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
		get_errors("Lighting::createShadow() E");
		
		glm::mat4 biasprojview = light.bias * light.proj * light.view;
		GLint biasprojview_uniform = shader->get_uniform_location("shadowProjView");
		glUniformMatrix4fv(biasprojview_uniform, 1, GL_FALSE, glm::value_ptr(biasprojview));
		get_errors("Lighting::createShadow() F");
		
		GLint shadowMap_uniform = shader->get_uniform_location("shadowMap");
		glUniform1i(shadowMap_uniform, 1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
		get_errors("Lighting::createShadow() G");
	shader->unbind();
	
	
	
	m_camera_1 = cam_tmp;
	
}