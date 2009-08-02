#include "stdafx.h"
#include "World.h"
#include "MessagePlaySound.h"
#include "Pickups.h"

PickupItem::PickupItem( World *_world ) : world(_world)
{
	ASSERT(world, "Null member: world");
}

void PickupItem::playSound( const FileName &fileName ) const
{
	BlackBoard &bb = getWorld().getParentBlackBoard();
	bb.relayGlobalMessage(MessagePlaySound(fileName));
}

void PickupItem::relayMessage( OBJECT_ID id, Message &message )
{
	getWorld().getObjects().get(id).relayMessage(message);
}

void PickupItem::spawnParticleSystem(const FileName &fileName,
									 const vec3 &position)
{
	TextureFactory &textureFactory = getWorld().getTextureFactory();
	
	getWorld().particleEngine.add(fileName,
	                              position,
								  0.0f,
								  textureFactory);
}
