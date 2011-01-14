/**
* @file ParticleManager.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date Januar 2010
* @version unstable
* @brief A particle manager that keeps track of all Particle Systems in the scene
* @namespace echtzeitlu
*/
#pragma once

#include "ParticleSystem.h"
#include <set>
#include <cstring>

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
