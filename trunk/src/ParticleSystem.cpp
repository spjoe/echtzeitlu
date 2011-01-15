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
void ParticleSystem::generateRandomParticles()
{
	for(unsigned i = 0; i < totalparticles; i++)
	{
		Particle * par = new Particle();
		generateOneRandomParticle(par);
		particles.push_back(par);
	}
}

SteamParticleSystem::SteamParticleSystem(std::string name, unsigned totalnr, glm::vec4 pScenter, float radius)
{
	this->name = name;
	this->radius = radius;
	shader = new Shader("../shader/SmokeShader");
	shader->bind_frag_data_location("fragColor");
	totalparticles = totalnr;
	colorlist.assign(4, glm::vec4(0.5, 0.5, 0.5, 0.5));

	srand(87435);
	generateRandomParticles();
	
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
	start = false;
	angle = 0.0f;


}
void SteamParticleSystem::SetupShape(unsigned nr)
{
	if(nr >= totalparticles) return;
}
bool SteamParticleSystem::Update(float dtime)
{
	if(animation){
		float scale = 200.0f;
		angle = (angle + dtime * scale);
		if(angle > 360.0f)
			angle = angle - 360.0f;
		if(!(angle > 90 && angle < 270)){
			if(!start){
				for(unsigned i = 0; i < totalparticles; i++){
					generateOneRandomParticle(particles[i]);
				}
				start = true;
			}
			for(int i = 0;i < (angle - 270) * 20 && i < totalparticles; i++)
			{
				particles[i]->alive = true;
			}
			
		}else if(angle > 200 && angle < 240 && start)
			start = false;

		for(unsigned i = 0; i < totalparticles; i++){
			if(particles[i]->alive){
				particles[i]->oldPos = particles[i]->position;
				particles[i]->position = particles[i]->position + (particles[i]->velocity * dtime);
				particles[i]->velocity = particles[i]->velocity + (particles[i]->g * dtime);
				particles[i]->energy = std::max(0.0, particles[i]->energy - (double)(rand() % 100) * dtime);
				particles[i]->color.a = float(particles[i]->energy)/ 200.0f;
			}
		}
	}
	return true;
	/*
	for(unsigned i = 0; i < totalparticles; i++){
		particles[i]->position = particles[i]->position + (particles[i]->velocity * dtime);
		particles[i]->energy = std::max(0.0, particles[i]->energy - (double)(rand() % 40) * dtime);
		particles[i]->velocity = particles[i]->velocity + (particles[i]->g * dtime);
		particles[i]->color.a = float(particles[i]->energy)/ 200.0f;
		if(particles[i]->color.a < 0.01){
			generateOneRandomParticle(particles[i]);
		}
	}
	return true;*/
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

	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 rot = m_camera_1.extrinsic; 
	rot[3][0] = rot[3][1] =rot[3][2] = 0;
	rot[0][3] = rot[1][3] =rot[2][3] = 0;
	rot[3][3] = 1;
	rot = glm::transpose(rot);
	glUniformMatrix4fv(rot_uniform,       1, GL_FALSE, glm::value_ptr(rot));

	for(size_t i = 0; i < totalparticles; i++)
	{
		Particle particle = *(particles[i]);
		if(particle.alive){
			glUniform1f(size_uniform, (GLfloat)particle.size);
			glUniform4fv(particle_position_uniform,1, glm::value_ptr(particle.position));
			glUniform4fv(color_uniform,1, glm::value_ptr(particle.color));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indexlist);
			get_errors();
		}
	}
	get_errors();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	shader->unbind();
	glDisable(GL_BLEND);
}
void SteamParticleSystem::generateOneRandomParticle(Particle *par)
{
	int alpha = rand() % 3600;
	float falpha = float(alpha)/10.0f;
	int diffradius = rand() % 100 - 50;
	float radius = this->radius + ((float)diffradius /1000.0f);
	float fx = 0.0f;
	float fy =  radius * std::cos(glm::radians(falpha));
	float fz = radius * std::sin(glm::radians(falpha));
	int size = rand() % 100 + 100;
	float sf = (float) size / 3000.0f;
	
	float speed2 = (float)(rand() % 100)  / 40.0f;
	float speed3 = (float)(rand() % 100 - 50) / 300.0f;
	float speed1 = (float)(rand() % 100 - 50) / 300.0f;
	float bright = (float)(rand() % 30 + 40) / 100.0f; 
	float accjit = (float)(rand() % 100 - 50) / 1000.0f;

	par->position = glm::vec4(fx,fy,fz,1);
	par->oldPos = glm::vec4(fx,fy,fz,1);
	par->size = sf;
	par->energy = (float) (rand() % 100);
	par->velocity = glm::vec4(2.0+speed2,speed3,speed1,0.0f);
	par->color = glm::vec4(bright,bright,bright+0.05,0.5);
	par->g = glm::vec4(0.0,0.0,std::pow(speed2+0.5,2)+accjit,0.0);
	par->alive = false;
}


//Spark Particle System
SparkParticleSystem::SparkParticleSystem(std::string name, unsigned totalnr, glm::vec4 pScenter)
{
	this->name = name;
	shader = new Shader("../shader/SparkShader");
	shader->bind_frag_data_location("fragColor");
	totalparticles = totalnr;
	colorlist.assign(4, glm::vec4(0.8, 0.1, 0.1, 0.5));

	srand(87435);
	generateRandomParticles();
	
	PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	my_glGenVertexArrays(1, &vao_id);
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
	GLuint* tmp_vbo_id = GenerateVBO(2);
	vbo_id[0] = tmp_vbo_id[0];
	vbo_id[1] = tmp_vbo_id[1];
	delete tmp_vbo_id;

	
	pointlist[0] = glm::vec4(-1, 2, 0, 1); 
	pointlist[1] = glm::vec4( 1, 2, 0, 1);
	pointlist[2] = glm::vec4( 1,-2, -1.0f, 1);
	pointlist[3] = glm::vec4(-1,-2, -1.0f, 1);

	
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
	this->animation = true;
	this->angle = 0.0f;
	this->start = false;

}
void SparkParticleSystem::SetupShape(unsigned nr)
{
	if(nr >= totalparticles) return;
}
bool SparkParticleSystem::Update(float dtime)
{
	if(animation){
		float scale = 200.0f;
		angle = (angle + dtime * scale);
		if(angle > 360.0f)
			angle = angle - 360.0f;
		if(angle > 150.0f && !start){
			for(unsigned i = 0; i < totalparticles; i++){
				generateOneRandomParticle(particles[i]);
				particles[i]->alive = true;
			}
			start = true;
		}else if(angle > 100 && angle < 150 && start)
			start = false;

		for(unsigned i = 0; i < totalparticles; i++){
			particles[i]->oldPos = particles[i]->position;
			particles[i]->position = particles[i]->position + (particles[i]->velocity * dtime);
			particles[i]->velocity = particles[i]->velocity + (particles[i]->g * dtime);
			particles[i]->energy = std::max(0.0, particles[i]->energy - (double)(rand() % 40) * dtime);
			particles[i]->color.a = float(particles[i]->energy)/ 200.0f;
		}
	}
	return true;
}

void SparkParticleSystem::Render(void)
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
	GLint particle_old_position_uniform       = shader->get_uniform_location( "particle_old_position");
	GLint color_uniform       = shader->get_uniform_location( "color");

	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));

	glm::mat4 rot = m_camera_1.extrinsic; 
	rot[3][0] = rot[3][1] =rot[3][2] = 0;
	rot[0][3] = rot[1][3] =rot[2][3] = 0;
	rot[3][3] = 1;
	rot = glm::transpose(rot);
	glUniformMatrix4fv(rot_uniform,       1, GL_FALSE, glm::value_ptr(rot));

	for(size_t i = 0; i < totalparticles; i++)
	{
		Particle particle = *particles[i];
		if(particle.alive){
			glUniform1f(size_uniform, (GLfloat)particle.size);
			glUniform4fv(particle_position_uniform,1, glm::value_ptr(particle.position));
			glUniform4fv(particle_old_position_uniform,1, glm::value_ptr(particle.oldPos));
			glUniform4fv(color_uniform,1, glm::value_ptr(particle.color));
			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, indexlist);
			get_errors();
		}
	}
	get_errors();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	shader->unbind();
	glDisable(GL_BLEND);
}


void SparkParticleSystem::generateOneRandomParticle(Particle * par)
{
	int x = rand() % 500;
	int y = rand() % 500;
	int z = 0;
	float fx = ((float)x - 250.0f) / 200.0f;
	float fy = ((float)y - 250.0f) / 200.0f;
	float fz = ((float)z  -250.0f) / 200.0f;
	int size = rand() % 100 + 100;
	float sf = (float) size / 3000.0f;
	float speed = 1.0f + ((float)(rand() % 100)) / 100.0f;
	float speed2 = (float)(rand() % 100 - 50)  / 100.0f;
	float speed3 = (float)(rand() % 100 - 50) / 100.0f;
	float bright = (float)(rand() % 30 + 40) / 100.0f; 

	par->position = glm::vec4(fx,fy,fz,1);
	par->oldPos = glm::vec4(fx,fy,fz,1);
	par->size = sf*0.7;
	par->energy = (float) (rand() % 100);
	par->velocity = glm::vec4(fx*2,fy*2,speed*3,0);
	par->color = glm::vec4((float)std::min(1.0f,bright+0.7f),bright+0.2,bright,1);
	par->g = glm::vec4(0.0f,0.0f,-9.81f,0.0f);
	par->alive = false;
}
