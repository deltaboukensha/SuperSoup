#pragma comment(lib, "Box2D.lib" ) //box2d physics engine library
#pragma comment(lib, "opengl32.lib") //opengl library for 32-bit computers
#pragma comment(lib, "glu32.lib") //openglu32 librarary extension to opengl

#include <Windows.h>
#include <gl\GL.h>

#include "Entity.h"

#include "SharedMisc.hpp"

Entity::Entity()
{
	b2Body* body = 0;
	
	entityID = (uint32)this;
	bodyType = dynamic_body;
	shapeWidth = 0.5f;
	shapeHeight = 0.5f;
	aftcX = 0;
	aftcY = 0;

	angle = 0;
	angularDamping = 0;
	angularVelocity = 0;
	gravityScale = 1.0f;
	intertia = 0;
	linearDamping = 0;
	linearVelocityX = 0;
	linearVelocityY = 0;
	mass = 1.0f;
	positionX = 0;
	positionY = 0;

	isActive = true;
	isAwake = true;
	isBullet = false;
	isFixedRotation = false;
	isSleepingAllowed = true;
}

Entity::~Entity()
{
}

void Entity::construct(b2World& world)
{
	//body def
	b2BodyDef bodyDef;

	if(bodyType == static_body)
		bodyDef.type = b2_staticBody;
	else if(bodyType == dynamic_body)
		bodyDef.type = b2_dynamicBody;
	else
		throw "error invalid body def types";

	bodyDef.angle = angle;
	bodyDef.angularDamping = angularDamping;
	bodyDef.angularVelocity = angularVelocity;
	bodyDef.gravityScale = gravityScale;
	
	bodyDef.linearDamping = linearDamping;
	bodyDef.linearVelocity.x = linearVelocityX;
	bodyDef.linearVelocity.y = linearVelocityY;
	bodyDef.position.x = positionX;
	bodyDef.position.y = positionY;
	
	bodyDef.active = isActive;
	bodyDef.awake = isAwake;
	bodyDef.bullet = isBullet;
	bodyDef.fixedRotation = isFixedRotation;
	bodyDef.allowSleep = isSleepingAllowed;

	//body
	// Call the body factory which allocates memory for the ground body
	// from a pool and creates the ground box shape (also from a pool).
	// The body is also added to the world.
	body = world.CreateBody(&bodyDef);

	//fixture def
	b2PolygonShape polygonShape;
	polygonShape.SetAsBox(shapeWidth, shapeHeight);

	b2FixtureDef fixtureDef;
	fixtureDef.shape = &polygonShape;
	fixtureDef.restitution = 0.8f;	// air resistance / fluid resistance
	fixtureDef.density = 1.0f;
	fixtureDef.friction = 1.0f;

	//fixture
	body->CreateFixture(&fixtureDef);
	
	//set mass inertia and center
	b2MassData md;
	md.center = b2Vec2_zero;
	md.I = intertia;
	md.mass = mass;
	body->SetMassData(&md);

	//these cannot be predefined for some reason, seems like a bug in box2d
	body->SetFixedRotation(isFixedRotation);
}

void Entity::destruct()
{
}

void Entity::run()
{
}

void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color)
{
	glColor3f(color.r, color.g, color.b);
	glBegin(GL_LINE_LOOP);
	for (int32 i = 0; i < vertexCount; ++i)
	{
		glVertex2f(vertices[i].x, vertices[i].y);
	}
	glEnd();
}

void Entity::draw()
{
	b2Vec2 position = body->GetPosition();

	glBegin(GL_POINTS);
	glVertex2f(position.x, position.y);
	glEnd();

	b2Color color;
	
	if( body->IsAwake() )
		color = b2Color(1,0,0);
	else
		color = b2Color(0,0,1);

	auto transform = body->GetTransform();
	b2Vec2 vertices[b2_maxPolygonVertices];
	
	for (b2Fixture* f = body->GetFixtureList(); f; f = f->GetNext())
	{
		b2PolygonShape* polygonShape = (b2PolygonShape*)f->GetShape();

		for(int32 i=0; i<polygonShape->m_vertexCount; i++)
			vertices[i] = b2Mul( transform, polygonShape->m_vertices[i] );

		DrawPolygon( vertices, polygonShape->m_vertexCount, color );
	}
}

Message Entity::getSync(bool print)
{
	angle = body->GetAngle();
	angularDamping = body->GetAngularDamping();
	angularVelocity = body->GetAngularVelocity();
	gravityScale = body->GetGravityScale();
	intertia = body->GetInertia();
	linearDamping = body->GetLinearDamping();
	b2Vec2 linearVelocity = body->GetLinearVelocity();
	//body->GetLinearVelocityFromLocalPoint();
	//body->GetLinearVelocityFromWorldPoint();
	//body->GetLocalCenter();
	//body->GetLocalPoint();
	//body->GetLocalVector();
	mass = body->GetMass();
	b2Vec2 position = body->GetPosition();
	//b2Transform transform = body->GetTransform();
	isActive = body->IsActive();
	isAwake = body->IsAwake();
	isBullet = body->IsBullet();
	isFixedRotation = body->IsFixedRotation();
	isSleepingAllowed = body->IsSleepingAllowed();
	
	Message message;
	message.recpientID = entityID;
	message.messageSize = 2 * sizeof(uint32) + 13 * sizeof(float32) + 5 * sizeof(bool);
	message.messageData = new char[message.messageSize];

	unsigned int offset = 0;

	*((uint32*)&message.messageData[offset]) = entityID; offset += sizeof(uint32);
	*((uint32*)&message.messageData[offset]) = bodyType; offset += sizeof(uint32);

	*((float32*)&message.messageData[offset]) = shapeWidth; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = shapeHeight; offset += sizeof(float32);

	*((float32*)&message.messageData[offset]) = angle; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = angularDamping; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = angularVelocity; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = gravityScale; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = intertia; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = linearDamping; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = linearVelocity.x; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = linearVelocity.y; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = mass; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = position.x; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = position.y; offset += sizeof(float32);
	
	*((bool*)&message.messageData[offset]) = isActive; offset += sizeof(bool);
	*((bool*)&message.messageData[offset]) = isAwake; offset += sizeof(bool);
	*((bool*)&message.messageData[offset]) = isBullet; offset += sizeof(bool);
	*((bool*)&message.messageData[offset]) = isFixedRotation; offset += sizeof(bool);
	*((bool*)&message.messageData[offset]) = isSleepingAllowed; offset += sizeof(bool);

	if(print)
	{
		printf("getSync()-------------------------------------------------\n");
		printf("entityID %d\n", entityID);
		printf("bodyType %d\n", bodyType);

		printf("shapeWidth %+e\n", shapeWidth);
		printf("shapeHeight %+e\n", shapeHeight);
		printf("angle %+e\n", angle);
		printf("angularDamping %+e\n", angularDamping);
		printf("angularVelocity %+e\n", angularVelocity);
		printf("gravityScale %+e\n", gravityScale);
		printf("intertia %+e\n", intertia);
		printf("linearDamping %+e\n", linearDamping);
		printf("linearVelocity.x %+e\n", linearVelocity.x);
		printf("linearVelocity.y %+e\n", linearVelocity.y);
		printf("mass %+e\n", mass);
		printf("position.x %+e\n", position.x);
		printf("position.y %+e\n", position.y);

		printf("isActive %d\n", isActive);
		printf("isAwake %d\n", isAwake);
		printf("isBullet %d\n", isBullet);
		printf("isFixedRotation %d\n", isFixedRotation);
		printf("isSleepingAllowed %d\n", isSleepingAllowed);
	}

	return message;
}

void Entity::setSync(const Message& message)
{
	if( message.messageSize != 2 * sizeof(uint32) + 13 * sizeof(float32) + 5 * sizeof(bool) )
		throw "bad sync message";

	unsigned int offset = 0;

	entityID = *((uint32*)&message.messageData[offset]); offset += sizeof(uint32);
	bodyType = (BodyType)*((uint32*)&message.messageData[offset]); offset += sizeof(uint32);

	shapeWidth = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	shapeHeight = *((float32*)&message.messageData[offset]); offset += sizeof(float32);

	angle = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	angularDamping = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	angularVelocity = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	gravityScale = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	intertia = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	linearDamping = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	linearVelocityX = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	linearVelocityY = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	mass = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	positionX = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	positionY = *((float32*)&message.messageData[offset]); offset += sizeof(float32);

	isActive = *((bool*)&message.messageData[offset]); offset += sizeof(bool);
	isAwake = *((bool*)&message.messageData[offset]); offset += sizeof(bool);
	isBullet = *((bool*)&message.messageData[offset]); offset += sizeof(bool);
	isFixedRotation = *((bool*)&message.messageData[offset]); offset += sizeof(bool);
	isSleepingAllowed = *((bool*)&message.messageData[offset]); offset += sizeof(bool);

	/*
	printf("setSync()------------------------------------------------------\n");
	printf("entityID %d\n", entityID);
	printf("bodyType %d\n", bodyType);

	printf("shapeWidth %+e\n", shapeWidth);
	printf("shapeHeight %+e\n", shapeHeight);
	printf("angle %+e\n", angle);
	printf("angularDamping %+e\n", angularDamping);
	printf("angularVelocity %+e\n", angularVelocity);
	printf("gravityScale %+e\n", gravityScale);
	printf("intertia %+e\n", intertia);
	printf("linearDamping %+e\n", linearDamping);
	printf("linearVelocity.x %+e\n", linearVelocityX);
	printf("linearVelocity.y %+e\n", linearVelocityY);
	printf("mass %+e\n", mass);
	printf("position.x %+e\n", positionX);
	printf("position.y %+e\n", positionY);

	printf("isActive %d\n", isActive);
	printf("isAwake %d\n", isAwake);
	printf("isBullet %d\n", isBullet);
	printf("isFixedRotation %d\n", isFixedRotation);
	printf("isSleepingAllowed %d\n", isSleepingAllowed);
	*/
}

Message Entity::getAFTC()
{
	Message message;
	message.recpientID = entityID;
	message.messageSize = 2 * sizeof(float32);
	message.messageData = new char[message.messageSize];

	unsigned int offset = 0;

	*((float32*)&message.messageData[offset]) = aftcX; offset += sizeof(float32);
	*((float32*)&message.messageData[offset]) = aftcY; offset += sizeof(float32);

	return message;
}

void Entity::setAFTC(const Message& message)
{
	if( message.messageSize != 2 * sizeof(float32) )
		throw "bad aftc message";

	unsigned int offset = 0;

	aftcX = *((float32*)&message.messageData[offset]); offset += sizeof(float32);
	aftcY = *((float32*)&message.messageData[offset]); offset += sizeof(float32);

	body->ApplyForceToCenter( b2Vec2(aftcX, aftcY) );
	//printf("aftc x: %+e y: %+e\n", aftcX, aftcY);
	aftcX = 0;
	aftcY = 0;
}

uint32 Entity::getChecksum()
{
	union Union32{
		bool b;
		float32 f;
		uint32 i;
	} u32;

	uint32 checksum = 0;

	u32.f = body->GetAngle(); checksum += u32.i; // printf("angle integer: %d float: %+e\n", u32.i, u32.f);
	u32.f = body->GetAngularDamping(); checksum += u32.i;
	u32.f = body->GetAngularVelocity(); checksum += u32.i;
	u32.f = body->GetGravityScale(); checksum += u32.i;
	u32.f = body->GetInertia(); checksum += u32.i;
	u32.f = body->GetLinearDamping(); checksum += u32.i;
	b2Vec2 linearVelocity = body->GetLinearVelocity();
	u32.f = linearVelocity.x; checksum += u32.i; //translate negative 0 to positive 0
	u32.f = linearVelocity.y; checksum += u32.i;
	//body->GetLinearVelocityFromLocalPoint();
	//body->GetLinearVelocityFromWorldPoint();
	//body->GetLocalCenter();
	//body->GetLocalPoint();
	//body->GetLocalVector();
	u32.f = body->GetMass(); checksum += u32.i;
	b2Vec2 position = body->GetPosition();
	u32.f = position.x; checksum += u32.i;
	u32.f = position.y; checksum += u32.i;
	//b2Transform transform = body->GetTransform();
	
	u32.b = body->IsActive(); checksum += u32.i;
	u32.b = body->IsAwake(); checksum += u32.i;
	u32.b = body->IsBullet(); checksum += u32.i;
	u32.b = body->IsFixedRotation(); checksum += u32.i;
	u32.b = body->IsSleepingAllowed(); checksum += u32.i;

	return checksum;
}