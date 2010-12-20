#pragma once

#include <vector>
#include "Particle.h"

class ParticleSystem
{
private:
	//Texture *texture
	//BlendMode blendMode
	int systemType;
	std::vector<Particle> particles;
	//Array PShape shapes
	int nrAlive;
	//BoundingBox3 boundingBox
public:
	ParticleSystem(void);
	~ParticleSystem(void);

	void SetupShade(int nr);

	bool Update();
};

