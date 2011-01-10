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
	
	for(unsigned i=0; i<lightlist.size(); i++){
		if(glIsTexture(lightlist[i].texShadowMap))
			glDeleteTextures(1,&lightlist[i].texShadowMap);
	}
	get_errors("Lighting::~Lighting()");
}

void Lighting::addLight(glm::vec3 position, glm::vec4 color)
{
	if(lightlist.size() >= 4){
		printf("Lighting::addLight() Warning, number of lights limited to 4\n");
		return;
	}
	
	Light light;
	light.position = position;
	light.color = color;
	float aspect = float(width)/float(height);
	
	light.bias = glm::mat4(	0.5, 0.0, 0.0, 0.0, 
							0.0, 0.5, 0.0, 0.0,
							0.0, 0.0, 0.5, 0.0,
							0.5, 0.5, 0.5, 1.0 );
	light.proj = glm::perspective(90.0f, aspect, 0.1f, 100.0f);
	light.view = glm::lookAt(light.position, glm::vec3(0,0,0), glm::vec3(0,0,1));
	
	glGenTextures(1, &light.texShadowMap);
	glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, fbo_res, fbo_res, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	
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
	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	get_errors("Lighting::init() B");
	
	isinit = true;
}

char uniform[64];

void Lighting::createShadow(SceneObject* scene, Shader* shader)
{
	// TODO Render to fbo-texture (pbuffer)
	
	if(lightlist.empty())
		return;
	
	Camera cam_tmp = m_camera_1;
	
	for(unsigned i=0; i<lightlist.size() && i<4; i++){
		
		Light light = lightlist[i];
		
		m_camera_1.intrinsic = light.proj;
		m_camera_1.extrinsic = light.view;

		// Draw Scene into depth buffer
		my_glBindFramebuffer(GL_FRAMEBUFFER, fbo);
		my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.texShadowMap, 0);
		glViewport(0,0,fbo_res,fbo_res);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.1f, 0.1f);

		scene->drawSimple();

		glDisable(GL_POLYGON_OFFSET_FILL);
		get_errors("Lighting::createShadow() B");
		my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,width,height);

		get_errors("Lighting::createShadow() C");

		// Apply light matrices and shadow map to shader
		shader->bind();
			sprintf(uniform, "%s%d", "light_position", i);
			GLint light_position_uniform = shader->get_uniform_location( uniform );
			sprintf(uniform, "%s%d", "light_color", i);
			GLint light_color_uniform    = shader->get_uniform_location( uniform );
			glUniform3fv(light_position_uniform, 1, glm::value_ptr(light.position));
			glUniform4fv(light_color_uniform,    1, glm::value_ptr(light.color));
			get_errors("Lighting::createShadow() E");
			
			glm::mat4 biasprojview = light.bias * light.proj * light.view;
			sprintf(uniform, "%s%d", "shadowProjView", i);
			GLint biasprojview_uniform = shader->get_uniform_location( uniform );
			glUniformMatrix4fv(biasprojview_uniform, 1, GL_FALSE, glm::value_ptr(biasprojview));
			get_errors("Lighting::createShadow() F");
			
			sprintf(uniform, "%s%d", "shadowMap", i);
			GLint shadowMap_uniform = shader->get_uniform_location( uniform );
			glUniform1i(shadowMap_uniform, 1+i);
			glActiveTexture(GL_TEXTURE1 + i);
			glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
			get_errors("Lighting::createShadow() G");
		shader->unbind();
	}
	
	shader->bind();
		GLint ambient_color_uniform  = shader->get_uniform_location( "ambient_color");
		glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
		GLint num_lights_uniform  = shader->get_uniform_location( "num_lights");
		glUniform1i(num_lights_uniform,  (int)lightlist.size());
	shader->unbind();
	
	m_camera_1 = cam_tmp;
}