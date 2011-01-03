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
	std::vector<Particle> particles;
	std::vector<tShape> shapes;
	int nrAlive;
	int totalparticles;
	//BoundingBox3 boundingBox
	Shader *shader;
	glm::mat4 model; //position of the hole system
public:
	ParticleSystem(void);
	~ParticleSystem(void);
	virtual void Render(void) = 0;
	virtual void SetupShape(int nr) = 0;
	virtual bool Update(float dtime) = 0;

	std::string name;
};

class SmokeParticleSystem : public ParticleSystem
{
private:
	GLint vao_id;

public:
	SmokeParticleSystem(std::string name);
	virtual void Render(void);
	virtual void SetupShape(int nr);
	virtual bool Update(float dtime);
};
}