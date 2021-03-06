/*
 * PhysContactListener.cpp
 *
 *  Created on: Jan 22, 2015
 *      Author: bogdan
 */
 
#ifdef WITH_BOX2D

#include <boglfw/physics/PhysContactListener.h>
#include <boglfw/physics/PhysicsBody.h>
#include <boglfw/perf/marker.h>
#include <boglfw/utils/log.h>

#include <Box2D/Box2D.h>

#ifdef DEBUG_DMALLOC
#include <dmalloc.h>
#endif

PhysContactListener::PhysContactListener()
	: eventBuffer(1024) {
}

PhysContactListener::~PhysContactListener() {
}

/*void PhysContactListener::BeginContact(b2Contact* contact) {

}

void PhysContactListener::EndContact(b2Contact* contact) {

}

void PhysContactListener::PreSolve(b2Contact* contact, const b2Manifold* oldManifold) {

}*/

void PhysContactListener::PostSolve(b2Contact* contact, const b2ContactImpulse* impulse) {
	PhysicsBody *body1 = (PhysicsBody*)contact->GetFixtureA()->GetBody()->GetUserData();
	PhysicsBody *body2 = (PhysicsBody*)contact->GetFixtureB()->GetBody()->GetUserData();
	if (!body1 || !body2)
		return;

	if (body1->categoryFlags_ & body2->collisionEventMask_)
		eventBuffer.push_back(eventData(body2, body1, impulse->normalImpulses[0]));
	if (body2->categoryFlags_ & body1->collisionEventMask_)
		eventBuffer.push_back(eventData(body1, body2, impulse->normalImpulses[0]));
}

void PhysContactListener::update(float dt) {
	PERF_MARKER_FUNC;
	for (auto e : eventBuffer) {
		e.target->onCollision.trigger(e.argument, e.impulseMagnitude);
	}
	eventBuffer.clear();
}

#endif // WITH_BOX2D
