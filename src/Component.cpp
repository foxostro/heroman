#include "stdafx.h"
#include "Actor.h"
#include "Component.h"
#include "ComponentUserControllable.h"
#include "ComponentRenderAsModel.h"
#include "ComponentRenderAsPhysicsGeom.h"
#include "ComponentBigSwitchDevice.h"
#include "ComponentRenderAsLamp.h"
#include "ComponentModelSetOnPlayerNumber.h"
#include "ComponentPhysicsBody.h"
#include "ComponentPhysicsGeom.h"
#include "ComponentMovement.h"
#include "ComponentPlaySoundOnUse.h"
#include "ComponentObjectCanBeUsed.h"
#include "ComponentIsSwitch.h"
#include "ComponentSwitchReceiver.h"
#include "ComponentGate.h"
#include "ComponentExitMapOnUse.h"
#include "ComponentBrainShooter.h"
#include "ComponentIsPickupItem.h"
#include "ComponentUseOnCollision.h"
#include "ComponentHealth.h"
#include "ComponentDamageOnCollision.h"
#include "ComponentDestroySelfOnCollision.h"
#include "ComponentObjectApproachable.h"
#include "ComponentAttachParticleSystem.h"
#include "ComponentExplodeOnDeath.h"
#include "ComponentExplodeAfterTimeout.h"
#include "ComponentDeathBehavior.h"
#include "ComponentHighlightOnApproach.h"
#include "ComponentMonsterSpawn.h"
#include "ComponentSpinAround.h"
#include "ComponentScorable.h"
#include "ComponentDropsLoot.h"
#include "ComponentBrainKamikaze.h"
#include "ComponentSoundOnDeath.h"
#include "ComponentSpecialAbility.h"

Component* Component::createComponent(const string &name,
									  BlackBoardUID uid,
									  BlackBoard *blackBoard)
{
	AllocMap nameToAlloc;

	nameToAlloc["UserControllable"]       = bind(&Component::newComponent<ComponentUserControllable>, _1, _2);
	nameToAlloc["RenderAsModel"]          = bind(&Component::newComponent<ComponentRenderAsModel>, _1, _2);
	nameToAlloc["ModelSetOnPlayerNumber"] = bind(&Component::newComponent<ComponentModelSetOnPlayerNumber>, _1, _2);
	nameToAlloc["RenderAsPhysicsGeom"]    = bind(&Component::newComponent<ComponentRenderAsPhysicsGeom>, _1, _2);
	nameToAlloc["Movement"]               = bind(&Component::newComponent<ComponentMovement>, _1, _2);
	nameToAlloc["PlaySoundOnUse"]         = bind(&Component::newComponent<ComponentPlaySoundOnUse>, _1, _2);
	nameToAlloc["BigSwitchDevice"]        = bind(&Component::newComponent<ComponentBigSwitchDevice>, _1, _2);
	nameToAlloc["RenderAsLamp"]           = bind(&Component::newComponent<ComponentRenderAsLamp>, _1, _2);
	nameToAlloc["PhysicsGeom"]            = bind(&Component::newComponent<ComponentPhysicsGeom>, _1, _2);
	nameToAlloc["PhysicsBody"]            = bind(&Component::newComponent<ComponentPhysicsBody>, _1, _2);
	nameToAlloc["ObjectCanBeUsed"]        = bind(&Component::newComponent<ComponentObjectCanBeUsed>, _1, _2);
	nameToAlloc["IsSwitch"]               = bind(&Component::newComponent<ComponentIsSwitch>, _1, _2);
	nameToAlloc["SwitchReceiver"]         = bind(&Component::newComponent<ComponentSwitchReceiver>, _1, _2);
	nameToAlloc["Gate"]                   = bind(&Component::newComponent<ComponentGate>, _1, _2);
	nameToAlloc["ExitMapOnUse"]           = bind(&Component::newComponent<ComponentExitMapOnUse>, _1, _2);
	nameToAlloc["BrainShooter"]           = bind(&Component::newComponent<ComponentBrainShooter>, _1, _2);
	nameToAlloc["IsPickupItem"]			  =	bind(&Component::newComponent<ComponentIsPickupItem>, _1, _2);
	nameToAlloc["UseOnCollision"]         = bind(&Component::newComponent<ComponentUseOnCollision>, _1, _2);
	nameToAlloc["Health"]                 = bind(&Component::newComponent<ComponentHealth>, _1, _2);
	nameToAlloc["DamageOnCollision"]      = bind(&Component::newComponent<ComponentDamageOnCollision>, _1, _2);
	nameToAlloc["DestroySelfOnCollision"] = bind(&Component::newComponent<ComponentDestroySelfOnCollision>, _1, _2);
	nameToAlloc["ObjectApproachable"]     = bind(&Component::newComponent<ComponentObjectApproachable>, _1, _2);
	nameToAlloc["AttachParticleSystem"]   = bind(&Component::newComponent<ComponentAttachParticleSystem>, _1, _2);
	nameToAlloc["ExplodeOnDeath"]         = bind(&Component::newComponent<ComponentExplodeOnDeath>, _1, _2);
	nameToAlloc["ExplodeAfterTimeout"]    = bind(&Component::newComponent<ComponentExplodeAfterTimeout>, _1, _2);
	nameToAlloc["DeathBehavior"]          = bind(&Component::newComponent<ComponentDeathBehavior>, _1, _2);
	nameToAlloc["HighlightOnApproach"]    = bind(&Component::newComponent<ComponentHighlightOnApproach>, _1, _2);
	nameToAlloc["MonsterSpawn"]			  = bind(&Component::newComponent<ComponentMonsterSpawn>, _1, _2);
	nameToAlloc["SpinAround"]             = bind(&Component::newComponent<ComponentSpinAround>, _1, _2);
	nameToAlloc["Scorable"]               = bind(&Component::newComponent<ComponentScorable>, _1, _2);
	nameToAlloc["DropsLoot"]              = bind(&Component::newComponent<ComponentDropsLoot>, _1, _2);
	nameToAlloc["BrainKamikaze"]          = bind(&Component::newComponent<ComponentBrainKamikaze>, _1, _2);
	nameToAlloc["SoundOnDeath"]           = bind(&Component::newComponent<ComponentSoundOnDeath>, _1, _2);
	nameToAlloc["SpecialAbility"]		  = bind(&Component::newComponent<ComponentSpecialAbility>, _1, _2);

	AllocMap::const_iterator i = nameToAlloc.find(name);

	VERIFY(i!=nameToAlloc.end(),"Could not create component \"" + name + "\"");

	return (i->second)(uid, blackBoard);
}

Actor* Component::getActorPtr() const
{
	return dynamic_cast<Actor*>(getParentBlackBoardPtr());
}
