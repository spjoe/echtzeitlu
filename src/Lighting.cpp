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
	max_lights = 4;
	totaltime = 0.0f;
	init();
}

Lighting::~Lighting()
{
	PFNGLISFRAMEBUFFERPROC my_glIsFramebuffer = (PFNGLISFRAMEBUFFERPROC)glfwGetProcAddress("glIsFramebuffer");
	PFNGLDELETEFRAMEBUFFERSPROC my_glDeleteFramebuffer = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffer");
	
	if(my_glIsFramebuffer(shadow_fbo))
		my_glDeleteFramebuffers(1, &shadow_fbo);
	
	for(unsigned i=0; i<lightlist.size(); i++){
		if(glIsTexture(lightlist[i].texShadowMap))
			glDeleteTextures(1,&lightlist[i].texShadowMap);
	}
	get_errors("Lighting::~Lighting()");
}

void Lighting::addLight(glm::vec3 position, glm::vec4 color)
{
	if(lightlist.size() >= max_lights){
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
	light.proj = glm::perspective(130.0f, aspect, 0.1f, 100.0f);
	light.view = glm::lookAt(light.position, glm::vec3(0,0,0), glm::vec3(0,0,1));
	
	glGenTextures(1, &light.texShadowMap);
	glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, fbo_res, fbo_res, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	get_errors("Lighting::addLight()");
	
	lightlist.push_back(light);
}

void Lighting::changePosition(unsigned id, glm::vec3 position){
	
	if(id < lightlist.size()){
		lightlist[id].position = position;
		lightlist[id].view = glm::lookAt(lightlist[id].position, glm::vec3(0,0,0), glm::vec3(0,0,1));
	}
}

void Lighting::init()
{
	my_glGenFramebuffers = (PFNGLGENFRAMEBUFFERSPROC)glfwGetProcAddress("glGenFramebuffers");
	my_glBindFramebuffer = (PFNGLBINDFRAMEBUFFERPROC)glfwGetProcAddress("glBindFramebuffer");
	my_glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DPROC)glfwGetProcAddress("glFramebufferTexture2D");
	my_glDeleteFramebuffers = (PFNGLDELETEFRAMEBUFFERSPROC)glfwGetProcAddress("glDeleteFramebuffers");

	glGenTextures(1, &fbo_tex_color);
	glBindTexture(GL_TEXTURE_2D, fbo_tex_color);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, fbo_res, fbo_res, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	get_errors("Lighting::init() A");
	
	my_glGenFramebuffers(1,&shadow_fbo);
	my_glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fbo_tex_color, 0);
	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	get_errors("Lighting::init() B");
	
	glGenTextures(1, &light_map);
	glBindTexture(GL_TEXTURE_2D, light_map);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	get_errors("Lighting::init() C");
	
	my_glGenFramebuffers(1,&light_fbo);
	my_glBindFramebuffer(GL_FRAMEBUFFER, light_fbo);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, light_map, 0);
	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	get_errors("Lighting::init() B");
	
	isinit = true;
}

char uniform_name[64];

void Lighting::createShadow(SceneObject* scene, std::vector<Shader*> shaders)
{
	// TODO Render to fbo-texture (pbuffer)
	
	if(lightlist.empty())
		return;
	
	Camera cam_tmp = m_camera_1;
	
	for(unsigned i=0; i<lightlist.size() && i<max_lights; i++){
		
		Light light = lightlist[i];
		
		// Draw Scene from light point of view into depth buffer
		m_camera_1.intrinsic = light.proj;
		m_camera_1.extrinsic = light.view;
		
		my_glBindFramebuffer(GL_FRAMEBUFFER, shadow_fbo);
		my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light.texShadowMap, 0);
		glViewport(0,0,fbo_res,fbo_res);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glEnable(GL_POLYGON_OFFSET_FILL);
		glPolygonOffset(0.8f, 0.8f);

			scene->drawSimple();

		glDisable(GL_POLYGON_OFFSET_FILL);
		my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glViewport(0,0,width,height);
		get_errors("Lighting::createShadow() C");

		// Apply light matrices and shadow map to shader
		for(std::vector<Shader*>::iterator iter = shaders.begin();
			iter != shaders.end();iter++){
			Shader * shader = *iter;
			shader->bind();
				sprintf(uniform_name, "%s%d", "light_position", i);
				GLint light_position_uniform = shader->get_uniform_location( uniform_name );
				sprintf(uniform_name, "%s%d", "light_color", i);
				GLint light_color_uniform    = shader->get_uniform_location( uniform_name );
				glUniform3fv(light_position_uniform, 1, glm::value_ptr(light.position));
				glUniform4fv(light_color_uniform,    1, glm::value_ptr(light.color));
				get_errors("Lighting::createShadow() E");
			
				glm::mat4 biasprojview = light.bias * light.proj * light.view;
				sprintf(uniform_name, "%s%d", "shadowProjView", i);
				GLint biasprojview_uniform = shader->get_uniform_location( uniform_name );
				glUniformMatrix4fv(biasprojview_uniform, 1, GL_FALSE, glm::value_ptr(biasprojview));
				get_errors("Lighting::createShadow() F");
			
				sprintf(uniform_name, "%s%d", "shadowMap", i);
				GLint shadowMap_uniform = shader->get_uniform_location( uniform_name );
				glUniform1i(shadowMap_uniform, 1+i);
				glActiveTexture(GL_TEXTURE1 + i);
				glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
				get_errors("Lighting::createShadow() G");
			shader->unbind();
		}
	}
	for(std::vector<Shader*>::iterator iter = shaders.begin();
			iter != shaders.end();iter++){
			Shader * shader = *iter;
		shader->bind();
			GLint ambient_color_uniform  = shader->get_uniform_location( "ambient_color");
			glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
			GLint num_lights_uniform  = shader->get_uniform_location( "num_lights");
			glUniform1i(num_lights_uniform,  (int)lightlist.size());
			get_errors("Lighting::createShadow() H");
		shader->unbind();
	}
	m_camera_1 = cam_tmp;
}

void Lighting::createLight(std::vector<Shader*> shaders)
{
	if(lightlist.empty())
		return;
	
	Camera cam_tmp = m_camera_1;
	
	for(unsigned i=0; i<lightlist.size() && i<max_lights; i++){
		
		Light light = lightlist[i];
		
		
		
	}
}

void Lighting::update(float dTime){
	totaltime += dTime;
	if(lightlist.size() < 2)
		return;

	//smooth fade out of light 1
	if(totaltime < 1.0f){
		//lightlist[0].color = glm::vec4(1,1,1,1) * (1.0f - totaltime);
	}else if(totaltime < 2.0f){
		//lightlist[1].color = glm::vec4(1,1,1,1) * (2.0f - totaltime);
	}else if(totaltime < 3.0f){
		//lightlist[0].color = glm::vec4(1,1,1,1) * (1.0f - (3.0f - totaltime));
	}else if(totaltime < 4.0f){
		//lightlist[1].color = glm::vec4(1,1,1,1) * (1.0f - (4.0f - totaltime));
	}else if(totaltime < 5.0f){
		;
	}

	//flicker of light 0
	if(totaltime < 3.0)
		return;

	for(float j = 3.1 ; j < 4.5; j+=0.2){
		if(totaltime < j){
			lightlist[0].color = glm::vec4(0.1,0.1,0.1,1);
			break;
		}else if(totaltime < (j + 0.1)){
			lightlist[0].color = glm::vec4(0.9,0.9,0.9,1);
			break;
		}
	}
	

	if(totaltime > 5.0f)//Nach fünf sekunden von vorne
		totaltime = totaltime -5.0f;
}
