/*
 * PhysicsBody.cpp
 *
 *  Created on: Jan 21, 2015
 *      Author: bog
 */

#ifdef WITH_BOX2D

#include <boglfw/physics/PhysicsBody.h>
#include <boglfw/World.h>
#include <boglfw/math/box2glm.h>
#include <boglfw/math/aabb.h>

#include <Box2D/Box2D.h>
#include <cmath>

#ifdef DEBUG_DMALLOC
#include <dmalloc.h>
#endif

PhysicsBody::PhysicsBody(int userObjType, void* userPtr, int categFlags, int collisionMask)
	: b2Body_(nullptr)
	, userObjectType_(userObjType)
	, userPointer_(userPtr)
	, categoryFlags_(categFlags)
	, collisionEventMask_(collisionMask)
{
}

PhysicsBody::PhysicsBody(PhysicsBody &&b)
	: onCollision(std::move(b.onCollision))
	, onDestroy(std::move(b.onDestroy))
	, b2Body_(b.b2Body_)
	, userObjectType_(b.userObjectType_)
	, userPointer_(b.userPointer_)
	, getEntityFunc_(std::move(b.getEntityFunc_))
	, categoryFlags_(b.categoryFlags_)
	, collisionEventMask_(b.collisionEventMask_)
{
	b.b2Body_ = nullptr;
	b.onCollision.clear();
	b.onDestroy.clear();
}


void PhysicsBody::create(const PhysicsProperties& props) {
#ifdef DEBUG
	World::assertOnMainThread();
#endif
	assertDbg(b2Body_==nullptr);
	assertDbg(userPointer_ != nullptr);
	assertDbg(!std::isnan(props.angle));
	assertDbg(!std::isnan(props.angularVelocity));
	assertDbg(!std::isnan(props.position.x));
	assertDbg(!std::isnan(props.position.y));
	assertDbg(!std::isnan(props.velocity.x));
	assertDbg(!std::isnan(props.velocity.y));

	b2BodyDef def;
	def.angle = props.angle;
	def.position.Set(props.position.x, props.position.y);
	def.type = props.dynamic ? b2_dynamicBody : b2_staticBody;
	def.userData = (void*)this;
	def.angularDamping = def.linearDamping = 0.3f;
	def.angularVelocity = props.angularVelocity;
	def.linearVelocity = g2b(props.velocity);

	b2Body_ = World::getInstance().getPhysics()->CreateBody(&def);
}

PhysicsBody::~PhysicsBody() {
#ifdef DEBUG
	World::assertOnMainThread();
#endif
	onDestroy.trigger(this);
	if (b2Body_) {
		auto bodyPtr = b2Body_;
		bodyPtr->GetWorld()->DestroyBody(bodyPtr);
	}
}

PhysicsBody* PhysicsBody::getForB2Body(b2Body* body) {
	if (!body->GetUserData())
		return nullptr;
	return (PhysicsBody*)body->GetUserData();
}

AABB PhysicsBody::getAABB() const {
	AABB x;
	if (b2Body_) {
		for (b2Fixture* pFix = b2Body_->GetFixtureList(); pFix; pFix = pFix->GetNext()) {
			x = x.reunion(pFix->GetAABB(0));
		}
	}
	return x;
}

#endif // WITH_BOX2D
