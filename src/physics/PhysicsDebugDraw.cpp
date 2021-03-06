/*
 * PhysicsDebugDraw.cpp
 *
 *  Created on: Nov 27, 2014
 *      Author: bog
 */
 
#ifdef WITH_BOX2D

#include <boglfw/physics/PhysicsDebugDraw.h>
#include <boglfw/renderOpenGL/Shape3D.h>
#include <boglfw/math/box2glm.h>
#include <boglfw/math/math3D.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#ifdef DEBUG_DMALLOC
#include <dmalloc.h>
#endif

PhysicsDebugDraw::PhysicsDebugDraw() {
}

PhysicsDebugDraw::~PhysicsDebugDraw() {
}

/// Draw a closed polygon provided in CCW order.
void PhysicsDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	glm::vec3 verts[vertexCount];
	for (int i=0; i<vertexCount; i++)
		verts[i] = {vertices[i].x, vertices[i].y, 0};
	Shape3D::get()->drawPolygon((glm::vec3*)verts, (int)vertexCount, b2g(color));
}

/// Draw a solid closed polygon provided in CCW order.
void PhysicsDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {
	DrawPolygon(vertices, vertexCount, color);
}

/// Draw a circle.
void PhysicsDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
	Shape3D::get()->drawCircleXOY(b2g(center), radius, 12, b2g(color));
}

/// Draw a solid circle.
void PhysicsDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
	DrawCircle(center, radius, color);
}

/// Draw a line segment.
void PhysicsDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
	Shape3D::get()->drawLine(glm::vec3{b2g(p1), 0}, glm::vec3{b2g(p2), 0}, b2g(color));
}

/// Draw a transform. Choose your own length scale.
/// @param xf a transform.
void PhysicsDebugDraw::DrawTransform(const b2Transform& xf) {
	b2Vec2 x = xf.p + xf.q.GetXAxis();
	b2Vec2 y = xf.p + xf.q.GetYAxis();
	b2Color red(1, 0, 0), green(0, 1, 0);
	DrawSegment(xf.p, x, red);
	DrawSegment(xf.p, y, green);
}

/// Draw a point.
void PhysicsDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
	DrawCircle(p, size, color);
	DrawSegment(p - b2Vec2(size/2, 0), p + b2Vec2(size/2, 0), color);
	DrawSegment(p - b2Vec2(0, size/2), p + b2Vec2(0, size/2), color);
}

#endif // WITH_BOX2D
