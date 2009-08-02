#ifndef COMPONENT_SPECIAL_ABILITY_H
#define COMPONENT_SPECIAL_ABILITY_H

#include "PropertyBag.h"
#include "Blackboard.h"
#include "Component.h"
#include "World.h"
#include "Actor.h"
#include "SpecialAbility.h"
#include "SpecialAbility_SpeedUp.h"

/**
This component describes special abilities (different from normal
attacks) that may be deployed by the actor.
*/
class ComponentSpecialAbility : public Component
{
public:
	/* -- Functions -- */
	/** Constructor */
	ComponentSpecialAbility(BlackBoardUID uid, BlackBoard *blackBoard);

	/** Destructor */
	~ComponentSpecialAbility(void);

	/** Returns type of component in string */
	virtual string getTypeString() const { return "SpecialAbility"; }

	/** Resets all object members to defaults */
	virtual void resetMembers();

	/** Loads component data from the pool of all object data */
	virtual void load(const PropertyBag &data);

	/** Draws the object */
	virtual void draw() const;

	/** Updates the object */
	virtual void update(float milliseconds);

	/** 
	Checks if a specified ability is active or not.
	@param name the name of the ability to check
	*/
	bool checkActive(string name);

	/**
	Returns the currently selected ability.
	@return a pointer to the currently selected ability (NULL on error)
	
	inline bool getCurrentAbility(shared_ptr<SpecialAbility> sa)
	{
		if (currentAbility != NULL)
		{
			sa = currentAbility->second;
			return true;
		}
		else
			return shared_ptr<SpecialAbility>();
		return false;
	}
	*/

private:
	/* -- Functions -- */
	/** Message handler callbacks */
	void handleMessageSetWorld(Message &m);
	void handleMessagePassFont(Message &m);
	void handleMessagePositionHasBeenSet(Message &m);
	void handleMessageRequestSetHeight(Message &m);
	void handleMessageCharacterReceivesDamage(Message &m);

	/**
	Helper function for load. It loads all the abilities specified in the component's
	definition, storing them in the abilities map.
	@param data the component data to be read
	*/
	void loadAbilities(const PropertyBag &data);

	/**
	Helper function for load. It loads an ability from an abilityDef .xml file.
	@param data the data being loaded
	@return a pointer to the new ability
	*/
	shared_ptr<SpecialAbility> loadAbility(const PropertyBag &data);

	/**
	Creates a new instance of the specified ability.
	@param name the name to give the new ability
	@param type the AbilityType
	@param a a pointer to the actor who owns the ability
	@param w a pointer to the world
	@return a pointer to the new ability
	*/
	shared_ptr<SpecialAbility> ComponentSpecialAbility::createAbility(const string &name, AbilityType t, ActorPtr a, World *w) const;

	/**
	Draws the ability charge meter for the current ability near the
	character. Helper function for draw() function.
	@param m the camera's orientation
	*/
	void ComponentSpecialAbility::drawChargeMeter(const mat3 &m) const;

private:
	/* -- Instance Variables -- */
	map<string, AbilityType> stringToType;											/** map to convert from string to AbilityType */

	map<AbilityType, shared_ptr<SpecialAbility> > abilities;						/** a list of all this character's abilities */
	map<AbilityType, shared_ptr<SpecialAbility> >::const_iterator currentAbility;	/** a pointer to the currently selected ability */
	World *world;																	/** pointer to the game world */
	shared_ptr<Font> font;															/** pointer to the game font */

	vec3 lastReportedPosition;		/** actor's last reported position */
	float lastReportedHeight;		/** actor's last reported height */
};

#endif
