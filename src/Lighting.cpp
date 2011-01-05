#include "Lighting.h"
#include "camera.h"

#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtx/transform2.hpp>

using namespace echtzeitlu;

extern Camera m_camera_1;
extern int width;
extern int height;

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

void Lighting::createShadowMaps(SceneObject* scene)
{
	if(lightlist.empty())
		return;
	
	Light light = lightlist[0]; // HACK assume one light for testing
	
	Camera cam_tmp = m_camera_1;
	
	m_camera_1.intrinsic = light.proj;
	m_camera_1.extrinsic = light.view;
	
	scene->draw();
	
	glBindTexture(GL_TEXTURE_2D, light.texShadowMap);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
// 	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 0, 0, width, height, 0);
	
	m_camera_1 = cam_tmp;
	
}