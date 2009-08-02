#include "stdafx.h"
#include "ParticleSystem.h"
#include "ParticleEngine.h"

void ParticleEngine::draw() const
{
	for(list<ParticleSystem*>::const_iterator i = particles.begin();
		i != particles.end(); ++i)
	{
		ParticleSystem *system = *i;
		system->draw();
	}
}

void ParticleEngine::update(float milliseconds, Camera &camera)
{
	list<ParticleSystem*>::iterator i = particles.begin();

	while(i != particles.end())
	{
		ParticleSystem *system = *i;

		system->update(milliseconds, camera);

		if(system->isDead())
		{
			i = particles.erase(i);
		}
		else
		{
			++i;
		}
	}
}

ParticleEngine::handle ParticleEngine::add(const FileName &fileName,
										   const vec3 &position,
										   float rotation,
										   TextureFactory &textureFactory)
{
	return particles.insert(particles.begin(),
	                        new ParticleSystem(fileName,
	                                           textureFactory,
	                                           position,
	                                           rotation));
}
