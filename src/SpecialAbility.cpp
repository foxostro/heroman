#include "stdafx.h"
#include "SpecialAbility.h"

SpecialAbility::SpecialAbility(ActorPtr a, string n, World *w)
{
	myActor = a;
	name = n;
	world = w;
}

SpecialAbility::~SpecialAbility(void)
{
}

void SpecialAbility::load(const PropertyBag &data)
{
	FileName file = FileName("data_debug.xml");
	data.saveToFile(file);

	string chargeString = data.getString("chargeType");
	
	//determine the way(s) in which this ability updates its charge meter and
	//load any necessary data
	if (chargeString.find("ByTimePassed") != string::npos)
		chargeTypes["ByTimePassed"]  = data.getFloat("chargeOnTimePassedValue");
	if (chargeString.find("ByDamageReceived") != string::npos)
		chargeTypes["ByDamageReceived"] = data.getFloat("chargeOnDamageReceivedValue");
	if (chargeString.find("ByScore") != string::npos)
		chargeTypes["ByScore"] = data.getFloat("chargeOnScoreValue");

	//initialize charge meter
	chargeMeter = data.getInt("totalCharge");
	initialCharge = data.getInt("initialCharge");
	currentCharge = (float)initialCharge;

	//load subclass data
	onLoad(data);
}

void SpecialAbility::chargeByType(float milliseconds)
{
	if (chargeTypes.find("ByTimePassed") != chargeTypes.end())
		chargeByTimePassed(milliseconds);
	if (chargeTypes.find("ByDamageReceived") != chargeTypes.end())
		chargeByDamageReceived(milliseconds);
	if (chargeTypes.find("ByScore") != chargeTypes.end())
		chargeByScore(milliseconds);
}

