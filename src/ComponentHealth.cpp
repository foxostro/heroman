#include "stdafx.h"
#include "GraphicsDevice.h"
#include "MessageCharacterHasDied.h"
#include "MessageCharacterHasBeenResurrected.h"
#include "MessageCharacterReceivesDamage.h"
#include "MessageCharacterReceivesHealing.h"
#include "MessageCharacterReceivesWeaponPower.h"
#include "MessageCharacterUpgradesWeapon.h"
#include "MessageHighlightModel.h"
#include "MessagePlaySound.h"
#include "MessageZombifyActor.h"
#include "World.h"
#include "ComponentPhysics.h"
#include "ComponentHealth.h"
#include "ComponentDeathBehavior.h"
#include "ComponentSpecialAbility.h"

ComponentHealth::ComponentHealth(BlackBoardUID _uid,
                                 BlackBoard *_blackBoard)
: Component(_uid, _blackBoard),
  world(0)
{
	registerMessageHandler("MessageCharacterHasBeenResurrected",   bind(&ComponentHealth::handleMessageCharacterHasBeenResurrected, this, _1));
	registerMessageHandler("MessageCharacterHasDied",              bind(&ComponentHealth::handleMessageCharacterHasDied, this, _1));
	registerMessageHandler("MessageCharacterReceivesWeaponPower",  bind(&ComponentHealth::handleMessageCharacterReceivesWeaponPower, this, _1));
	registerMessageHandler("MessageCharacterReceivesDamage",       bind(&ComponentHealth::handleMessageCharacterReceivesDamage, this, _1));
	registerMessageHandler("MessageCharacterReceivesHealing",      bind(&ComponentHealth::handleMessageCharacterReceivesHealing, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",            bind(&ComponentHealth::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageRequestSetHeight",              bind(&ComponentHealth::handleMessageRequestSetHeight, this, _1));
	registerMessageHandler("MessagePassFont",                      bind(&ComponentHealth::handleMessagePassFont, this, _1));
	registerMessageHandler("MessageExplosionOccurs",               bind(&ComponentHealth::handleMessageExplosionOccurs, this, _1));
	registerMessageHandler("MessageSetWorld",                      bind(&ComponentHealth::handleMessageSetWorld, this, _1));

	resetMembers();
}

void ComponentHealth::resetMembers()
{
	power = 0;
	health = 100;
	maxHealth = 100;
	dead = false;
	lastReportedPosition.zero();
	lastReportedHeight = 1.0f;
	damageToPowerRatio = 1.0f;
	willResurrectAfterCountDown = false;
	timeSpentDead = 0.0f;
	timeUntilResurrection = 0.0f;
	displayPower = false;
}

void ComponentHealth::update(float deltaTime)
{
	if(!dead) return;

	if((timeSpentDead += deltaTime) >= timeUntilResurrection)
	{
		if(getDeathBehavior() == Disappear)
		{
			MessageZombifyActor m;
			getParentBlackBoard().relayMessage(m);
		}
		else if(willResurrectAfterCountDown)
		{
			MessageCharacterHasBeenResurrected m;
			getParentBlackBoard().relayMessage(m);
		}
	}
}

void ComponentHealth::handleMessagePassFont(Message &message)
{
	font = message.getField<shared_ptr<Font> >("font");
}

void ComponentHealth::handleMessageCharacterHasBeenResurrected(Message &)
{
	dead = false;
	health = max(maxHealth / 2, 1);
	maxHealth = max(maxHealth, 1);
	power = 0;
	timeSpentDead = 0.0f;
}

void ComponentHealth::handleMessageCharacterHasDied(Message &message)
{
	health = 0;
	power = 0;
	dead = true;
	timeSpentDead = 0.0f;
}

void ComponentHealth::handleMessageCharacterReceivesDamage(Message &m)
{
	if(!dead)
	{
		int damage = m.getField<int>("damage");
		loseHealth(damage); // Lose health when damaged
		losePower(damage); // Lose weapon power when damaged

		// Flash the model briefly to signal damage
		{MessageEnableModelHighlight m(HighlightFlash, 300.0f, red, white);
		getParentBlackBoard().relayMessage(m);}
	}
}

void ComponentHealth::handleMessageCharacterReceivesHealing(Message &m)
{
	if(!dead)
	{
		int healing = m.getField<int>("healing");
		health += healing;
		health = min(health, maxHealth);
	}
}

void ComponentHealth::handleMessageCharacterReceivesWeaponPower(Message &m)
{
	if(!dead)
	{
		int p = m.getField<int>("power");
		power += p;

		if(power>100)
		{
			power = 0;
			getParentBlackBoard().relayMessage(MessageCharacterUpgradesWeapon());
		}
	}
}

void ComponentHealth::handleMessagePositionHasBeenSet(Message &message)
{
	lastReportedPosition = message.getField<vec3>("position");
}

void ComponentHealth::handleMessageRequestSetHeight(Message &message)
{
	lastReportedHeight = message.getField<float>("height");
}

void ComponentHealth::load(const PropertyBag &data)
{
	resetMembers();
	health = data.getInt("health");
	maxHealth = data.getInt("maxHealth");
	damageToPowerRatio = data.getFloat("damageToPowerRatio");
	willResurrectAfterCountDown = data.getBool("willResurrectAfterCountDown");
	timeUntilResurrection = data.getFloat("timeUntilResurrection");
	
	data.get("displayPower", displayPower); // optional tag
}

void ComponentHealth::drawTransparentBits() const
{
	ASSERT(font, "Null pointer: font");

	CHECK_GL_ERROR();
	
	glPushMatrix();
	glTranslatef(lastReportedPosition.x,
	             lastReportedPosition.y,
	             lastReportedPosition.z + lastReportedHeight);

	mat3 cameraOrientation = GraphicsDevice::getCameraOrientation();

	if(!dead)
	{
		drawHealthBar(cameraOrientation);
	}
	else if(willResurrectAfterCountDown)
	{
		drawResurrectCountDown(cameraOrientation);
	}

	glPopMatrix();

	CHECK_GL_ERROR();
}

void ComponentHealth::handleMessageExplosionOccurs(Message &message)
{
	int baseDamage = message.getField<int>("baseDamage");

	if(baseDamage <= 0)
		return;

	vec3 pos = message.getField<vec3>("position");
	float distance = vec3(lastReportedPosition - pos).getMagnitude();
	float p = powf((float)M_E, -SQR(distance/1.5f));
	int damage = (int)(baseDamage * p);
	OBJECT_ID actor = message.getField<OBJECT_ID>("actor");

	if(damage <= 0)
		return;

#if 1
	MessageCharacterReceivesDamage m(damage);
	getParentBlackBoard().relayMessage(m);
#else
	// Do a fine-detail check against physics geometry (e.g. walls)
	if(rayCast(actor,
	           lastReportedPosition,
	           pos - lastReportedPosition,
	           distance))
	{
		MessageCharacterReceivesDamage m(damage);
		getParentBlackBoard().relayMessage(m);
	}
#endif
}

void ComponentHealth::handleMessageSetWorld( Message &message )
{
	world = message.getField<World*const>("world");
}

dGeomID ComponentHealth::getGeom(OBJECT_ID actor)
{
	ASSERT(world, "Member \"world\" world was null");
	return getGeom(world->getObjects().get(actor));
}

dGeomID ComponentHealth::getGeom(const Actor &actor)
{
	ComponentPhysics* component =
		dynamic_cast<ComponentPhysics*>(actor.getComponent("Physics"));

	if(!component)
		return 0;
	else
		return component->getGeom();
}

bool ComponentHealth::rayCast(OBJECT_ID actor,
							  const vec3 &p,
							  const vec3 &d,
							  float l)
{
	ASSERT(world, "Member \"world\" world was null");
	return world->getPhysicsEngine()->rayCast(getGeom(actor),
	                                          getGeom(*getActorPtr()),
	                                          p, d, l);
}

void ComponentHealth::losePower(int damage)
{
	int lostPower = (int)(damage * damageToPowerRatio);
	if(lostPower>0)
	{
		power -= lostPower;

		MessageCharacterLosesWeaponPower m(lostPower);
		getParentBlackBoard().relayMessage(m);

		if(power < 0)
		{
			power = 100;
			MessageCharacterDowngradesWeapon n;
			getParentBlackBoard().relayMessage(n);
		}
	}
}

void ComponentHealth::loseHealth(int damage)
{
	health -= damage;

	if(health <= 0)
	{
		getParentBlackBoard().relayMessage(MessageCharacterHasDied());
		health = 0;
	}
}

DeathBehavior ComponentHealth::getDeathBehavior() const
{
	ActorPtr actor = getActorPtr();

	const ComponentDeathBehavior* c = dynamic_cast<const ComponentDeathBehavior*>(actor->getComponent("DeathBehavior"));

	if(c==0)
	{
		return Disappear;
	}
	else
	{
		return c->getDeathBehavior();
	}
}

void ComponentHealth::drawHealthBar(const mat3 &m) const
{
	{
		string s = "HP: " + itos(health) + " / " + itos(maxHealth);
		font->billboardWrite(s, white, Font::SizeLarge, m);
	}

	if(displayPower)
	{
		string s = "\nPOWER: " + itos(power) + "%";
		font->billboardWrite(s, white, Font::SizeLarge, m);
	}
}

void ComponentHealth::drawResurrectCountDown(const mat3 &m) const
{
	static const color c(0.9f, 0.9f, 1.0f, 0.9f);
	float t = timeUntilResurrection - timeSpentDead;
	string s = itos((int)(t / 1000.0f));
	font->billboardWrite(s, c, Font::SizeHuge, m);
}
