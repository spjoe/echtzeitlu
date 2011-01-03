#include "ParticleManager.h"
using namespace echtzeitlu;

ParticleManager::ParticleManager(void)
{
}


ParticleManager::~ParticleManager(void)
{
}

void ParticleManager::Init()
{
}

void ParticleManager::AddSystem(ParticleSystem* ps)
{
	particleSystems.insert(ps);
}
void ParticleManager::RemoveSystem(ParticleSystem* ps)
{
	//particleSystems.erase(particleSystems.find(ps));
}

void ParticleManager::Update(float dtime)
{
	for(std::set<ParticleSystem*,ltps>::iterator iter = particleSystems.begin();
		iter != particleSystems.end(); iter++)
	{
		ParticleSystem* tmp = *iter;

		if(tmp->Update(dtime)){

		}
	}
}
void ParticleManager::Render()
{
}
bool ParticleManager::DoesExist()
{
	return false;
}
void ParticleManager::Shutdown()
{
}

