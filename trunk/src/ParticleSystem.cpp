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

SteamParticleSystem::SteamParticleSystem(std::string name, unsigned totalnr, glm::vec4 pScenter)
{
	this->name = name;
	shader = new Shader("../shader/SmokeShader");
	shader->bind_frag_data_location("fragColor");
	totalparticles = totalnr;
	colorlist.assign(4, glm::vec4(0.5, 0.5, 0.5, 0.5));

	srand(87435);
	generateRandomeParticles();
	
	PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	my_glGenVertexArrays(1, &vao_id);
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
	GLuint* tmp_vbo_id = GenerateVBO(2);
	vbo_id[0] = tmp_vbo_id[0];
	vbo_id[1] = tmp_vbo_id[1];
	delete tmp_vbo_id;

	//bindVBO(vbo_id[1], &colorlist[0], 4 * 4 * sizeof(GLfloat));
	//GLint color_location = shader->get_attrib_location("color");
	//glEnableVertexAttribArray(color_location);
	//glVertexAttribPointer(	color_location, 4, GL_FLOAT, 
	//						GL_FALSE, 0, NULL);

	
	pointlist[0] = glm::vec4(-1, 1, 0, 1); 
	pointlist[1] = glm::vec4( 1, 1, 0, 1);
	pointlist[2] = glm::vec4( 1,-1, 0, 1);
	pointlist[3] = glm::vec4(-1,-1, 0, 1);

	
	//ccw
	indexlist[0] = 2;
	indexlist[1] = 1;
	indexlist[2] = 0;
	indexlist[3] = 0;
	indexlist[4] = 3;
	indexlist[5] = 2;

	bindVBO(vbo_id[0], pointlist, 4 * 4 * sizeof(GLfloat));
	GLint vertex_location = shader->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);

	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	model = glm::mat4(	1.0, 0.0, 0.0, 0.0,
						0.0, 1.0, 0.0, 0.0,
						0.0, 0.0, 1.0, 0.0,
						-pScenter.x, -pScenter.y, -pScenter.z, 1.0);

	center = pScenter;

}
void SteamParticleSystem::SetupShape(unsigned nr)
{
	if(nr >= totalparticles) return;

	/*glm::mat4 rot = m_camera_1.extrinsic; 
	rot[3][0] = rot[3][1] =rot[3][2] = 0;
	rot[0][3] = rot[1][3] =rot[2][3] = 0;
	rot[3][3] = 1;
	rot = glm::transpose(rot);*/
	
}
bool SteamParticleSystem::Update(float dtime)
{
	//glm::mat4 rot = m_camera_1.extrinsic; 
	//rot[3][0] = rot[3][1] =rot[3][2] = 0;
	//rot[0][3] = rot[1][3] =rot[2][3] = 0;
	//rot[3][3] = 1;
	//rot = glm::transpose(rot);
	for(unsigned i = 0; i < totalparticles; i++){
		particles[i].position = particles[i].position + (particles[i].velocity * dtime);
		particles[i].color.a = std::max(0.0, particles[i].color.a - dtime * 0.5);
		if(particles[i].position.z > 5){
			particles[i].position = particles[i].oldPos;
			particles[i].color.a = 0.5;
		}
		//SetupShape(i);
	}
	return true;
}

void SteamParticleSystem::Render(void)
{
	glEnable(GL_BLEND);
	get_errors();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

	glUniform3fv(light_position_uniform, 1, glm::value_ptr(light_position));
    glUniform4fv(light_color_uniform,    1, glm::value_ptr(light_color));
    glUniform4fv(ambient_color_uniform,  1, glm::value_ptr(ambient_color));
	get_errors();

    // set matrix-uniforms
    GLint perspective_uniform = shader->get_uniform_location( "perspective");
    GLint view_uniform        = shader->get_uniform_location( "view");
    GLint model_uniform       = shader->get_uniform_location( "model");
	GLint rot_uniform       = shader->get_uniform_location( "rot");
	GLint size_uniform       = shader->get_uniform_location( "size");
	GLint particle_position_uniform       = shader->get_uniform_location( "particle_position");
	GLint color_uniform       = shader->get_uniform_location( "color");
	
// 	for(unsigned i=0; i<4; i++){
// 		for(unsigned j=0; j<4; j++){
// 		  printf(" %f", model[i][j]);
// 		}
// 		printf("\n");
// 	}

	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 rot = m_camera_1.extrinsic; 
	rot[3][0] = rot[3][1] =rot[3][2] = 0;
	rot[0][3] = rot[1][3] =rot[2][3] = 0;
	rot[3][3] = 1;
	rot = glm::transpose(rot);
	glUniformMatrix4fv(rot_uniform,       1, GL_FALSE, glm::value_ptr(rot));

	//glm::vec4 *pointlist = new glm::vec4[totalparticles*4];
	//unsigned *indexlist = new unsigned[totalparticles*6];
	for(size_t i = 0; i < totalparticles; i++)
	{
		Particle particle = particles[i];
		
		glUniform1f(size_uniform, (GLfloat)particle.size);
		glUniform4fv(particle_position_uniform,1, glm::value_ptr(particle.position));
		glUniform4fv(color_uniform,1, glm::value_ptr(particle.color));
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indexlist);
		get_errors();

		//for(size_t j = 0; j < 4; j++)
			//pointlist[i*4 + j] = shapes[i].vertex[j];
		//better all particle pos relativ to particle system, so model matrix stay the same draw all particles with one glDrawElements
	}

	//pointlist ändert sich ist daher vbo angebracht?
	//indexlist bleibt gleich
	
	//get_errors();
	//glDrawElements(GL_TRIANGLES, totalparticles*2, GL_UNSIGNED_INT, indexlist);
	get_errors();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	shader->unbind();
	//delete pointlist;
	//delete indexlist;
	glDisable(GL_BLEND);
}

void SteamParticleSystem::generateRandomeParticles()
{
	for(unsigned i = 0; i < totalparticles; i++)
	{
		Particle cur;
		int x = rand() % 500;
		int y = rand() % 500;
		int z = rand() % 500;
		float fx = (float)x / 200.0f;
		float fy = (float)y / 200.0f;
		float fz = (float)z / 200.0f;
		int size = rand() % 100 + 100;
		float sf = (float) size / 2000.0f;
		float speed = 1.0f + ((float)(rand() % 100)) / 100.0f;
		float speed2 = (float)(rand() % 100 - 50)  / 1000.0f;
		float speed3 = (float)(rand() % 100 - 50) / 1000.0f;
		float bright = (float)(rand() % 50 + 30) / 100.0f; 

		cur.position = glm::vec4(fx,fy,fz,1);
		cur.oldPos = glm::vec4(fx,fy,fz,1);
		cur.size = sf;//sf;
		cur.energy = 100;
		cur.velocity = glm::vec4(speed2,speed3,speed,0);
		cur.color = glm::vec4(bright,bright,bright,0.5);
		particles.push_back(cur);
		//shapes.push_back(tShape());
		//SetupShape(i);
	}
}