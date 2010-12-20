#include "ParticleManager.h"


ParticleManager::ParticleManager(void)
{
}


ParticleManager::~ParticleManager(void)
{
}

void ParticleManager::Init()
{
}

void ParticleManager::AddSystem(ParticleSystem& ps)
{
	//particleSystems.push_back(ps);
}
void ParticleManager::RemoveSystem(const ParticleSystem& ps)
{

}

void ParticleManager::Update()
{
	for(std::vector<ParticleSystem>::iterator iter = particleSystems.begin();
		iter != particleSystems.end(); iter++)
	{
		if(!(*iter).Update()){

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

