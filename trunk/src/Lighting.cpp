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
extern Camera m_camera_ortho;
extern int width;
extern int height;
extern Shader *defaultColorShader;
extern Shader *simpleShader;
extern Shader* lightShader;
extern Shader* gaussShader;
extern Shader* additionShader;

char lm_errmsg[128];

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
	light.tview = glm::transpose(light.view);
	
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
	
	glGenTextures(1, &light_depth);
	glBindTexture(GL_TEXTURE_2D, light_depth);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	get_errors("Lighting::init() C");
	
	my_glGenFramebuffers(1,&light_fbo);
	my_glBindFramebuffer(GL_FRAMEBUFFER, light_fbo);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, light_map, 0);
	my_glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, light_depth, 0);
	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	get_errors("Lighting::init() D");
		
	PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	my_glGenVertexArrays(1, &light_vao_id);
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(light_vao_id);
	get_errors();
	GLuint* tmp_vbo_id = GenerateVBO(1);
	light_vbo_id = *tmp_vbo_id;
	delete tmp_vbo_id;
	
	float w = 0.1f;
	float h = 0.05f;
	light_verts[0] = glm::vec4(-w, h, 0, 1); 
	light_verts[1] = glm::vec4( w, h, 0, 1);
	light_verts[2] = glm::vec4( w,-h, 0, 1);
	light_verts[3] = glm::vec4(-w,-h, 0, 1);
	light_verts[4] = glm::vec4(-h, w, 0, 1); 
	light_verts[5] = glm::vec4( h, w, 0, 1);
	light_verts[6] = glm::vec4( h,-w, 0, 1);
	light_verts[7] = glm::vec4(-h,-w, 0, 1);
	
	light_idxs[0] = 2;
	light_idxs[1] = 1;
	light_idxs[2] = 0;
	light_idxs[3] = 0;
	light_idxs[4] = 3;
	light_idxs[5] = 2;
	light_idxs[6] = 6;
	light_idxs[7] = 5;
	light_idxs[8] = 4;
	light_idxs[9] = 4;
	light_idxs[10] = 7;
	light_idxs[11] = 6;
	
	bindVBO(light_vbo_id, light_verts, 8 * 4 * sizeof(GLfloat));
	GLint vertex_location = lightShader->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);

	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	
	my_glGenVertexArrays(1, &gauss_vao_id);
	my_glBindVertexArray(gauss_vao_id);
	get_errors();
	tmp_vbo_id = GenerateVBO(2);
	gauss_vbo_id[0] = tmp_vbo_id[0];
	gauss_vbo_id[1] = tmp_vbo_id[1];
	delete tmp_vbo_id;
	
	w = 0.5f * width;
	h = 0.5f * height;
	gauss_verts[0] = glm::vec4(-w, h, 0, 1); 
	gauss_verts[1] = glm::vec4( w, h, 0, 1);
	gauss_verts[2] = glm::vec4( w,-h, 0, 1);
	gauss_verts[3] = glm::vec4(-w,-h, 0, 1);
	
	gauss_texs[0] = glm::vec2(0.0f, 1.0f);
	gauss_texs[1] = glm::vec2(1.0f, 1.0f);
	gauss_texs[2] = glm::vec2(1.0f, 0.0f);
	gauss_texs[3] = glm::vec2(0.0f, 0.0f);
	
	gauss_idxs[0] = 2;
	gauss_idxs[1] = 1;
	gauss_idxs[2] = 0;
	gauss_idxs[3] = 0;
	gauss_idxs[4] = 3;
	gauss_idxs[5] = 2;
	
	bindVBO(gauss_vbo_id[0], gauss_verts, 4 * 4 * sizeof(GLfloat));
	vertex_location = gaussShader->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
	vertex_location = additionShader->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
							
							
	get_errors("Lighting::init() E");
	bindVBO(gauss_vbo_id[1], gauss_texs, 2 * 4 * sizeof(GLfloat));
	sprintf(lm_errmsg, "Lighting::init() F %d", gauss_vbo_id[1]);
	get_errors(lm_errmsg);
	GLint tex_location = gaussShader->get_attrib_location("texkoord");
	glEnableVertexAttribArray(tex_location);
	glVertexAttribPointer(	tex_location, 2, GL_FLOAT, 
							GL_FALSE, 0, NULL);
	tex_location = additionShader->get_attrib_location("texkoord");
	glEnableVertexAttribArray(tex_location);
	glVertexAttribPointer(	tex_location, 2, GL_FLOAT, 
							GL_FALSE, 0, NULL);
	sprintf(lm_errmsg, "Lighting::init() I %d", tex_location);
	get_errors(lm_errmsg);
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	
	m_camera_ortho.apply(gaussShader);
	m_camera_ortho.apply(additionShader);
	
	isinit = true;
}

void Lighting::createLightMap(SceneObject* scene)
{
	
	if(lightlist.empty())
		return;
		
	my_glBindFramebuffer(GL_FRAMEBUFFER, light_fbo);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	scene->drawSimple();
	
	lightShader->bind();
	get_errors();
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(light_vao_id);
	get_errors();

    // set matrix-uniforms
    GLint model_uniform = lightShader->get_uniform_location( "model");
	GLint rot_uniform = lightShader->get_uniform_location( "rot");
	GLint color_uniform = lightShader->get_uniform_location( "color");
	
	glm::mat4 rot = m_camera_1.extrinsic; 
	rot[3][0] = rot[3][1] =rot[3][2] = 0;
	rot[0][3] = rot[1][3] =rot[2][3] = 0;
	rot[3][3] = 1;
	rot = glm::transpose(rot);
	glUniformMatrix4fv(rot_uniform, 1, GL_FALSE, glm::value_ptr(rot));

	for(unsigned i=0; i<lightlist.size(); i++){
		Light light = lightlist[i];
		
		glm::mat4 model = glm::mat4(	1.0, 0.0, 0.0, 0.0,
										0.0, 1.0, 0.0, 0.0,
										0.0, 0.0, 1.0, 0.0,
										light.position.x,light.position.y,light.position.z, 1.0);
						
		glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
		glUniform4fv(color_uniform,1, glm::value_ptr(light.color));
// 		printf("%f %f %f\n", light.color.r, light.color.g, light.color.b);
		glDrawElements(GL_TRIANGLES, 12, GL_UNSIGNED_INT, light_idxs);
		get_errors();
	}

	get_errors();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	lightShader->unbind();

	my_glBindFramebuffer(GL_FRAMEBUFFER, 0);
	

	gaussShader->bind();
	my_glBindVertexArray(gauss_vao_id);
	
	model_uniform = gaussShader->get_uniform_location( "model");
	color_uniform = gaussShader->get_uniform_location( "color");
	glm::mat4 model = glm::mat4(	1.0, 0.0, 0.0, 0.0,
									0.0, 1.0, 0.0, 0.0,
									0.0, 0.0, 1.0, 0.0,
									0.0, 0.0, 0.0, 1.0);
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(color_uniform,1, glm::value_ptr(glm::vec4(1,0,0,1)));
	
	GLint texture_uniform = gaussShader->get_uniform_location("colorMap");
	GLint vertical_uniform = gaussShader->get_uniform_location("vertical");
	glUniform1i(texture_uniform, 0);
// 	get_errors(lm_errmsg);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, light_map);
	get_errors("Lighting::createLightMap()");
	
	glUniform1i(vertical_uniform, 1);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, gauss_idxs);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
	get_errors("Lighting::createLightMap()");
	
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	glUniform1i(vertical_uniform, 0);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, gauss_idxs);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
	
	
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	gaussShader->unbind();
	get_errors("Lighting::createLightMap()");
	
// 	glEnable(GL_DEPTH_TEST);
// 	printf("Lighting::createLightMap() B");
}

void Lighting::addLightMap(GLuint tex1)
{
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	additionShader->bind();
	my_glBindVertexArray(gauss_vao_id);
	
	GLint model_uniform = additionShader->get_uniform_location( "model");
	
	GLint color_uniform = additionShader->get_uniform_location( "color");
	
	glm::mat4 model = glm::mat4(	1.0, 0.0, 0.0, 0.0,
									0.0, 1.0, 0.0, 0.0,
									0.0, 0.0, 1.0, 0.0,
									0.0, 0.0, 0.0, 1.0);
	glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
	glUniform4fv(color_uniform,1, glm::value_ptr(glm::vec4(1,0,0,1)));
	
	GLint colorMap_uniform = additionShader->get_uniform_location("colorMap");
	GLint lightMap_uniform = additionShader->get_uniform_location("lightMap");
	glUniform1i(colorMap_uniform, 0);
	glUniform1i(lightMap_uniform, 1);
	
// 	get_errors(lm_errmsg);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, tex1);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, light_map);

	
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, gauss_idxs);
	glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 0, 0, width, height, 0);
	get_errors("Lighting::addTextures() 4");
	
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	additionShader->unbind();
	get_errors("Lighting::addTextures() 5");
}

void Lighting::Render()
{
	
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
			lightlist[0].color = glm::vec4(0.0,0.0,0.0,1);
			break;
		}else if(totaltime < (j + 0.1)){
			lightlist[0].color = glm::vec4(1,1,1,1);
			break;
		}
	}
	

	if(totaltime > 5.0f)//Nach f�nf sekunden von vorne
		totaltime = totaltime -5.0f;
}
