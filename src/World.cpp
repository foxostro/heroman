#include "stdafx.h"
#include "searchfile.h"
#include "Application.h"
#include "Frustum.h"
#include "Dimmer.h"
#include "Effect.h"
#include "ComponentPhysics.h"
#include "ComponentMovement.h"
#include "ComponentHealth.h"
#include "MessagePassInputSubsystem.h"
#include "MessageExplosionOccurs.h"
#include "MessageSetPlayerNumber.h"
#include "MessageDeclareGameOver.h"
#include "MessageChangeScore.h"
#include "MessageZombifyActor.h"
#include "World.h"

World::World(BlackBoardUID uid,
			 BlackBoard *parentBlackBoard,
			 GraphicsDevice &_graphicsDevice,
			 TextureFactory &_textureFactory,
			 SDLinput &_input,
			 shared_ptr<Font> _font,
			 GUI &gui)
: name("(nil)"),
  skyBox(_textureFactory,
         FileName("data/skybox/top.jpg"),
         FileName("data/skybox/bottom.jpg"),
		 FileName("data/skybox/east.jpg"),
		 FileName("data/skybox/west.jpg"),
		 FileName("data/skybox/north.jpg"),
		 FileName("data/skybox/south.jpg")),
  fog(bind(&GraphicsDevice::setClippingPlanes, &_graphicsDevice, _1, _2)),
  textureFactory(_textureFactory),
  input(_input),
  font(_font),
  cameraMode(false),
  physicsRunning(true),
  displayDebugData(true),
  yaw(0.0f),
  pitch(0.0f),
  w(false),
  s(false),
  a(false),
  d(false),
  i(false),
  j(false),
  k(false),
  l(false),
  mapChangeRequested(false),
  BlackBoardSubscriber(uid, parentBlackBoard)
{
	input.registerKeyPressHandler(SDLK_F1,    bind(&World::onKeyToggleCamera, this));
	input.registerKeyPressHandler(SDLK_F2,    bind(&World::onKeyToggleDebugRendering, this));
	input.registerKeyPressHandler(SDLK_F3,    bind(&World::onKeyTogglePhysics, this));

	input.registerKeyDownHandler (SDLK_UP,    bind(&World::onKeyDownUp, this));
	input.registerKeyDownHandler (SDLK_DOWN,  bind(&World::onKeyDownDown, this));
	input.registerKeyDownHandler (SDLK_LEFT,  bind(&World::onKeyDownLeft, this));
	input.registerKeyDownHandler (SDLK_RIGHT, bind(&World::onKeyDownRight, this));

	input.registerKeyDownHandler (SDLK_i, bind(&World::onKeyDownFwd, this));
	input.registerKeyDownHandler (SDLK_k, bind(&World::onKeyDownRev, this));
	input.registerKeyDownHandler (SDLK_j, bind(&World::onKeyDownStrafeLeft, this));
	input.registerKeyDownHandler (SDLK_l, bind(&World::onKeyDownStrafeRight, this));

	registerMessageHandler("MessageExitMap", bind(&World::handleMessageExitMap, this, _1));
	registerMessageHandler("MessageChangeScore", bind(&World::handleMessageChangeScore, this, _1));

	coinsLabel = new LabelWidget("Coins Remaining: (nil)",
	                             vec2(60,700),
								 white,
								 Font::SizeHuge,
								 font);
	coinsLabel->visible = true;
	scoreLabel = new LabelWidget("Score: ",
								 vec2(60, 650),
								 color(0.84765f, 0.359375f, 0.0f, 1.0f),
								 Font::SizeHuge,
								 font);
	scoreLabel->visible = true;
	gui.addWidget(coinsLabel);
	gui.addWidget(scoreLabel);

	// initialize score
	int initialScore = 0;
	levelScore = shared_ptr<Score>(new Score(initialScore,
	                       	                 BlackBoard::genName(),
	                       	                 getParentBlackBoardPtr()));
}

void World::handleMessageExitMap(Message &message)
{
	mapChangeRequested = true;
	nextMap = message.getField<FileName>("nextMap");
}

void World::handleMessageChangeScore(Message &message)
{
	levelScore->recvMessage(message);
}

void World::loadFromFile(const FileName &_fileName)
{
	fileName = _fileName;
	load(PropertyBag::fromFile(fileName));
}

void World::load(const PropertyBag &bag)
{
	PropertyBag mapBag, terrainBag;

	// Destroy any old instance of this world
	destroy();

	// New physics engine
	physicsEngine = shared_ptr<PhysicsEngine>(new PhysicsEngine());

	name = bag.getString("name");
	fog.load(bag.getBag("fog"));
	objects.setParentBlackBoard(getParentBlackBoardPtr());
	objects.load(bag.getBag("objects"), this);
	coinsRemaining = bag.getInt("numberCoinsGoal");

	if(bag.get("map", mapBag))
	{
		map.create(mapBag, textureFactory, physicsEngine);
	}

	if(bag.get("terrain", terrainBag))
	{
		terrain.create(terrainBag, textureFactory, physicsEngine);
	}

	// start with no debug information being displayed
	enableDebugRendering(false);

	// Set up lighting
	const PropertyBag light = bag.getBag("light");
	light_ambient = light.getVec4("ambient");
	light_diffuse = light.getVec4("diffuse");
	light_specular = light.getVec4("specular");
	light_position = light.getVec4("position");
	setupSimpleLight();

	// Initialize the camera
	cameraLooksAtPlayerStartPoint();
}

void World::draw() const
{
	CHECK_GL_ERROR();
	camera.setCamera();
	skyBox.draw(camera.eye);
	drawObjectsAndMap();
	drawTransparentObjects();
	drawParticles();
	CHECK_GL_ERROR();
}

void World::update(float deltaTime)
{
	if(isGameOver())
	{
		getParentBlackBoard().relayMessage(MessageDeclareGameOver());
	}
	else
	{
		handleMapChangeRequest();
		objects.update(deltaTime);
		recalculateAveragePlayerPosition();
		updateCamera(deltaTime);
		particleEngine.update(deltaTime, camera);
		updatePhysics(deltaTime);
		updateLevelScore(deltaTime);
		updateScoreLabel();
		updateCoinsLabel();
		resetKeyFlags();
	}
}

ActorPtr World::createPlayer(const vec3 &initialPosition,
							 const PropertyBag &playerData,
                             int playerNumber,
                             int numOfPlayers)
{
	const ActorPtr player = objects.create().get<1>();

	const PropertyBag base = playerData.getBag("components");
	const PropertyBag spec = PropertyBag(); // empty

	const ComponentDataSet components = ComponentDataSet::load(base, spec);

	player->load(components, initialPosition, vec3(0,0,0), this);
	player->relayMessage(MessageSetPlayerNumber(playerNumber));
	player->relayMessage(MessagePassInputSubsystem(&input));

	if(numOfPlayers > 1)
	{
		ComponentPhysics* component =
			dynamic_cast<ComponentPhysics*>(player->getComponent("Physics"));

		if(component)
		{
			const float ratio = (float)playerNumber / numOfPlayers;
			const float angle = 2.0f * (float)M_PI * ratio;
			const vec3 offset = vec3(cosf(angle), sinf(angle), 0.0f) * 1.2f;
			const vec3 &position = component->getPosition();

			component->setPosition(position + offset);
		}
	}

	return player;
}

void World::playersEnter(int numOfPlayers)
{
	detroyAllPlayers();

	const vec3 playerPosition = getPlayerStartPoint();

	const PropertyBag playerData =
		PropertyBag::fromFile(FileName("data/actorDefs/startingPlayer.xml"));

	int numOfJoy = SDL_NumJoysticks();
	numOfPlayers = (numOfPlayers>numOfJoy) ? numOfJoy : numOfPlayers;
	numOfPlayers = (numOfPlayers<1) ? 1 : numOfPlayers;

	for(int i = 0; i < numOfPlayers; ++i)
	{
		ActorPtr player = createPlayer(playerPosition,
		                               playerData,
									   i,
									   numOfPlayers);
		players.push_back(player);
	}

	recalculateAveragePlayerPosition(); // Set initially...
	updateCamera(0); // Set initially...
}

void World::updateCamera_Overhead()
{
	const size_t numOfPlayers = players.size();
	static const float minCameraDistance = 10.0f;

	if(numOfPlayers==0)
	{
		setCameraLook((float)(M_PI / 2.0), 0.0f, minCameraDistance, averagePlayerPosition);
		return;
	}

	const vec3 averagePlayerPosition = getAveragePlayerPosition();
	float distance = minCameraDistance;
	float cameraAngleZ = 0.0f;

	if(numOfPlayers>1)
	{
		float maxPlayerDistance=0.0f;
		for(vector<ActorPtr>::iterator i=players.begin(); i!=players.end(); ++i)
		{
			ActorPtr pl = *i;
			ASSERT(pl, "Null parameter: pl");

			ComponentPhysics* component =
				dynamic_cast<ComponentPhysics*>(pl->getComponent("Physics"));

			if(component)
			{
				const vec3 position = component->getPosition();
				vec3 delta = position - averagePlayerPosition;
				const float distanceToLocus = delta.getMagnitude();
				maxPlayerDistance = max(maxPlayerDistance, distanceToLocus);
			}
		}

		distance = max(minCameraDistance,maxPlayerDistance+minCameraDistance);
	}
	else
	{
		ActorPtr player = players[i];
		ASSERT(player, "Null parameter: player");

		ComponentMovement* component =
		dynamic_cast<ComponentMovement*>(player->getComponent("Movement"));

		if(component)
		{
			cameraAngleZ = component->getFacingAngle();
		}
	}

	setCameraLook((float)(M_PI / 2.0),
	              cameraAngleZ,
				  distance,
				  averagePlayerPosition);
}

void World::updateCamera_FirstPerson(float deltaTime)
{
	float dTime = deltaTime / 1000.0f;
	float yawSpeed = ((float)M_PI * 4.0f) * dTime;
	float pitchSpeed = ((float)M_PI * 2.0f) * dTime;
	float moveSpeed = 0.0f;
	float strafeSpeed = 0.0f;

	     if(a) yaw += yawSpeed;
	else if(d) yaw -= yawSpeed;

	     if(w) pitch -= pitchSpeed;
	else if(s) pitch += pitchSpeed;

	     if(i) moveSpeed = 5.0f * dTime;
	else if(k) moveSpeed = -5.0f * dTime;

	     if(j) strafeSpeed = 5.0f * dTime;
	else if(l) strafeSpeed = -5.0f * dTime;

	pitch = max(-1.570796f, min(1.570796f, pitch));
	yaw = fmod(yaw, (float)M_PI * 2.0f);

	const vec3 look = vec3(cosf(yaw), sinf(yaw), sinf(pitch)).getNormal();

	vec3 up = vec3(0,0,1);
	vec3 eye = camera.eye + (look*moveSpeed) + (up.cross(look)*strafeSpeed);
	vec3 center = camera.eye + look;

	camera.lookAt(eye, center, up);
}

void World::updateCamera(float deltaTime)
{
	if(cameraMode)
	{
		updateCamera_FirstPerson(deltaTime);
	}
	else
	{
		updateCamera_Overhead();
	}
}

vec3 World::findAveragePlayerPosition() const
{
	ASSERT(players.size()>=1, "There are no players in this world!");

	int count=0;
	vec3 averagePlayerPosition = vec3(0,0,0);

    for(vector<ActorPtr>::const_iterator i=players.begin();
		i!=players.end(); ++i)
    {
        const ActorPtr player = *i;
        ASSERT(player, "Null parameter: player");

		const ComponentPhysics* physics =
			dynamic_cast<ComponentPhysics*>(player->getComponent("Physics"));

		if(physics)
		{
			averagePlayerPosition=averagePlayerPosition+physics->getPosition();
			count++;
		}
    }

	averagePlayerPosition = averagePlayerPosition * (1.0f/count);

	return averagePlayerPosition;
}

void World::onKeyDownFwd()
{
	i = cameraMode;
}

void World::onKeyDownRev()
{
	k = cameraMode;
}

void World::onKeyDownStrafeLeft()
{
	j = cameraMode;
}

void World::onKeyDownStrafeRight()
{
	l = cameraMode;
}

void World::onKeyDownUp()
{
	w = cameraMode;
}

void World::onKeyDownDown()
{
	s = cameraMode;
}

void World::onKeyDownLeft()
{
	a = cameraMode;
}

void World::onKeyDownRight()
{
	d = cameraMode;
}

void World::onKeyToggleCamera()
{
	cameraMode = !cameraMode;
	playSound(FileName("data/sound/flashlight-toggle.wav"));
}

void World::onKeyTogglePhysics()
{
	physicsRunning = !physicsRunning;
	playSound(FileName("data/sound/flashlight-toggle.wav"));
}

void World::onKeyToggleDebugRendering()
{
	displayDebugData = !displayDebugData;
	enableDebugRendering(displayDebugData);
	playSound(FileName("data/sound/activate.wav"));
}

void World::enableDebugRendering(bool _displayDebugData)
{
	displayDebugData = _displayDebugData;
	objects.enableDebugRendering(_displayDebugData);
	terrain.displayDebugData = _displayDebugData;
	map.displayDebugData = _displayDebugData;
}

void World::playSound(const FileName &sound)
{
	if(sound != FileName(""))
	{
		getParentBlackBoard().relayMessage(MessagePlaySound(sound));
	}
}

void World::setupSimpleLight()
{
	/*float ambient[]  = { 0.4f, 0.4f, 0.4f, 1.0f };
	float diffuse[]  = { 0.7f, 0.7f, 0.7f, 1.0f };
	float specular[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	float position[] = { 0.577f, 0.577f, -0.577f, 0.0f };*/

	float *ambient = light_ambient;
	float *diffuse = light_diffuse;
	float *specular = light_specular;
	float *position = light_position;

	glLightfv(GL_LIGHT1, GL_AMBIENT,  ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE,  diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);
	glLightfv(GL_LIGHT1, GL_POSITION, position);
	glEnable(GL_LIGHT1);
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient); // scene ambient light
}

void World::destroy()
{
	mapChangeRequested = false;
	terrain.clear();
	map.clear();
	objects.destroy();
	players.clear();
	physicsEngine.reset();
}

void World::setCameraLook(float theta,
						  float angleZ,
						  float distance,
						  const vec3 &averagePlayerPosition)
{
	const float dx = distance*cosf(theta);
	const float dy = distance*cosf(theta);
	const float dz = distance*sinf(theta);

	camera.lookAt(averagePlayerPosition + vec3(dx,dy,dz),
	              averagePlayerPosition,
				  mat3::fromRotateZ(angleZ).transformVector(vec3(1,0,0)));
}

void World::generateExplosion(OBJECT_ID originator,
							  const vec3 &position,
							  float rotation,
							  int baseDamage,
							  const FileName &soundFileName,
							  const FileName &particlesFileName)
{
	playSound(soundFileName);

	particleEngine.add(particlesFileName,
	                   position,
	                   rotation,
	                   getTextureFactory());

	// Allow actors to apply splash damage and do ray checking
	getObjects().relayMessage(MessageExplosionOccurs(position,
	                                                 baseDamage,
													 originator));
}

void World::drawParticles() const
{
	effect_Begin(effect_PARTICLE);
	particleEngine.draw();
	effect_End();
}

void World::drawObjectsAndMap() const
{
	effect_Begin(effect_LIT_AND_TEXTURED);

	fog.activate();
	terrain.draw();
	map.draw();
	objects.drawForGame(GraphicsDevice::getCameraFrustum());
	fog.deactivate();

	effect_End();
}

void World::updateCoinsLabel()
{
	ASSERT(coinsLabel, "Null Pointer: coinsLabel");

	if(coinsRemaining>0)
	{
		coinsLabel->setLabel("Coins Remaining: " + itos(coinsRemaining));
	}
	else
	{
		coinsLabel->setLabel("No Coins Remaining! Exit is OPEN.");
	}
}

/** Updates the level's score on the main interface */
void World::updateScoreLabel()
{
	ASSERT(scoreLabel, "Null Pointer: scoreLabel");
	scoreLabel->setLabel(itos(levelScore->getScore()));
}

bool World::isGameOver()
{
	for(vector<ActorPtr>::iterator i=players.begin(); i!=players.end(); ++i)
	{
		ActorPtr pl = *i;
		ASSERT(pl, "Null player: pl");

		ComponentHealth *component =
			dynamic_cast<ComponentHealth*>(pl->getComponent("Health"));

		if(component && component->isDead())
		{
			return true;
		}
	}

	return false;
}

void World::drawTransparentObjects() const
{
	effect_Begin(effect_TRANSPARENT);
	objects.drawTransparentBits(GraphicsDevice::getCameraFrustum());
	effect_End();
}

void World::updateLevelScore( float deltaTime )
{
	ASSERT(levelScore, "Null Pointer: levelScore");
	levelScore->update(deltaTime);
}

void World::handleMapChangeRequest()
{
	if(mapChangeRequested)
	{
		int numOfPlayers = (int)players.size();
		mapChangeRequested = false;
		loadFromFile(nextMap);
		playersEnter(numOfPlayers);
	}
}

void World::updatePhysics( float deltaTime )
{
	if(physicsRunning)
	{
		physicsEngine->update(deltaTime);
	}
}

void World::detroyAllPlayers()
{
	for(size_t i = 0; i < players.size(); ++i)
	{
		ActorPtr player = players[i];
		player->relayMessage(MessageZombifyActor());
	}
	objects.reapZombieActors();
	players.clear();
}

void World::cameraLooksAtPlayerStartPoint()
{
	averagePlayerPosition = getPlayerStartPoint();
	updateCamera(0);
}

vec3 World::getPlayerStartPoint() const
{
	for(ActorSet::const_iterator i=objects.begin(); i!=objects.end(); ++i)
	{
		ActorPtr a = i->second;

		if(a->actorName == "PlayerStart")
		{
			const ComponentPhysics* physics =
				dynamic_cast<ComponentPhysics*>(a->getComponent("Physics"));

			if(physics)
			{
				vec3 p = vec3(physics->getPosition().xy(), 1.5f);
				TRACE("player start = " + vec3::toString(p));
				return p;
			}
		}
	}

	FAIL("No player start point found!");
	return vec3(0,0,0);
}
