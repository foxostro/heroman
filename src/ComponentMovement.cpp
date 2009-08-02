#include "stdafx.h"
#include "World.h"
#include "GraphicsDevice.h"
#include "MessageChangeAnimation.h"
#include "MessageRequestPositionBeSet.h"
#include "MessageCharacterHasDied.h"
#include "MessageCharacterHasBeenResurrected.h"
#include "MessageCharacterUpgradesWeapon.h"
#include "MessageRequestLookAt.h"
#include "ComponentPhysicsBody.h"
#include "WeaponItem.h"
#include "WeaponItem_PlasmaPistol.h"
#include "WeaponItem_TriShot.h"
#include "WeaponItem_GrenadeLauncher.h"
#include "WeaponItem_ChargeShot.h"
#include "ComponentMovement.h"
#include "ComponentDeathBehavior.h"

ComponentMovement::ComponentMovement(BlackBoardUID _uid,  BlackBoard *_bb)
: Component(_uid, _bb),
  amotor(0),
  lmotor(0),
  world(0),
  lastAction(Stand),
  dead(false),
  currentWeapon(0),
  disablePhysics(false)
{
	registerMessageHandler("MessagePerformAction",                bind(&ComponentMovement::handleMessagePerformAction, this, _1));
	registerMessageHandler("MessageSetWorld",                     bind(&ComponentMovement::handleMessageSetWorld, this, _1));
	registerMessageHandler("MessageCharacterHasBeenResurrected",  bind(&ComponentMovement::handleMessageCharacterHasBeenResurrected, this, _1));
	registerMessageHandler("MessageCharacterHasDied",             bind(&ComponentMovement::handleMessageCharacterHasDied, this, _1));
	registerMessageHandler("MessageRequestCharacterFaceAngle",    bind(&ComponentMovement::handleMessageRequestCharacterFaceAngle, this, _1));
	registerMessageHandler("MessageRequestCharacterTurn",         bind(&ComponentMovement::handleMessageRequestCharacterTurn, this, _1));
	registerMessageHandler("MessageCharacterUpgradesWeapon",      bind(&ComponentMovement::handleMessageCharacterUpgradesWeapon, this, _1));
	registerMessageHandler("MessageCharacterDowngradesWeapon",    bind(&ComponentMovement::handleMessageCharacterDowngradesWeapon, this, _1));
	registerMessageHandler("MessagePhysicsDisable",               bind(&ComponentMovement::handleMessagePhysicsDisable, this, _1));
	registerMessageHandler("MessagePhysicsEnable",                bind(&ComponentMovement::handleMessagePhysicsEnable, this, _1));	

	stringToWType["PlasmaPistol"] = Weapon_PlasmaPistol;
	stringToWType["PlasmaPistol-Enemy"] = Weapon_Enemy_PlasmaPistol;
	stringToWType["TriShot"] = Weapon_TriShot;
	stringToWType["GrenadeLauncher"] = Weapon_GrenadeLauncher;
	stringToWType["ChargeShot"] = Weapon_ChargeShot;
	// TODO: Add more weapon names here

	resetMembers();
}

void ComponentMovement::resetMembers()
{
	dead = false;
	currentWeapon = 0;
	weapons.clear();

	velocity = vec3(0,0,0);
	facingAngle = 0.0f;

	lastAction = Stand;

	if(amotor) dJointDestroy(amotor);
	if(lmotor) dJointDestroy(lmotor);
}

void ComponentMovement::load(const PropertyBag &data)
{
	createMotorJoints();


	topSpeed = data.getFloat("topSpeed");

	loadWeapons(data.getBag("weapons"));

}

void ComponentMovement::update(float milliseconds)
{
	// Set model orientation properly
	{
		MessageRequestLookAt m(vec3(getDirectionVector(getFacingAngle()), 0.0f));
		getParentBlackBoard().relayMessage(m);
	}

	// Request animation change based on walk speed
	if(!dead || getDeathBehavior()==Ghost)
	{
		const string anim = determineCurrentAnim();
		getParentBlackBoard().relayMessage(MessageChangeAnimation(anim));

		// Move the character
		dJointSetLMotorParam(lmotor, dParamVel,  velocity.x);
		dJointSetLMotorParam(lmotor, dParamVel2, velocity.y);
	}

	// Update weapon for cool down and FX, etc
	getWeapon()->update(milliseconds);
	
	// house keeping
	velocity.zero();
	lastAction = Stand; 
}

string ComponentMovement::determineCurrentAnim() const
{
	if(lastAction == Stand)
	{
		return "idle";
	}
	else
	{
		return "run";
	}
}

void ComponentMovement::handleMessagePerformAction(Message &m)
{
	applyAction(m.getField<Action>("action"));
}

void ComponentMovement::handleMessageRequestCharacterFaceAngle(Message &m)
{
	turnToFace(m.getField<float>("facingAngle"));
}

void ComponentMovement::handleMessageRequestCharacterTurn(Message &m)
{
	turn(m.getField<float>("dFacingAngle"));
}

void ComponentMovement::handleMessageSetWorld(Message &m)
{
	world = m.getField<World*const>("world");
	physicsEngine = world->getPhysicsEngine();
}

void ComponentMovement::applyAction(Action action)
{
	vec2 x(-1,  1);
	vec2 y(-1, -1);

	if(!dead || getDeathBehavior()==Ghost)
	{
		switch(action)
		{
		/* Step in an absolute direction */
		case StepNorth:     walk(y);    turnToFace(velocity.xy()); break;
		case StepSouth:     walk(-y);   turnToFace(velocity.xy()); break;
		case StepEast:      walk(x);    turnToFace(velocity.xy()); break;
		case StepWest:      walk(-x);   turnToFace(velocity.xy()); break;
		case StepNorthEast: walk(y+x);  turnToFace(velocity.xy()); break;
		case StepNorthWest: walk(y-x);  turnToFace(velocity.xy()); break;
		case StepSouthEast: walk(-y+x); turnToFace(velocity.xy()); break;
		case StepSouthWest: walk(-y-x); turnToFace(velocity.xy()); break;

		/* Step in a relative direction */
		case StepForward:       walk(getDirectionVector(getFacingAngle()));                        break;
		case StepForwardRight:  walk(getDirectionVector(getFacingAngle() - (float)(M_PI * 0.25))); break;
		case StepRight:         walk(getDirectionVector(getFacingAngle() - (float)(M_PI * 0.50))); break;
		case StepBackWardRight: walk(getDirectionVector(getFacingAngle() - (float)(M_PI * 0.75))); break;
		case StepBackWard:      walk(getDirectionVector(getFacingAngle() - (float)M_PI));          break;
		case StepBackWardLeft:  walk(getDirectionVector(getFacingAngle() + (float)(M_PI * 0.75))); break;
		case StepLeft:          walk(getDirectionVector(getFacingAngle() + (float)(M_PI * 0.50))); break;
		case StepForwardLeft:   walk(getDirectionVector(getFacingAngle() + (float)(M_PI * 0.25))); break;

		/* Other */
		case BeginChargeUp: chargeUp(); break;
		case EndChargeUp: endChargeUp(); break;
		case AttackOnce: attackOnce(); break;
		case Suicide: suicide(); break;

		default: break;
		}
		lastAction = action;
	}
	else
	{
		lastAction = InvalidAction;
	}
}

void ComponentMovement::endChargeUp()
{
	if(!dead)
	{
		getWeapon()->endChargeUp();
	}
}

void ComponentMovement::chargeUp()
{
	if(!dead)
	{
		getWeapon()->beginChargeUp();
	}
}

void ComponentMovement::attackOnce()
{
	if(!dead)
	{
		getWeapon()->beginChargeUp();
		getWeapon()->endChargeUp();
	}
}

void ComponentMovement::walk(const vec2 &direction, float speed)
{
	ASSERT(speed >= -1.0f, "Speed is too negative: " + ftos(speed));
	ASSERT(speed <= +1.0f, "Speed is too positive: " + ftos(speed));

	const float modifiedSpeed = speed * topSpeed;
	vec3 dv = vec3(direction, 0.0f).getNormal() * modifiedSpeed;

	velocity = velocity + dv;
}

void ComponentMovement::handleMessageCharacterHasBeenResurrected(Message&)
{
	dead = false;
}

void ComponentMovement::handleMessageCharacterHasDied(Message &message)
{
	dead = true;
	velocity.zero();
	dJointSetLMotorParam(lmotor, dParamVel,  velocity.x);
	dJointSetLMotorParam(lmotor, dParamVel2, velocity.y);
	getParentBlackBoard().relayMessage(MessageChangeAnimation("dying"));
}

WeaponType ComponentMovement::getWeaponTypeFromName(const string &s) const
{
	map<string, WeaponType>::const_iterator i = stringToWType.find(s);
	VERIFY(i != stringToWType.end(), "Cannot identify weapon type: " + s);
	return i->second;
}

shared_ptr<WeaponItem> ComponentMovement::loadWeapon(const PropertyBag &data)
{
	string name = data.getString("name");
	FileName file = FileName("data/weaponDefs").append(FileName(name+".xml"));
	PropertyBag weaponData = PropertyBag::fromFile(file);

	shared_ptr<WeaponItem> weapon = weaponFactory(name);
	weapon->load(weaponData);

	return weapon;
}

shared_ptr<WeaponItem>
ComponentMovement::weaponFactory(const string &name) const
{
	shared_ptr<WeaponItem> weapon;
	WeaponType type = getWeaponTypeFromName(name);
	switch(type)
	{
	case Weapon_Enemy_PlasmaPistol:
	case Weapon_PlasmaPistol:
		weapon = shared_ptr<WeaponItem>(new WeaponItem_PlasmaPistol(world, getActorPtr()));
		break;

	case Weapon_TriShot:
		weapon = shared_ptr<WeaponItem>(new WeaponItem_TriShot(world, getActorPtr()));
		break;

	case Weapon_GrenadeLauncher:
		weapon = shared_ptr<WeaponItem>(new WeaponItem_GrenadeLauncher(world, getActorPtr()));
		break;

	case Weapon_ChargeShot:
		weapon = shared_ptr<WeaponItem>(new WeaponItem_ChargeShot(world, getActorPtr()));
		break;
	}
	return weapon;
}

void ComponentMovement::loadWeapons( const PropertyBag &data )
{
	weapons.clear();
	size_t numberOfWeapons = data.getNumInstances("weapon");
	weapons.resize(numberOfWeapons);
	for(size_t i=0; i<numberOfWeapons; ++i)
	{
		weapons[i] = loadWeapon(data.getBag("weapon", i));
	}
}

void ComponentMovement::handleMessageCharacterUpgradesWeapon( Message &m )
{
	currentWeapon = min(currentWeapon+1, (int)weapons.size()-1);
}

void ComponentMovement::handleMessageCharacterDowngradesWeapon(Message &m)
{
	currentWeapon = max(currentWeapon-1, 0);
}

shared_ptr<WeaponItem> ComponentMovement::getWeapon()
{
	ASSERT(currentWeapon >= 0 && currentWeapon < (int)weapons.size(),
		"currentWeapon is invalid: " + itos(currentWeapon));
	ASSERT(weapons[currentWeapon], "current weapon is null");
	return weapons[currentWeapon];
}

void ComponentMovement::suicide()
{
	getParentBlackBoard().relayMessage(MessageCharacterHasDied());
}

DeathBehavior ComponentMovement::getDeathBehavior() const
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

void ComponentMovement::turn( float dAngle )
{
	if(!dead || getDeathBehavior()==Ghost)
	{
		facingAngle = angle_clamp(facingAngle + dAngle);
	}
}

void ComponentMovement::handleMessagePhysicsDisable( Message &message )
{
	disablePhysics = true;
}

void ComponentMovement::handleMessagePhysicsEnable( Message &message )
{
	if(disablePhysics)
	{
		disablePhysics = false;

		// recreated motr joints
		createMotorJoints();
	}
}

void ComponentMovement::createMotorJoints()
{
	ASSERT(physicsEngine, "Null pointer: physicsEngine");

	const ComponentPhysics &rigidBody =
		getPrerequisiteComponent<ComponentPhysics>("Physics");

	dBodyID body = rigidBody.getBody();

	ASSERT(body, "Cannot create motor joints: No physics body available");

	amotor = dJointCreateAMotor(physicsEngine->getWorld(), 0);
	dJointAttach(amotor, body, 0);
	dJointSetAMotorNumAxes(amotor, 3);
	dJointSetAMotorAxis (amotor, 0, 1, 1, 0, 0);
	dJointSetAMotorAxis (amotor, 1, 1, 0, 1, 0);
	dJointSetAMotorAxis (amotor, 2, 1, 0, 0, 1);
	dJointSetAMotorParam(amotor, dParamFMax,  2000);
	dJointSetAMotorParam(amotor, dParamFMax2, 2000);
	dJointSetAMotorParam(amotor, dParamFMax3, 2000);
	dJointSetAMotorParam(amotor, dParamVel,  0);
	dJointSetAMotorParam(amotor, dParamVel2, 0);
	dJointSetAMotorParam(amotor, dParamVel3, 0);

	lmotor = dJointCreateLMotor(physicsEngine->getWorld(), 0);
	dJointAttach(lmotor, body, 0);
	dJointSetLMotorNumAxes(lmotor, 2);
	dJointSetLMotorAxis (lmotor, 0, 1, 1, 0, 0);
	dJointSetLMotorAxis (lmotor, 1, 1, 0, 1, 0);
	dJointSetLMotorParam(lmotor, dParamFMax,  2000);
	dJointSetLMotorParam(lmotor, dParamFMax2, 2000);
	dJointSetLMotorParam(lmotor, dParamVel,  0);
	dJointSetLMotorParam(lmotor, dParamVel2, 0);
}