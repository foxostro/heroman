#include "stdafx.h"
#include "Filename.h"
#include "SpecialAbility.h"
#include "SpecialAbility_SpeedUp.h"
#include "ComponentMovement.h"
#include "ComponentHealth.h"

SpecialAbility_SpeedUp::SpecialAbility_SpeedUp(ActorPtr a, string n, World *w)
	: SpecialAbility(a, n, w)
{
	type = SpeedUp;
	bIsActive = false;
	bIsCharged = false;
}

SpecialAbility_SpeedUp::~SpecialAbility_SpeedUp(void)
{
}

bool SpecialAbility_SpeedUp::executeAbility()
{
	//make sure ability is not already being executed and is fully charged
	if (bIsActive || !bIsCharged)
		return false;

	ComponentMovement *movement = dynamic_cast<ComponentMovement *>(myActor->getComponent("Movement"));
	
	//store the unmodified speed of the actor; update new speed
	originalSpeed = movement->getTopSpeed();
	originalCool = movement->getWeapon()->getCoolDown();
	movement->setTopSpeed(originalSpeed * speedFactor);
	movement->getWeapon()->setCoolDown(originalCool / speedFactor);

	//set active, start countdown, and erase charge meter
	timeLeft = duration;
	currentCharge = 0.0f;
	bIsActive = true;
	bIsCharged = false;

	return true;
}

void SpecialAbility_SpeedUp::display()
{

}

void SpecialAbility_SpeedUp::onLoad(const PropertyBag &data)
{
	//load necessary data to build ability
	speedFactor = data.getFloat("speedFactor");
	duration = (unsigned int)data.getInt("duration");
}

void SpecialAbility_SpeedUp::update(float milliseconds)
{
	//update countdown on effect if currently active
	if (bIsActive)
	{
		timeLeft -= (int)milliseconds;

		//check time has expired on effect; if so, reset necessary values
		if (timeLeft <= 0)
		{
			ComponentMovement *movement = dynamic_cast<ComponentMovement *>(myActor->getComponent("Movement"));
			movement->setTopSpeed(originalSpeed);
			movement->getWeapon()->setCoolDown(originalCool);
			currentCharge = (float)initialCharge;
			bIsActive = false;
			bIsCharged = false;
		}
	}
	else if (!bIsCharged)
	{
		//update charge meter if it isn't full yet
		chargeByType(milliseconds);
	}
	else
		executeAbility();

	lastHealth = (dynamic_cast<ComponentHealth *>(myActor->getComponent("Health")))->getHealth();
}

void SpecialAbility_SpeedUp::chargeByDamageReceived(float milliseconds)
{
	int currentHealth = (dynamic_cast<ComponentHealth *>(myActor->getComponent("Health")))->getHealth();
	int damage = lastHealth - currentHealth;

	if (damage < 0)
		damage = 0;

	currentCharge += chargeTypes["ByDamageReceived"] * damage;

	if (currentCharge >= chargeMeter)
	{
		bIsCharged = true;
		currentCharge = 100.0;
	}

	/*
	timePassed += milliseconds;

	if (timePassed >= 1000.0)
	{
		timePassed = 0.0;
		currentCharge += chargeTypes["ByTimePassed"];
		if (currentCharge >= chargeMeter)
		{
			bIsCharged = true;
			currentCharge = 100.0;
		}
	}
	*/
}
