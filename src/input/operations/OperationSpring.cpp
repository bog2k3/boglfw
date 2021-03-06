/*
 * OperationSpring.cpp
 *
 *  Created on: Nov 13, 2014
 *      Author: bogdan
 */

#ifdef WITH_BOX2D

#include <boglfw/input/operations/OperationSpring.h>
#include <boglfw/input/operations/OperationContext.h>
#include <boglfw/renderOpenGL/Viewport.h>
#include <boglfw/input/InputEvent.h>
#include <boglfw/input/operations/IOperationSpatialLocator.h>
#include <boglfw/math/box2glm.h>
#include <boglfw/math/math3D.h>
#include <boglfw/physics/PhysicsBody.h>

#include <Box2D/Box2D.h>

#ifdef DEBUG_DMALLOC
#include <dmalloc.h>
#endif

OperationSpring::OperationSpring(InputEvent::MOUSE_BUTTON boundButton)
	: pContext(nullptr), boundButton(boundButton), isActive(false), mouseJoint(nullptr), mouseBody(nullptr)
	, pressedObj(nullptr)
	, onDestroySubscription(-1)
{
}

OperationSpring::~OperationSpring() {
}

void OperationSpring::enter(const OperationContext* pContext) {
	this->pContext = pContext;
}

void OperationSpring::leave() {
	if (mouseBody) {
		pContext->physics->DestroyBody(mouseBody);
		mouseBody = nullptr;
	}
	pContext = nullptr;
}

void OperationSpring::getFocus() {
}

void OperationSpring::loseFocus() {
}

void OperationSpring::handleInputEvent(InputEvent& ev) {
	switch (ev.type) {
	case InputEvent::EV_MOUSE_DOWN: {
		if (ev.mouseButton != boundButton)
			break;
		glm::vec2 wldClickPos = vec3xy(pContext->pViewport->unproject({ev.x, ev.y, 0.f}));
		pressedObj = pContext->locator->getBodyAtPos(wldClickPos);
		if (pressedObj == nullptr)
			return;
		PhysicsBody* phPtr = (PhysicsBody*)pressedObj->GetUserData();
		onDestroySubscription = phPtr->onDestroy.add(std::bind(&OperationSpring::onOtherBodyDestroyed, this, std::placeholders::_1));
		isActive = true;

		b2BodyDef bdef;
		bdef.type = b2_staticBody;
		bdef.position = g2b(wldClickPos);
		mouseBody = pContext->physics->CreateBody(&bdef);
		b2CircleShape shape;
		shape.m_radius = 0.05f;

		b2FixtureDef fdef;
		fdef.shape = &shape;
		b2Fixture* fix = mouseBody->CreateFixture(&fdef);
		b2Filter filter;
		filter.maskBits = 0;
		fix->SetFilterData(filter);

		b2MouseJointDef def;
		def.target = g2b(wldClickPos);
		def.bodyA = mouseBody;
		def.bodyB = pressedObj;
		def.bodyB->SetAwake(true);
		def.maxForce = 100;
		mouseJoint = (b2MouseJoint*)pContext->physics->CreateJoint(&def);
		break;
	}
	case InputEvent::EV_MOUSE_UP: {
		if (ev.mouseButton != boundButton || !isActive)
			break;
		releaseJoint();
		break;
	}
	case InputEvent::EV_MOUSE_MOVED: {
		if (mouseJoint) {
			mouseJoint->SetTarget(g2b(vec3xy(pContext->pViewport->unproject({ev.x, ev.y, 0}))));
			mouseBody->SetTransform(mouseJoint->GetTarget(), 0);
		}
		break;
	}
	default:
		break;
	}
}

void OperationSpring::onOtherBodyDestroyed(PhysicsBody* body) {
	releaseJoint();
}

void OperationSpring::releaseJoint() {
	if (!isActive)
		return;
	if (onDestroySubscription > 0) {
		PhysicsBody* phPtr = (PhysicsBody*)pressedObj->GetUserData();
		phPtr->onDestroy.remove(onDestroySubscription);
		onDestroySubscription = -1;
	}
	isActive = false;
	pressedObj = nullptr;
	pContext->physics->DestroyJoint(mouseJoint);
	mouseJoint = nullptr;
	pContext->physics->DestroyBody(mouseBody);
	mouseBody = nullptr;
}

void OperationSpring::update(float dt) {
}

#endif // WITH_BOX2D
