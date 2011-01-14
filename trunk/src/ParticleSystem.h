/**
* @file ParticleSystem.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date Januar 2010
* @version unstable
* @brief The Base Particle System ad all other particle Systems (SteamParticleSystem, SparkParticleSystem
* @namespace echtzeitlu
*/
#pragma once

#include <vector>
#include "Particle.h"
#include "shader.hpp"
namespace echtzeitlu{
class ParticleSystem
{
protected:
	int systemType; //not used
	std::vector<Particle*> particles; // All Particles belong to the ParticleSystem
	int nrAlive; //How many of the Particls are alive
	unsigned totalparticles; //The total amout of (particles.size())

	Shader *shader; //With this Shader the particles are drawn
	glm::mat4 model; //model Matrix of the ParticleSystem
	glm::vec4 center; //position of the hole system
	GLuint vao_id;
	
public:
	ParticleSystem(void);
	~ParticleSystem(void);
	virtual void Render(void) = 0;
	//virtual void SetupShape(unsigned nr) = 0;
	virtual bool Update(float dtime) = 0;
	void generateRandomParticles();
	virtual void generateOneRandomParticle(Particle * p) = 0;
	std::string name;
};

class SteamParticleSystem : public ParticleSystem
{
private:
	std::vector<glm::vec4> colorlist;
	GLuint vbo_id[2];
	glm::vec4 pointlist[4];
	unsigned indexlist[6];
	float radius;
	float angle;
	bool animation;
	bool start;

public:
	SteamParticleSystem(std::string name,unsigned totalnr, glm::vec4 pScenter, float radius);
	virtual void Render(void);
	void SetupShape(unsigned nr);
	virtual bool Update(float dtime);

private:
	
	void generateOneRandomParticle(Particle * p);
};

class SparkParticleSystem : public ParticleSystem
{
private:
	std::vector<glm::vec4> colorlist;
	GLuint vbo_id[2];
	glm::vec4 pointlist[4];
	unsigned indexlist[6];
	float angle;
	bool animation;
	bool start;

public:
	SparkParticleSystem(std::string name,unsigned totalnr, glm::vec4 pScenter);
	virtual void Render(void);
	void SetupShape(unsigned nr);
	virtual bool Update(float dtime);

private:
	void generateOneRandomParticle(Particle * p);
};
}