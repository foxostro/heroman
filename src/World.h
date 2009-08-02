#ifndef _WORLD_H_
#define _WORLD_H_

#include "vec4.h"
#include "PropertyBag.h"

#include "ActorSet.h"
#include "ActorFactory.h"
#include "Terrain.h"
#include "Map.h"
#include "SkyBox.h"
#include "Camera.h"
#include "Fog.h"
#include "PhysicsEngine.h"
#include "gui.h"
#include "LabelWidget.h"
#include "ParticleEngine.h"
#include "Score.h"

// class prototypes
class Font;
class SDLinput;
class GraphicsDevice;

/**
Game world.
Contains the game world map, the player, game objects, and other entities
*/
class World : public BlackBoardSubscriber
{
public:
	/**
	Constructor
	@param uid UID to identify this blackboard subscriber
	@param parentBlackBoard Communicatin ith game subsystems
	@param graphicsDevice Tracks major graphics device state
	@param textureFactory Tracks loaded textures
	@param input Input event handler for the current game state
	@param font Font in which to print all object text
	*/
	World(BlackBoardUID uid,
		  BlackBoard *parentBlackBoard,
		  GraphicsDevice &graphicsDevice,
		  TextureFactory &textureFactory,
		  SDLinput &input,
		  shared_ptr<Font> font,
		  GUI &gui);

	/** Loads the world from file */
	void loadFromFile(const FileName &fileName);

	/**
	Retrieves the name of the realm
	@return Name of the realm
	*/
	inline const string& getName() const
	{
		return name;
	}

	/**
	Sets the name of the realm
	@param name New name of the realm
	*/
	inline void setName(const string &name)
	{
		this->name = name;
	}

	/**
	Gets the object database
	@return object database
	*/
	inline ActorSet& getObjects()
	{
		return objects;
	}

	/**
	Gets the object database
	@return object database
	*/
	inline const ActorSet& getObjects() const
	{
		return objects;
	}

	/** Destroys all game world assets and resets the game world */
	void destroy();

	/**
	Update the World
	@param deltaTime Time elapsed since the last update
	*/
	void update(float deltaTime);


	/** Draws the scene */
	void draw() const;

	/**
	Gets the most recently calculated mean player position
	@return mean player position
	*/
	inline const vec3& getAveragePlayerPosition() const
	{
		return averagePlayerPosition;
	}

	inline shared_ptr<PhysicsEngine> getPhysicsEngine() const
	{
	    return physicsEngine;
	}

	/** Gets a pointer to the font */
	inline shared_ptr<Font> getFont() const
	{
		return font;
	}

	int getNumberOfCoinsRemaining() const
	{
		return coinsRemaining;
	}

	/** Notify the world that another coin was collected by the party */
	void coinCollected()
	{
		coinsRemaining = max(0, coinsRemaining-1);
	}

	TextureFactory& getTextureFactory()
	{
		return textureFactory;
	}

	const TextureFactory& getTextureFactory() const
	{
		return textureFactory;
	}

	/**
	Generates an explosion in the game world
	@param originator actor that originated the explosion (immune)
	@param position Position of the explosion
	@param rotation Rotation (radians) of the particle system about the Z-axis
	@param baseDamage Base damage before distance falloff
	@param soundFileName File name of the explosion sound effect
	@param particlesFileName Filename of the particle system
	*/
	void generateExplosion(OBJECT_ID originator,
	                       const vec3 &position,
	                       float rotation,
						   int baseDamage,
	                       const FileName &soundFileName,
	                       const FileName &particlesFileName);

	/**
	Players enter the game world; recreate players in the game world
	@param numPlayers Number of players entering the game
	*/
	void playersEnter(int numPlayers);

	/** Gets the world map */
	inline Map& getMap()
	{
		return map;
	}

	/** Gets the world map */
	inline const Map& getMap() const
	{
		return map;
	}

private:
	void onKeyDownUp();
	void onKeyDownDown();
	void onKeyDownLeft();
	void onKeyDownRight();
	void onKeyDownFwd();
	void onKeyDownRev();
	void onKeyDownStrafeLeft();
	void onKeyDownStrafeRight();
	void onKeyToggleCamera();
	void onKeyToggleDebugRendering();
	void onKeyTogglePhysics();

	void handleMessageExitMap(Message &message);
	void handleMessageChangeScore(Message &message);

	/** Enables or disabled ebug rendering */
	void enableDebugRendering(bool displayDebugData);
	
	/** Plays a sound */
	void playSound(const FileName &sound);

	/**
	Loads the world state
	@param xml data source
	*/
	void load(const PropertyBag &xml);

	/**
	Search for the player start point and set the initial camera position
	to focus on that location
	*/
	void cameraLooksAtPlayerStartPoint();

	/** Searches for the player start point and returns its position */
	vec3 getPlayerStartPoint() const;

	/**
	Creates a single player entity from data (Adds it to "objects" set).
	Please note that the position described in the playerData may be
	interpreted as the starting position of the player (single-player) or as
	the locus of the player party (multi-player)  So, a second step where the
	player's are repositioned around the party locus may be necessary after
	the call to this method has completed.
	@param initialPosition Initial player position
	@param playerData Data describing the starting player configuration
	@param playerNumber The player's number
	@param numOfPlayers Number of players in the party (so we can center about
	                    the party position)
	@return Player
	*/
	ActorPtr createPlayer(const vec3 &initialPosition,
	                      const PropertyBag &playerData,
	                      int playerNumber,
						  int numOfPlayers);

	/** Destroys all player characters in the game world */
	void detroyAllPlayers();

	/** Updates the camera positions */
	void updateCamera(float deltaTime);

	/** Harmonizes the camera with the current player positions */
	void updateCamera_Overhead();

	/**
	Sets the camera position and looks at the average player position
	@param theta Camera angle (radians) about the X axis
	@param angleZ Camera angle (radians) about the Z axis
	@param distance Greatest distance from a player to the average position
	@param averagePlayerPosition Average of all player positions
	*/
	void setCameraLook(float theta,
	                   float angleZ,
	                   float distance,
	                   const vec3 &averagePlayerPosition);

	/** Flying 1st person camera */
	void updateCamera_FirstPerson(float deltaTime);

	/** Periodically calculates and caches the average player position */
	inline void recalculateAveragePlayerPosition()
	{
		averagePlayerPosition = findAveragePlayerPosition();
	}

	/**
	Finds the mean position of all the players in the game
	@return average position of all the players
	*/
	vec3 findAveragePlayerPosition() const;

	/** Sets up a simple light source in the world */
	void setupSimpleLight();

	/** Draws the opaque objects in the world and game map */
	void drawObjectsAndMap() const;

	/** Draws the transparent objects in the world */
	void drawTransparentObjects() const;

	/** Draws particles in the game world */
	void drawParticles() const;

	/** Update the label tat displays the number of coins remaining */
	void updateCoinsLabel();

	/** Updates the level score displayed on the main interface. */
	void updateScoreLabel();

	inline void resetKeyFlags()
	{
		w = s = a = d = i = k = j = l = false;
	}

	/** Updates game physics */
	void updatePhysics( float deltaTime );

	/** Process any pending requests to change the map */
	void handleMapChangeRequest();

	/**
	Directly polls the players in the world to determine whether
	game over conditions have occurred.
	*/
	bool isGameOver();

	/** Allow global score modifier &c to update over time */
	void updateLevelScore( float deltaTime );

public:
	/** The camera for the client perspective */
	Camera camera;

	/** References to the players */
	vector<ActorPtr> players;

	/** Particle engine manages all particle systems */
	ParticleEngine particleEngine;

private:
	/** Name of the World */
	string name;

	/** Label indicates the number of coins remaining */
	LabelWidget *coinsLabel;

	/** Label indicating the score */
	LabelWidget *scoreLabel;

	/** Aggregate score of players for this level */
	shared_ptr<Score> levelScore;

	/** Number of coins remaining until the map is unlocked */
	int coinsRemaining;
	
	/** Filename of most recent map data source */
	FileName fileName;

	/** Set of objects that reside within this World */
	ActorSet objects;

	/** Heightmap-based terrain */
	Terrain terrain;

	/** Tile-based rooms and hallways */
	Map map;

	/** SkyBox associated with the world */
	SkyBox skyBox;

	/** Fog associated with the world */
	Fog fog;

	/** Tracks loaded textures */
	TextureFactory &textureFactory;

	/** Input handler for the current game state */
	SDLinput &input;

	/** Font in which to print all object text */
	shared_ptr<Font> font;

	/** Periodically calculates and caches the average player position */
	vec3 averagePlayerPosition;

	/** Physics engine subsystem */
	shared_ptr<PhysicsEngine> physicsEngine;

	/** Indicates that the camera is in FP mode (true) or game mode (false) */
	bool cameraMode;

	/** Indicates that the physics engine is running */
	bool physicsRunning;

	/** Indicates that the debug rendering should be used */
	bool displayDebugData;

	/** Camera yaw and pitch while in FP camera mode */
	float yaw, pitch;

	bool w, s, a, d, i, j, k, l;

	/**
	Only valid when mapChangeRequested is true.
	Indicates the map to change to
	*/
	FileName nextMap;
	
	/** Indicates that a map change was requested in the previous tick */
	bool mapChangeRequested;

	/** Simple light properties */
	vec4 light_ambient, light_diffuse, light_specular, light_position;
};

#endif
