#pragma once

#include <vector>
#include "Particle.h"
#include "shader.hpp"
namespace echtzeitlu{
class ParticleSystem
{
protected:
	//Texture *texture
	//BlendMode blendMode
	int systemType;
	std::vector<Particle*> particles;
	//std::vector<tShape> shapes;
	int nrAlive;
	unsigned totalparticles;
	//BoundingBox3 boundingBox
	Shader *shader;
	glm::mat4 model; 
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

public:
	SteamParticleSystem(std::string name,unsigned totalnr, glm::vec4 pScenter);
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