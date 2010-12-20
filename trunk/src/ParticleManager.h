#pragma once

#include "ParticleSystem.h"
#include <vector>

class ParticleManager
{
private:
	std::vector<ParticleSystem> particleSystems;
public:
	ParticleManager(void);
	~ParticleManager(void);
public:
	void Init();
	void AddSystem(ParticleSystem&);
	void RemoveSystem(const ParticleSystem&);
	void Update();
	void Render();
	void Shutdown();
	bool DoesExist();

};

