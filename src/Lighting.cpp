#include "Lighting.h"
#include "shader.hpp"
#include "camera.h"

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
	lightlist.push_back(light);
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
	
	scene->drawSimple();
	get_errors("Lighting::createShadow() A");
	
	glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, width, height, 0);
	get_errors("Lighting::createShadow() B");
	
	
	shader->bind();
	
	GLint light_position_uniform = shader->get_uniform_location( "light_position");
	GLint light_color_uniform    = shader->get_uniform_location( "light_color");
	GLint ambient_color_uniform  = shader->get_uniform_location( "ambient_color");
	get_errors("Lighting::createShadow() C");
	glUniform3fv(light_position_uniform, 1, glm::value_ptr(light.position));
    glUniform4fv(light_color_uniform,    1, glm::value_ptr(light_color));
    glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
	get_errors("Lighting::createShadow() D");
	
	glm::mat4 biasprojview = light.bias * light.proj * light.view;
	GLint biasprojview_uniform = shader->get_uniform_location("shadowProjView");
	glUniformMatrix4fv(biasprojview_uniform, 1, GL_FALSE, glm::value_ptr(biasprojview));
	get_errors("Lighting::createShadow() E");
	
	GLint shadowMap_uniform = shader->get_uniform_location("shadowMap");
	glUniform1i(shadowMap_uniform, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
	get_errors("Lighting::createShadow() F");
	
	shader->unbind();
	
	m_camera_1 = cam_tmp;
	
}