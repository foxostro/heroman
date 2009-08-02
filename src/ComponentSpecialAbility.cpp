#include "stdafx.h"
#include "GraphicsDevice.h"
#include "Color.h"
#include "Font.h"
#include "MessageSetWorld.h"
#include "MessagePassFont.h"
#include "MessagePositionHasBeenSet.h"
#include "MessageRequestSetHeight.h"
#include "ComponentSpecialAbility.h"

/** Constructor */
ComponentSpecialAbility::ComponentSpecialAbility(BlackBoardUID uid,
									 BlackBoard *blackBoard)
: Component(uid, blackBoard)
{
	//register message handler callbacks
	registerMessageHandler("MessageSetWorld",			     bind(&ComponentSpecialAbility::handleMessageSetWorld, this, _1));
	registerMessageHandler("MessagePassFont",			     bind(&ComponentSpecialAbility::handleMessagePassFont, this, _1));
	registerMessageHandler("MessagePositionHasBeenSet",	     bind(&ComponentSpecialAbility::handleMessagePositionHasBeenSet, this, _1));
	registerMessageHandler("MessageRequestSetHeight",	     bind(&ComponentSpecialAbility::handleMessageRequestSetHeight, this, _1));
	registerMessageHandler("MessageCharacterReceivesDamage", bind(&ComponentSpecialAbility::handleMessageCharacterReceivesDamage, this, _1));

	//initialize string to AbilityType converter
	stringToType["SpeedUp"] = SpeedUp;
	stringToType["SlowEnemies"] = SlowEnemies;
	stringToType["ImmobilizeEnemies"] = ImmobilizeEnemies;
	stringToType["InvertDamage"] = InvertDamage;
	stringToType["DoubleHealth"] = DoubleHealth;
	stringToType["DoubleAttackPower"] = DoubleAttackPower;
	stringToType["OneShotKill"] = PoisonAttack;
	stringToType["Regen"] = Invincible;
	stringToType["Explosion"] = Explosion;
}

/** Destructor */
ComponentSpecialAbility::~ComponentSpecialAbility()
{
	abilities.clear();
}

void ComponentSpecialAbility::resetMembers()
{
	abilities.clear();
	currentAbility = abilities.end(); // intentionally invalid iterator
	lastReportedPosition.zero();
	lastReportedHeight = 1.0f;
}

void ComponentSpecialAbility::loadAbilities(const PropertyBag &data)
{
	abilities.clear();

	//load all the abilities mentioned int the data file
	size_t numAbilities = data.getNumInstances("ability");
	for (size_t i = 0; i < numAbilities; i++)
	{
		shared_ptr<SpecialAbility> newAbility = loadAbility(data.getBag("ability", i));
		abilities[newAbility->getType()] = newAbility;
	}

	//set the current ability to initially be the first in the list
	currentAbility = abilities.begin();
}

void ComponentSpecialAbility::load(const PropertyBag &data)
{
	resetMembers();
	loadAbilities(data.getBag("abilities"));
}

shared_ptr<SpecialAbility> ComponentSpecialAbility::loadAbility(const PropertyBag &data)
{
	//get filename for abilityDef .xml file
	string name = data.getString("name");
	FileName file = FileName("data/abilityDefs").append(FileName(name+".xml"));
	PropertyBag abilityData = PropertyBag::fromFile(file);

	//get ability's type
	string typeString = abilityData.getString("type");
	AbilityType type;
	map<string, AbilityType>::iterator iter = stringToType.find(typeString);
	ASSERT(iter != stringToType.end(), "Invalid AbilityType specified!");
	type = iter->second;

	//create the ability and load necessary data
	shared_ptr<SpecialAbility> ability = createAbility(name, type, getActorPtr(), world);
	ability->load(abilityData);

	return ability;
}

shared_ptr<SpecialAbility> ComponentSpecialAbility::createAbility(const string &name, AbilityType t, ActorPtr a, World *w) const
{
	shared_ptr<SpecialAbility> ability;
	AbilityType type = t;
	switch(type){
	case SpeedUp:
		ability = shared_ptr<SpecialAbility>(reinterpret_cast<SpecialAbility *>(new SpecialAbility_SpeedUp(a, name, w)));
		break;

	default:
		break;
	}

	return ability;
}

bool ComponentSpecialAbility::checkActive(string name)
{
	//get pointer to ability
	map<AbilityType, shared_ptr<SpecialAbility> >::const_iterator iter;
	iter = abilities.begin();

	//search for named ability in list
	for (iter = abilities.begin(); iter != abilities.end(); iter++)
	{
		if (iter->second->getName() == name)
			return iter->second->isActive();
	}

	ASSERT(true, "Invalid ability name specified!");
	return false;
}

void ComponentSpecialAbility::draw() const
{
	ASSERT(font, "Null pointer: font");

	CHECK_GL_ERROR();
	
	glPushMatrix();
	glTranslatef(lastReportedPosition.x,
	             lastReportedPosition.y,
	             lastReportedPosition.z + lastReportedHeight);

	mat3 cameraOrientation = GraphicsDevice::getCameraOrientation();
	
	drawChargeMeter(cameraOrientation);

	glPopMatrix();

	CHECK_GL_ERROR();
}

void ComponentSpecialAbility::drawChargeMeter(const mat3 &m) const
{
	if (abilities.empty() || currentAbility == abilities.end())
		return;

	//get charge percentage
	int pct = currentAbility->second->getChargePercentage();

	//construct string to display
	string s = "\n\n" + currentAbility->second->getName() + ": " + itos(pct) + "%";

	//change display color based on percentage
	if (pct <= 40)
		font->billboardWrite(s, white, Font::SizeLarge, m);
	else if (pct <= 70)
		font->billboardWrite(s, yellow, Font::SizeLarge, m);
	else if (pct <= 99)
		font->billboardWrite(s, blue, Font::SizeLarge, m);
	else if (pct <= 101)
	{
		string s = "" + currentAbility->second->getName() + ": READY!";
		font->billboardWrite(s, green, Font::SizeLarge, m);
	}
}

void ComponentSpecialAbility::update(float milliseconds)
{
	//update all the abilities
	map<AbilityType, shared_ptr<SpecialAbility> >::const_iterator iter;
	for (iter = abilities.begin(); iter != abilities.end(); iter++)
		iter->second->update(milliseconds);
}

/** Callback handler for MessageSetWorld. */
void ComponentSpecialAbility::handleMessageSetWorld(Message &m)
{
	world = m.getField<World *>("world");
}

/** Callback handler for MessagePassFont */
void ComponentSpecialAbility::handleMessagePassFont(Message &m)
{
	font = m.getField<shared_ptr<Font> >("font");
}

/** Callback handler for MessagePositionHasBeenSet. */
void ComponentSpecialAbility::handleMessagePositionHasBeenSet(Message &m)
{
	lastReportedPosition = m.getField<vec3>("position");
}

/** Callback handler for MessageRequestSetHeight. */
void ComponentSpecialAbility::handleMessageRequestSetHeight(Message &m)
{
	lastReportedHeight = m.getField<float>("height");
}

void ComponentSpecialAbility::handleMessageCharacterReceivesDamage(Message &m)
{
}