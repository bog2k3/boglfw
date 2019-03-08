#pragma once

/*
 * GLM 4x4 Matrices are column-major and column-axis-aligned
 * (each column of the matrix represents the local space's X/Y/Z/W axis)
 * Translation is encoded as the last element of each of the first 3 columns
 *
 * When passing the matrix to OpenGL it will interpret it as colum major by default (when transpose=GL_FALSE)
 * For this reason, both in C++ and GLSL the correct multiplication order is:
 *
 * LAST_MATRIX * ... * FIRST_MATRIX * VEC4
 *
 */

#include "constants.h"
#include "../utils/assert.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <glm/geometric.hpp>

#include <utility>

template<typename T> inline T sqr(T const &x) { return x*x; }
template<typename T> inline void xchg(T &x1, T &x2) { T aux(x1); x1 = x2; x2 = aux; }
template<typename T> inline void xchg(T &&x1, T &&x2) { T aux(std::move(x1)); x1 = std::move(x2); x2 = std::move(aux); }
template<typename T> inline T min(T const &x, T const &y) { return x < y ? x : y; }
template<typename T> inline T max(T const &x, T const &y) { return x > y ? x : y; }
template<typename T> inline T sign(T const& x) { return x > 0 ? T(+1) : (x < 0 ? T(-1) : T(0)); }
template<typename T> inline T abs(T const& x) { return x < 0 ? -x : x; }

// returns the normal vector of a 2D vector in the same plane, pointing towards the positive subspace of the initial vector (left)
// as if crossing the original vector with a 3D upwards vector
inline glm::vec2 getNormalVector(glm::vec2 v) { return glm::vec2(-v.y, v.x); }

inline float eqEps(float f1, float f2) { return abs(f1 - f2) < EPS; }
inline float eqEps(float f1, float f2, float eps) { return abs(f1 - f2) < eps; }

// returns the cross product of two 2D vectors, as a scalar (positive if v2 is clockwise from v1 and negative otherwise)
inline float cross2D(const glm::vec2 &v1, const glm::vec2 &v2) {
	return (v1.x*v2.y) - (v1.y*v2.x);
}


inline int circularPrev(int index, int n) {
	if (n == 0)
		return 0;
	return (index+n-1) % n;
}

inline int circularNext(int index, int n) {
	if (n == 0)
		return 0;
	return (index+1) % n;
}

/*
 * computes the angle from (0,0) in direction p. p is assumed to be normalized
 */
inline float pointDirectionNormalized(glm::vec2 const &p) {
	if (p.x >= 0)
		return asinf(p.y);
	else {
		auto s = sign(p.y);
		if (s == 0)
			s = 1;
		return -asinf(p.y) + PI*s;
	}
}

/*
 * computes the angle from (0,0) in direction p. p can have any arbitrary length
 */
inline float pointDirection(glm::vec2 const &p) {
	assertDbg(glm::length(p) != 0);
	return pointDirectionNormalized(glm::normalize(p));
}

/**
 * brings an angle into a user defined range (bisector being the max angle where the circle is cut):
 * 	[bisector-2*PI, bisector]
 * for example, providing PI/2 as bisector, the angle will be brought into this interval:
 * 	[-3*PI/2, PI/2]
 */
inline float limitAngle(float a, float bisector) {
	assert(bisector >= 0 && bisector <= 2*PI);
	while (a > bisector)
		a -= 2*PI;
	while (a < bisector - 2*PI)
		a += 2*PI;
	return a;
}

// computes the angular difference between two angles a and b.
// the difference is expressed in terms of where is b relative to a, in the interval (-PI, +PI]
inline float angleDiff(float a, float b) {
	float d = b-a;
	while (d > PI)
		d -= 2*PI;
	while (d <= -PI)
		d += 2*PI;
	return d;
}

// computes the smaller angular difference (that is the smallest value to increment one of them such that the two become equivalent)
// between the two angles in absolute terms.
// The returned value lies in the interval [0, PI)
inline float absAngleDiff(float a, float b) {
	float d = a - b;
	while (d >= 2*PI)
		d -= 2*PI;
	while (d < 0)
		d += 2*PI;
	if (d > PI)
		d = 2*PI - d;
	return d;
}

// tests if two angle spans overlap and tells the amount of overlap (if true) or the shortest distance between them (if false)
// angle1 - center of span1
// span1 - the size of the "cone"
// angle2, span2 - same
// sweepPositive - return the gap from the "positive" side of span1 or "negative" side
// will set outMargin to negative if overlap, or positive shortest gap around element if no overlap
// returns true if overlap
bool angleSpanOverlap(float angle1, float span1, float angle2, float span2, bool sweepPositive, float &outMargin);

inline glm::vec2 operator * (glm::vec2 const& x, float f) {
	return glm::vec2(x.x*f, x.y*f);
}

template <class V3>
inline glm::vec2 vec3xy(V3 const &in) {
	return glm::vec2(in.x, in.y);
}

template <class V3>
inline glm::vec2& vec3xy(V3 &in) {
	return (glm::vec2&)in;
}

template <class V3>
inline glm::vec2 vec3xz(V3 const &in) {
	return glm::vec2(in.x, in.z);
}

inline glm::vec3 vec4xyz(glm::vec4 const& v) {
	return {v.x, v.y, v.z};
}

inline float vec3lenSq(glm::vec3 const&v) {
	return sqr(v.x) + sqr(v.y) + sqr(v.z);
}

inline float vec3len(glm::vec3 const&v) {
	return sqrtf(vec3lenSq(v));
}

inline float vec2lenSq(glm::vec2 const& v) {
	return vec3lenSq(glm::vec3(v, 0));
}

inline float vec2len(glm::vec2 const& v) {
	return vec3len(glm::vec3(v, 0));
}

// extract translation from a transformation matrix
inline glm::vec3 m4Translation(glm::mat4 const& m4) {
	return {m4[3][0], m4[3][1], m4[3][2]};
}

// extract a column from a matrix
inline glm::vec4 m4col(glm::mat4 const& m, int c) {
	return {m[c][0], m[c][1], m[c][2], m[c][3]};
}

// extract a row from a matrix
inline glm::vec4 m4row(glm::mat4 const& m, int r) {
	return {m[0][r], m[1][r], m[2][r], m[3][r]};
}

// builds an object transformation matrix from 3 perpendicular axes and a translation value.
// the axes are assumed to be normalized and orthogonal.
glm::mat4 buildMatrix(glm::vec3 right, glm::vec3 up, glm::vec3 front, glm::vec3 translation);

// builds an object transformation object from a position, a direction (towards which the object is facing) and the up-vector
// the function normalizes vectors automatically
glm::mat4 buildMatrixFromOrientation(glm::vec3 position, glm::vec3 direction, glm::vec3 up);

/**
 * computes the distance from point P to the line defined by lineOrigin and lineDirection.
 * lineDirection is assumed to be normalized.
 */
inline float distPointLine(glm::vec2 P, glm::vec2 lineOrigin, glm::vec2 lineDirection) {
	glm::vec2 OP = P - lineOrigin;
	return glm::length(OP - lineDirection * glm::dot(OP, lineDirection));
}

template<typename T> T clamp(T x, T a, T b) {
	return x < a ? a : (x > b ? b : x);
}

/**
 * linearly interpolates between a and b by factor t
 * t is assumed to be in [0.0, 1.0]
 * use clamp on t before calling if unsure
 */
template<typename T> T lerp(T a, T b, float t) {
	return a * (1-t) + b*t;
}

/**
 * sample a value from an array by linearly interpolating across neighbor values
 * The position is a float index, giving the center of the sample kernel
 * The size of the kernel is 1.0
 * the function doesn't do bound checking on the initial position,
 * but it is safe to use on the first or last locations in the vector - will not sample neighbors outside the vector
 */
template<typename T> inline T lerp_lookup(const T* v, int nV, float position) {
	int index = int(position);
	float lerpFact = position - index;
	float value = v[index];
	if (lerpFact < 0.5f && index > 0) {
		// lerp with previous value
		value = lerp(v[index-1], value, lerpFact + 0.5f);
	} else if (lerpFact > 0.5f && index < nV-1) {
		// lerp with next value
		value = lerp(value, v[index+1], lerpFact - 0.5f);
	}
	assertDbg(!std::isnan(value));
	return value;
}

// returns the intersection point of two lines in 3D space (does not care if the point lies on the segments or outside)
// if the lines don't intersect, false is returned in the second value.
std::pair<glm::vec3, bool> intersectLines(glm::vec3 p1, glm::vec3 p2, glm::vec3 q1, glm::vec3 q2);

// compute a normalized plane from 3 points (specified in CW order as seen from the positive side of the plane)
glm::vec4 planeFromPoints(glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const& p3);

/**
 * Casts a ray from the box's center in the given direction and returns the coordinates of the point
 * on the edge of the box that is intersected by the ray
 * length is along OX axis, and width along OY. direction is relative to trigonometric zero (OX+)
 */
glm::vec2 rayIntersectBox(float length, float width, float direction);

// Computes the intersection point of a ray and a plane.
// Assumes [dir] is normalized.
// The 4th component of the returned vector is the distance along the ray from the start (can be negative if the intersection is behind start)
// The first 3 components of the returned vector are the intersection point coordinates.
// Returns false in the second value if the ray is parallel to the plane.
std::pair<glm::vec4, bool> rayIntersectPlane(glm::vec3 const& start, glm::vec3 const& dir, glm::vec4 const& plane);

// computes the dot product of a point and a plane - the distance from the point to the plane
float pointDotPlane(glm::vec3 const& point, glm::vec4 const& plane);

// computes the intersection point between a ray and a triangle and fills [outIntersectionPoint] with the coordinates.
// returns true if the ray intersects the triangle, or false otherwise
bool rayIntersectTri(glm::vec3 const& start, glm::vec3 const& dir,
	glm::vec3 const& p1, glm::vec3 const& p2, glm::vec3 const&p3,
	glm::vec3 &outIntersectionPoint);
