#include "ParticleSystem.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 
#include "camera.h"

using namespace echtzeitlu;

extern glm::vec3 light_position;
extern glm::vec4 light_color;
extern glm::vec4 ambient_color;
extern Camera m_camera_1;

ParticleSystem::ParticleSystem(void)
{
}


ParticleSystem::~ParticleSystem(void)
{
}

SmokeParticleSystem::SmokeParticleSystem(std::string name)
{
	this->name = name;
	shader = new Shader("../shader/SmokeShader");
	shader->bind_frag_data_location("fragColor");
}
void SmokeParticleSystem::SetupShape(int nr)
{
	if(nr > totalparticles) return;

	glm::vec4 camerapos = m_camera_1.extrinsic * particles[nr].position;

	shapes[nr].vertex[0] = camerapos + glm::vec4(-particles[nr].size, particles[nr].size,0,1);
	shapes[nr].vertex[1] = camerapos + glm::vec4(particles[nr].size, particles[nr].size,0,1);
	shapes[nr].vertex[2] = camerapos + glm::vec4(particles[nr].size, -particles[nr].size,0,1);
	shapes[nr].vertex[3] = camerapos + glm::vec4(-particles[nr].size, -particles[nr].size,0,1);
}
bool SmokeParticleSystem::Update(float dtime)
{
	return true;
}

void SmokeParticleSystem::Render(void)
{
	get_errors();
	shader->bind();
	get_errors();
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
    GLint light_position_uniform = shader->get_uniform_location( "light_position");
    GLint light_color_uniform    = shader->get_uniform_location( "light_color");
    GLint ambient_color_uniform  = shader->get_uniform_location( "ambient_color");
	get_errors();
//	if(!texlist.empty() && !texidlist.empty()){ //very HACKY!!! fallt weg wenn ich effekte sinnvoll einlese und mit model klasse verknüpfe!
//		GLint texture_uniform = shader->get_uniform_location("texture");
//		get_errors();
//		glUniform1i(texture_uniform, 0); //soll erste textureinheit verwenden
//		get_errors();
//		glActiveTexture(GL_TEXTURE0);
//		get_errors();
//		glBindTexture(GL_TEXTURE_2D, texidlist[0]);
//		get_errors();
//	}



	glUniform3fv(light_position_uniform, 1, glm::value_ptr(light_position));
    glUniform4fv(light_color_uniform,    1, glm::value_ptr(light_color));
    glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
	get_errors();

    // set matrix-uniforms
    GLint perspective_uniform = shader->get_uniform_location( "perspective");
    GLint view_uniform        = shader->get_uniform_location( "view");
    GLint model_uniform       = shader->get_uniform_location( "model");
	
// 	for(unsigned i=0; i<4; i++){
// 		for(unsigned j=0; j<4; j++){
// 		  printf(" %f", model[i][j]);
// 		}
// 		printf("\n");
// 	}

	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));

	glm::vec4 *pointlist = new glm::vec4[totalparticles*4];
	unsigned *indexlist = new unsigned[totalparticles*6];
	for(size_t i = 0; i < totalparticles; i++)
	{
		Particle curr = particles[i];
		this->SetupShape(i);
		for(size_t j = 0; j < 4; i++)
			pointlist[i*4 + j] = shapes[i].vertex[j];

		indexlist[i*6 + 0] = i*4 + 0;
		indexlist[i*6 + 1] = i*4 + 1;
		indexlist[i*6 + 2] = i*4 + 2;
		indexlist[i*6 + 3] = i*4 + 2;
		indexlist[i*6 + 4] = i*4 + 3;
		indexlist[i*6 + 5] = i*4 + 0;
		//better all particle pos relativ to particle system, so mode stay the same draw all particles with one glDrawElements
	}
	//pointlist ändert sich ist daher vbo angebracht?
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));
	get_errors();
	//glDrawElements(GL_TRIANGLES, totalparticles*6, GL_UNSIGNED_INT, indexlist);
	get_errors();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	shader->unbind();
}


