#pragma once

#include "ParticleSystem.h"
#include <set>

namespace echtzeitlu{
struct ltps
{
  bool operator()( ParticleSystem* ps1,  ParticleSystem* ps2) const
  {
	  return strcmp(ps1->name.c_str(),ps2->name.c_str()) < 0;
  }
};


class ParticleManager
{
private:
	std::set< ParticleSystem*,ltps> particleSystems;
public:
	ParticleManager(void);
	~ParticleManager(void);
public:
	void Init();
	void AddSystem( ParticleSystem*);
	void RemoveSystem( ParticleSystem*);
	void Update(float dtime);
	void Render();
	void Shutdown();
	bool DoesExist();

};
}