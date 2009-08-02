#include "stdafx.h"
#include "World.h"
#include "MessagePositionHasBeenSet.h"
#include "MessageOrientationHasBeenSet.h"
#include "MessageRequestSetHeight.h"
#include "MessagePhysicsToggle.h"
#include "PhysicsEngine.h"
#include "ComponentRenderAsModel.h"
#include "ComponentPhysicsBody.h"

ComponentPhysicsBody::ComponentPhysicsBody(BlackBoardUID uid, BlackBoard *blackBoard)
: ComponentPhysics(uid, blackBoard)
{
	resetMembers();
	registerMessageHandler("MessageSetWorld",                     bind(&ComponentPhysicsBody::handleMessageSetWorld, this, _1));
	registerMessageHandler("MessageRequestPositionBeSet",         bind(&ComponentPhysicsBody::handleMessageRequestPositionBeSet, this, _1));
	registerMessageHandler("MessageRequestLookAt",                bind(&ComponentPhysicsBody::handleMessageRequestLookAt, this, _1));
	registerMessageHandler("MessageRequestSetOrientation",        bind(&ComponentPhysicsBody::handleMessageRequestSetOrientation, this, _1));
	registerMessageHandler("MessageDeclareInitialPosition",       bind(&ComponentPhysicsBody::handleMessageDeclareInitialPosition, this, _1));
	registerMessageHandler("MessageZombifyActor",                 bind(&ComponentPhysicsBody::handleMessageZombifyActor, this, _1));
	registerMessageHandler("MessagePhysicsDisable",               bind(&ComponentPhysicsBody::handleMessagePhysicsDisable, this, _1));
	registerMessageHandler("MessagePhysicsEnable",                bind(&ComponentPhysicsBody::handleMessagePhysicsEnable, this, _1));	
}

void ComponentPhysicsBody::handleMessageZombifyActor(Message &message)
{
	destroyPhysicsResources();
	resetMembers();
}

void ComponentPhysicsBody::handleMessageSetWorld(Message &message)
{
	World*const world = message.getField<World*const>("world");
	physicsEngine = world->getPhysicsEngine();
}

void ComponentPhysicsBody::handleMessageRequestPositionBeSet(Message &m)
{
	setPosition(m.getField<vec3>("position"));
}

void ComponentPhysicsBody::handleMessageRequestLookAt(Message &m)
{
	if(!body) return;

	const vec3 lookDirection = m.getField<vec3>("lookDirection");

	const vec3 eye = getPosition();
	const vec3 center = eye + lookDirection;
	const vec3 up(0,0,1);

	mat3 orientation;
	orientation.lookAt(eye, center, up);
	setOrientation(orientation);
}

void ComponentPhysicsBody::handleMessageRequestApplyForce(Message &m)
{
	if(!body) return;
	const vec3 force = m.getField<vec3>("force");
	dBodyAddForce(body, force.x, force.y, force.z);
}

void ComponentPhysicsBody::handleMessageDeclareInitialPosition(Message &m)
{
	setPosition(m.getField<vec3>("position"));
	setVelocity(m.getField<vec3>("velocity"));
}

void ComponentPhysicsBody::resetMembers()
{
	modelScale = 1.0f;
	body=0;
	geom=0;
	disableCollisions=false;

	collisionRadius = 1.0f;
	desiredHeight = 1.0f;
	influencedByGravity = true;
	physicsGeometryType = "Box";
	kilograms = 1.0f;
}

void ComponentPhysicsBody::update(float milliseconds)
{
	if(disableCollisions)
	{
		destroyPhysicsResources();
		return;
	}
	
	// record trimesh's previous transform
	recordTriMeshTransform();

	// Poll for last known position and orientation
	pollPosition();
	pollOrientation();

	// Declare the final position and orientation for this frame
	getParentBlackBoard().relayMessage(MessagePositionHasBeenSet(getPosition()));
	getParentBlackBoard().relayMessage(MessageOrientationHasBeenSet(getOrientation()));
}

void ComponentPhysicsBody::draw() const
{
	if(!displayDebugData)
	{
		return;
	}

	CHECK_GL_ERROR();

	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		drawAxes();
		drawPhysicsGeom();
	}
	glPopAttrib();

	CHECK_GL_ERROR();
}

void ComponentPhysicsBody::load(const PropertyBag &data)
{
	ASSERT(physicsEngine, "physics engine is null");

	resetMembers();

	// Destroy any previous rigid body
	destroyAllJoints();
	destroyPhysicsResources();

	// Create the rigid body
	loadRigidBodyData(data);
	createBodyRigidBodyData();

	// Load initial position from data, if available
	loadPosition(data);
	
	// Poll for last known position and orientation
	pollPosition();
	pollOrientation();

	// Declare the initial state
	getParentBlackBoard().relayMessage(MessagePositionHasBeenSet(getPosition()));
	getParentBlackBoard().relayMessage(MessageOrientationHasBeenSet(getOrientation()));
	getParentBlackBoard().relayMessage(MessageRequestSetHeight(desiredHeight));

	// Disable physics initially?
	bool initiallyDisablePhysics = false;
	data.get("initiallyDisablePhysics", initiallyDisablePhysics);
	if(initiallyDisablePhysics)
	{
		MessagePhysicsDisable m;
		getParentBlackBoard().relayMessage(m);
	}
}

void ComponentPhysicsBody::setPosition(const vec3 &position)
{
	lastKnownPosition = position;

	if(!disableCollisions)
	{
		ASSERT(body, "Physics body is nil");
		dBodySetPosition(body, position.x, position.y, position.z);
	}

	getParentBlackBoard().relayMessage(MessagePositionHasBeenSet(getPosition()));
}

void ComponentPhysicsBody::setVelocity(const vec3 &velocity)
{
	if(!disableCollisions)
	{
		ASSERT(body, "Physics body is nil");
		dBodySetLinearVel(body, velocity.x, velocity.y, velocity.z);
	}
}

void ComponentPhysicsBody::setOrientation(const mat3 &m)
{
	lastKnownOrientation = m;

	if(!disableCollisions)
	{
		ASSERT(body, "Physics body is nil");

		dMatrix3 r;

		const vec3 x = m.getAxisX().getNormal();
		const vec3 y = m.getAxisY().getNormal();
		const vec3 z = m.getAxisZ().getNormal();

		r[0] = x.x; r[1] = x.y; r[2] = x.z; r[3] = 0.0f;
		r[4] = y.x; r[5] = y.y; r[6] = y.z; r[7] = 0.0f;
		r[8] = z.x; r[9] = z.y; r[10]= z.z; r[11]= 0.0f;

		dBodySetRotation(body, r);
	}

	getParentBlackBoard().relayMessage(MessageOrientationHasBeenSet(getOrientation()));
}

void ComponentPhysicsBody::createGeom(const string &type, float kilograms)
{
	     if(type == "Box")      createGeomBox(kilograms);
	else if(type == "Cylinder") createGeomCylinder(kilograms);
	else if(type == "Capsule")  createGeomCapsule(kilograms);
	else if(type == "Sphere")   createGeomSphere(kilograms);
	else if(type == "Mesh")     createGeomMesh(kilograms);
	else FAIL("Unknown geom type: " + type);

	dGeomSetData(geom, getActorPtr());
}

void ComponentPhysicsBody::createGeomSphere(float kilograms)
{
	ASSERT(physicsEngine, "\"physicsEngine\" was null");
	dMass mass;
	geom = dCreateSphere(physicsEngine->getSpace(), collisionRadius);
	dGeomSetBody(geom, body);
	dMassSetSphereTotal(&mass, kilograms, collisionRadius);
	dBodySetMass(body, &mass);
}

void ComponentPhysicsBody::createGeomBox(float total_mass)
{
	ASSERT(physicsEngine, "\"physicsEngine\" was null");

	dMass mass;
	dMassSetBoxTotal(&mass,
	                 total_mass,
	                 collisionRadius,
	                 collisionRadius,
	                 desiredHeight);

	geom = dCreateBox(physicsEngine->getSpace(),
	                  collisionRadius,
	                  collisionRadius,
	                  desiredHeight);

	dGeomSetBody(geom, body);

	dBodySetMass(body, &mass);
}

void ComponentPhysicsBody::createGeomCylinder(float kilograms)
{
	ASSERT(physicsEngine, "\"physicsEngine\" was null");

	dMass mass;

	geom = dCreateCylinder(physicsEngine->getSpace(),
	                       collisionRadius,
	                       desiredHeight);

	dGeomSetBody(geom, body);

	dMassSetCylinderTotal(&mass,
	                      kilograms,
	                      3, // aligned with Z-axis
	                      collisionRadius,
	                      desiredHeight);

	dBodySetMass(body, &mass);
}

void ComponentPhysicsBody::createGeomCapsule(float kilograms)
{
	ASSERT(physicsEngine, "\"physicsEngine\" was null");

	dMass mass;

	const float capsuleHeight = desiredHeight - collisionRadius*2.0f;

	geom = dCreateCCylinder(physicsEngine->getSpace(),
	                        collisionRadius,
	                        capsuleHeight);

	dGeomSetBody(geom, body);

	dMassSetCappedCylinderTotal(&mass,
	                            kilograms,
	                            3, // aligned with Z-axis
	                            collisionRadius,
	                            capsuleHeight);

	dBodySetMass(body, &mass);
}

void ComponentPhysicsBody::drawPhysicsGeom() const
{
	if(!geom) return;

	CHECK_GL_ERROR();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	//PhysicsEngine::drawGeomAABB(geom);
	PhysicsEngine::drawGeom(geom);
	glPopAttrib();
	CHECK_GL_ERROR();
}

void ComponentPhysicsBody::drawAxes() const
{
	if(!body) return;

	CHECK_GL_ERROR();
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	{
		glLineWidth(2.0f);

		const mat3 orientation = getOrientation();
		mat4 transformation(getPosition(),
		                    orientation.getAxisX(),
		                    orientation.getAxisY(),
		                    orientation.getAxisZ());

		glPushMatrix();
		glMultMatrixf(transformation);

		glBegin(GL_LINES);
		glColor4fv(red);
		glVertex3fv(vec3(0,0,0));
		glVertex3fv(orientation.getAxisX());

		glColor4fv(green);
		glVertex3fv(vec3(0,0,0));
		glVertex3fv(orientation.getAxisY());

		glColor4fv(blue);
		glVertex3fv(vec3(0,0,0));
		glVertex3fv(orientation.getAxisZ());
		glEnd();

		glPopMatrix();
	}
	glPopAttrib();
	CHECK_GL_ERROR();
}

void ComponentPhysicsBody::handleMessagePhysicsDisable(Message &)
{
	disableCollisions = true;
}

void ComponentPhysicsBody::handleMessagePhysicsEnable(Message &)
{
	if(disableCollisions)
	{
		disableCollisions = false;

		// Recreate rigid body data
		createBodyRigidBodyData();

		/*
		Reaffirm last known transformation.
		NOTE: Raise just anove the ground to allow the object to
		make good contact with the floor beneath it.
		*/
		setPosition(lastKnownPosition + vec3(0.0f, 0.0f, 0.2f));
		setOrientation(lastKnownOrientation);

		// Declare the initial state
		getParentBlackBoard().relayMessage(MessagePositionHasBeenSet(getPosition()));
		getParentBlackBoard().relayMessage(MessageOrientationHasBeenSet(getOrientation()));
		getParentBlackBoard().relayMessage(MessageRequestSetHeight(desiredHeight));
	}
}

void ComponentPhysicsBody::destroyAllJoints()
{
	if(!body)
	{
		return;
	}

	int numOfJoints = dBodyGetNumJoints(body);
	vector<dJointID> joints;
	
	for(int i=0; i<numOfJoints; ++i)
	{
		dJointID joint = dBodyGetJoint(body, i);
		joints.push_back(joint);
	}

	for(size_t i=0; i<joints.size(); ++i)
	{
		dJointDestroy(joints[i]);
	}
}

void ComponentPhysicsBody::resetBodyParameters( dBodyID body )
{
	dBodySetForce(body, 0, 0, 0);
	dBodySetTorque(body, 0, 0, 0);
	dBodySetLinearVel(body, 0, 0, 0);
	dBodySetAngularVel(body, 0, 0, 0);
}

void ComponentPhysicsBody::destroyPhysicsResources()
{
	if(body){dBodyDestroy(body);} body=0;
	if(geom){dGeomDestroy(geom);} geom=0;
	mesh=0;
}

void ComponentPhysicsBody::handleMessageRequestSetOrientation( Message &m )
{
	setOrientation(m.getField<mat3>("orientation"));
}

void ComponentPhysicsBody::createGeomMesh(float kilograms)
{
	ASSERT(physicsEngine, "\"physicsEngine\" was null");
	dSpaceID space = physicsEngine->getSpace();
	ComponentRenderAsModel &c = getPrerequisiteComponent<ComponentRenderAsModel>("RenderAsModel");
	const AnimationSequence &sequence = c.getModel().getAnimation();
	tuple<dGeomID,dTriMeshDataID> n = sequence.createGeom(space);
	geom = n.get<0>();
	mesh = n.get<1>();

	dMass mass;
	dGeomSetBody(geom, body);
	dMassSetTrimeshTotal(&mass, 100.0f, geom);
	dMassTranslate(&mass, -mass.c[0], -mass.c[1], -mass.c[2]);
	dBodySetMass(body, &mass);
}

vec3 ComponentPhysicsBody::pollPosition()
{
	ASSERT(body, "Physics body is nil");
	const dReal *p = dBodyGetPosition(body);
	lastKnownPosition = vec3((float)p[0], (float)p[1], (float)p[2]);
	return lastKnownPosition;
}

mat3 ComponentPhysicsBody::pollOrientation()
{
	ASSERT(body, "Physics body is not nil");
	const dReal *r = dBodyGetRotation(body);
	lastKnownOrientation = mat3(vec3((float)r[0], (float)r[1], (float)r[2]),
	                            vec3((float)r[4], (float)r[5], (float)r[6]),
	                            vec3((float)r[8], (float)r[9], (float)r[10]));
	return lastKnownOrientation;
}

void ComponentPhysicsBody::recordTriMeshTransform()
{
	if(dGeomGetClass(geom) == dTriMeshClass)
	{
		const dReal *rot = dBodyGetRotation(body);
		const dReal *pos = dBodyGetPosition(body);
		dMatrix4 trans = {rot[0], rot[1], rot[2],  pos[0],
		                  rot[4], rot[5], rot[6],  pos[1],
		                  rot[8], rot[9], rot[10], pos[2],
		                  0,      0,       0,      1};
		dGeomTriMeshSetLastTransform(geom, trans);
	}
}

void ComponentPhysicsBody::createBodyRigidBodyData()
{
	body = dBodyCreate(physicsEngine->getWorld());
	createGeom(physicsGeometryType, kilograms);
	dBodySetGravityMode(body, influencedByGravity ? 1 : 0);
	resetBodyParameters(body);
}

void ComponentPhysicsBody::loadPosition( const PropertyBag &data )
{
	vec3 position;
	if(data.get("position", position)) // optional tag
	{
		setPosition(position);
	}
}