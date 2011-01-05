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
	colorlist.assign(totalnr*4, glm::vec4(0.5, 0.5, 0.5, 0.5));

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

	bindVBO(vbo_id[1], &colorlist[0], colorlist.size() * 4 * sizeof(GLfloat));
	GLint color_location = shader->get_attrib_location("color");
	glEnableVertexAttribArray(color_location);
	glVertexAttribPointer(	color_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);

	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	model = glm::mat4(	1.0,  0.0, 0.0, 0.0,
					0.0,  1.0, 0.0, 0.0,
					0.0,  0.0, 1.0, 0.0,
					-pScenter.x,	-pScenter.y, -pScenter.z, 1.0);

	center = pScenter;

}
void SteamParticleSystem::SetupShape(unsigned nr, glm::mat4 rot)
{
	if(nr >= totalparticles) return;

	/*glm::mat4 rot = m_camera_1.extrinsic; 
	rot[3][0] = rot[3][1] =rot[3][2] = 0;
	rot[0][3] = rot[1][3] =rot[2][3] = 0;
	rot[3][3] = 1;
	rot = glm::transpose(rot);*/
	shapes[nr].vertex[0] = rot * glm::vec4(-particles[nr].size, particles[nr].size,0,1) + particles[nr].position ; 
	shapes[nr].vertex[1] = rot * glm::vec4(particles[nr].size,particles[nr].size,0,1) + particles[nr].position;
	shapes[nr].vertex[2] = rot * glm::vec4(particles[nr].size, -particles[nr].size,0,1) + particles[nr].position;
	shapes[nr].vertex[3] = rot * glm::vec4(-particles[nr].size, -particles[nr].size,0,1) + particles[nr].position;
}
bool SteamParticleSystem::Update(float dtime)
{
	glm::mat4 rot = m_camera_1.extrinsic; 
	rot[3][0] = rot[3][1] =rot[3][2] = 0;
	rot[0][3] = rot[1][3] =rot[2][3] = 0;
	rot[3][3] = 1;
	rot = glm::transpose(rot);
	for(unsigned i = 0; i < totalparticles; i++){
		particles[i].position = particles[i].position + (particles[i].velocity * dtime);
		if(particles[i].position.z > 5)
			particles[i].position = particles[i].oldPos;
		SetupShape(i,rot);
	}
	return true;
}

void SteamParticleSystem::Render(void)
{
	glEnable(GL_BLEND);
	get_errors();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	get_errors();
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
		
		for(size_t j = 0; j < 4; j++)
			pointlist[i*4 + j] = shapes[i].vertex[j];

		indexlist[i*6 + 0] = i*4 + 2;
		indexlist[i*6 + 1] = i*4 + 1;
		indexlist[i*6 + 2] = i*4 + 0;
		indexlist[i*6 + 3] = i*4 + 0;
		indexlist[i*6 + 4] = i*4 + 3;
		indexlist[i*6 + 5] = i*4 + 2;
		//better all particle pos relativ to particle system, so model matrix stay the same draw all particles with one glDrawElements
	}
	bindVBO(vbo_id[0], pointlist, totalparticles * 4 * 4 * sizeof(GLfloat));
	GLint vertex_location = shader->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
	get_errors();

	//pointlist ändert sich ist daher vbo angebracht?
	//indexlist bleibt gleich
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));
	get_errors();
	glDrawElements(GL_TRIANGLES, totalparticles*2, GL_UNSIGNED_INT, indexlist);
	get_errors();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	shader->unbind();
	delete pointlist;
	delete indexlist;
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
		cur.position = glm::vec4(fx,fy,fz,1);
		cur.oldPos = glm::vec4(fx,fy,fz,1);
		cur.size = sf;
		cur.energy = 100;
		cur.velocity = glm::vec4(speed2,speed3,speed,0);
		cur.color = 0;
		particles.push_back(cur);
		shapes.push_back(tShape());
		//SetupShape(i);
	}
}