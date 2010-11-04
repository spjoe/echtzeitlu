
#include "Model.h"
#include "camera.h"


#include <stdio.h>
#include <string.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

using namespace echtzeitlu;

extern glm::vec3 light_position;
extern glm::vec4 light_color;
extern glm::vec4 ambient_color;
extern Camera m_camera_1;
extern glm::mat4 model;

Model::Model(GLuint vbo_id[3], GLuint vao_id,GLuint * indices, size_t numIndices) : shader("../shader/simple_shader")
{
	//memcpy (this->vbo_id,vbo_id, 3 * sizeof(GLuint));
	this->vbo_id[0] = vbo_id[0];
	this->vbo_id[1] = vbo_id[1];
	this->vbo_id[2] = vbo_id[2];
	this->vao_id = vao_id;
	this->indices = indices;
	this->numIndices = numIndices;
	shader.bind_frag_data_location("fragColor");

}
Model::Model() : shader("../shader/simple_shader")
{
	this->vbo_id[0] = this->vbo_id[1] = this->vbo_id[2] = 0;
	this->vao_id = 0;
	this->indices = NULL;
}

void Model::draw()
{
	drawAll();
	if(indices == NULL) //rootScene
		return;

	shader.bind();

	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);

    GLint light_position_uniform = shader.get_uniform_location( "light_position");
    GLint light_color_uniform    = shader.get_uniform_location( "light_color");
    GLint ambient_color_uniform  = shader.get_uniform_location( "ambient_color");

	glUniform3fv(light_position_uniform, 1, glm::value_ptr(light_position));
    glUniform4fv(light_color_uniform,    1, glm::value_ptr(light_color));
    glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));


    // set matrix-uniforms
    GLint perspective_uniform = shader.get_uniform_location( "perspective");
    GLint view_uniform        = shader.get_uniform_location( "view");
    GLint model_uniform       = shader.get_uniform_location( "model");

	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));

	glDrawElements(GL_TRIANGLES, sizeof(indices)/sizeof(*indices), GL_UNSIGNED_INT, indices);

	my_glBindVertexArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	shader.unbind();

}

void Model::update(float fTime)
{
  
}
void Model::bindVertex(void* data, size_t size){
	bindVBO(vbo_id[0], data, size);
	GLint vertex_location = shader.get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}

void Model::bindNormals(void* data, size_t size){
	bindVBO(vbo_id[1], data, size);
	GLint normal_location = shader.get_attrib_location("normal");
	glEnableVertexAttribArray(normal_location);
	glVertexAttribPointer(	normal_location, 3, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}

void Model::bindColor(void* data, size_t size){
	bindVBO(vbo_id[2], data, size);
	GLint color_location = shader.get_attrib_location("color");
	glEnableVertexAttribArray(color_location);
	glVertexAttribPointer(	color_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}