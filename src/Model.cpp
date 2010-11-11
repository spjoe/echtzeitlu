
#include "Model.h"
#include "camera.h"


#include <stdio.h>
#include <string.h>


using namespace echtzeitlu;

extern glm::vec3 light_position;
extern glm::vec4 light_color;
extern glm::vec4 ambient_color;
extern Camera m_camera_1;
//extern glm::mat4 model;
// extern Shader* defaultShader;

Model::Model( 	std::vector<glm::vec4> &pointlist, std::vector<glm::vec3> &normallist, 
			std::vector<GLuint> &indexlist, Shader* shader)
{
	if(pointlist.size() != normallist.size()){
		printf("[Model::Model] Warning: pointlist.size() != normallist.size()\n");
		return;
	}
	
	this->pointlist = pointlist;
	this->normallist = normallist;
	this->indexlist = indexlist;
	this->colorlist.assign(pointlist.size(), glm::vec4(0.5, 0.5, 0.5, 1));
	
	get_errors();
	PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	my_glGenVertexArrays(1, &vao_id);
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
	GLuint* tmp_vbo_id = GenerateVBO(3);
	vbo_id[0] = tmp_vbo_id[0];
	vbo_id[1] = tmp_vbo_id[1];
	vbo_id[2] = tmp_vbo_id[2];
	
	this->shader = shader;
	get_errors();
	
	bindVertex(&pointlist[0], pointlist.size() * 4 * sizeof(GLfloat));
	bindColor(&colorlist[0], colorlist.size() * 4 * sizeof(GLfloat));
	bindNormals(&normallist[0], normallist.size() * 3 * sizeof(GLfloat));
	get_errors();
	
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
}

Model::Model( 	std::vector<glm::vec4> &pointlist, std::vector<glm::vec3> &normallist,  std::vector<glm::vec2> &texturelist,
			std::vector<GLuint> &indexlist, Shader* shader)
{
	if(pointlist.size() != normallist.size()){
		printf("[Model::Model] Warning: pointlist.size() != normallist.size()\n");
		return;
	}
	
	this->pointlist = pointlist;
	this->normallist = normallist;
	this->indexlist = indexlist;
	this->texlist = texturelist;
	this->colorlist.assign(pointlist.size(), glm::vec4(0.5, 0.5, 0.5, 1));

	get_errors();
	PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	my_glGenVertexArrays(1, &vao_id);
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
	GLuint* tmp_vbo_id = GenerateVBO(3);
	vbo_id[0] = tmp_vbo_id[0];
	vbo_id[1] = tmp_vbo_id[1];
	vbo_id[2] = tmp_vbo_id[2];
	
	this->shader = shader;
	get_errors();
	
	bindVertex(&pointlist[0], pointlist.size() * 4 * sizeof(GLfloat));
	//bindColor(&colorlist[0], colorlist.size() * 4 * sizeof(GLfloat));
	bindTexture(&texlist[0], texlist.size() * 2 * sizeof(GLfloat));
	bindNormals(&normallist[0], normallist.size() * 3 * sizeof(GLfloat));
	get_errors();
	
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
}

Model::Model()
{
	this->vbo_id[0] = this->vbo_id[1] = this->vbo_id[2] = 0;
	this->vao_id = 0;
	this->shader = 0;
}

void Model::print()
{
	for(unsigned i=0; i<pointlist.size(); i++){
		printf("%f %f %f, %f %f %f\n", pointlist[i].x, pointlist[i].y, pointlist[i].z, normallist[i].x, normallist[i].y, normallist[i].z);
	}
	for(unsigned i=0; i<indexlist.size(); i++){
		printf("%d ", indexlist[i]);
	}
	printf("\n");
	
	printf("Vertices: %d, Indices: %d\n", pointlist.size(), indexlist.size());
}

void Model::draw()
{
	if(indexlist.empty())
		return;
	
	get_errors();
	shader->bind();
	get_errors();
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
    GLint light_position_uniform = shader->get_uniform_location( "light_position");
    GLint light_color_uniform    = shader->get_uniform_location( "light_color");
    GLint ambient_color_uniform  = shader->get_uniform_location( "ambient_color");
	if(!texlist.empty()){
		GLint texture_uniform = shader->get_uniform_location("texture");
		glUniform1i(texture_uniform, 0); //soll erste textureinheit verwenden
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texid);
	}



	glUniform3fv(light_position_uniform, 1, glm::value_ptr(light_position));
    glUniform4fv(light_color_uniform,    1, glm::value_ptr(light_color));
    glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
	get_errors();

    // set matrix-uniforms
    GLint perspective_uniform = shader->get_uniform_location( "perspective");
    GLint view_uniform        = shader->get_uniform_location( "view");
    GLint model_uniform       = shader->get_uniform_location( "model");

	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));
	get_errors();
	glDrawElements(GL_TRIANGLES, indexlist.size(), GL_UNSIGNED_INT, &indexlist[0]);
	get_errors();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	shader->unbind();
}

void Model::update(float fTime)
{
  
}
void Model::bindVertex(void* data, size_t size){
	bindVBO(vbo_id[0], data, size);
	GLint vertex_location = shader->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}

void Model::bindNormals(void* data, size_t size){
	bindVBO(vbo_id[2], data, size);
	GLint normal_location = shader->get_attrib_location("normal");
	glEnableVertexAttribArray(normal_location);
	glVertexAttribPointer(	normal_location, 3, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}

void Model::bindColor(void* data, size_t size){
	bindVBO(vbo_id[1], data, size);
	GLint color_location = shader->get_attrib_location("color");
	glEnableVertexAttribArray(color_location);
	glVertexAttribPointer(	color_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}
void Model::bindTexture(void* data, size_t size){
	bindVBO(vbo_id[1], data, size);
	GLint tex_location = shader->get_attrib_location("texkoord");
	glEnableVertexAttribArray(tex_location);
	glVertexAttribPointer(	tex_location, 2, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}
void Model::assignTextureId(GLuint texid)
{
	this->texid =texid;
}