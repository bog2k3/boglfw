/*
 * Shape3D.h
 *
 *  Created on: Sep 12, 2017
 *      Author: bogdan
 */

#ifndef RENDEROPENGL_SHAPE3D_H_
#define RENDEROPENGL_SHAPE3D_H_

#include <boglfw/math/aabb.h>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include <set>
#include <string>

class Viewport;

// renders 3D shapes in world space
class Shape3D {
public:
	static Shape3D* get();
	virtual ~Shape3D();

	// draw a single line segment
	void drawLine(glm::vec3 point1, glm::vec3 point2, glm::vec3 rgb);
	void drawLine(glm::vec3 point1, glm::vec3 point2, glm::vec4 rgba);
	// draw a list of separate lines (pairs of two vertices)
	void drawLineList(glm::vec3 verts[], int nVerts, glm::vec3 rgb);
	void drawLineList(glm::vec3 verts[], int nVerts, glm::vec4 rgba);
	// draw a line strip (connected lines)
	void drawLineStrip(glm::vec3 verts[], int nVerts, glm::vec3 rgb);
	void drawLineStrip(glm::vec3 verts[], int nVerts, glm::vec4 rgba);
	// draw a rectangle; pos is the top-left position
	void drawRectangleXOY(glm::vec2 pos, glm::vec2 size, glm::vec3 rgb);
	void drawRectangleXOY(glm::vec2 pos, glm::vec2 size, glm::vec4 rgba);
	// draw a rectangle; pos is the center position
	void drawRectangleXOYCentered(glm::vec2 pos, glm::vec2 size, float rotation, glm::vec3 rgb);
	void drawRectangleXOYCentered(glm::vec2 pos, glm::vec2 size, float rotation, glm::vec4 rgba);

	// draw a polygon
	void drawPolygon(glm::vec3 verts[], int nVerts, glm::vec3 rgb);
	void drawPolygon(glm::vec3 verts[], int nVerts, glm::vec4 rgba);

	// draw a circle
	void drawCircleXOY(glm::vec2 pos, float radius, int nSides, glm::vec3 rgb);
	void drawCircleXOY(glm::vec2 pos, float radius, int nSides, glm::vec4 rgba);

	void drawAABB(AABB const& aabb, glm::vec3 rgb);
	void drawAABB(AABB const& aabb, glm::vec4 rgba);

	// sets a transform matrix that will affect all future drawXXX calls
	void setTransform(glm::mat4 mat);
	void resetTransform();

protected:
	friend class RenderHelpers;
	static void init();
	static void unload();
	void flush();
	Shape3D();

private:
	void transform(glm::vec3* v[], int n);
	void transform(glm::vec3 v[], int n);

	struct s_vertex {
		glm::vec3 pos;
		glm::vec4 rgba; 	// color
	};
	// line buffers
	std::vector<s_vertex> buffer_;
	std::vector<uint32_t> indices_;
	glm::mat4 transform_ {1};
	bool transformActive_ = false;

	unsigned lineShaderProgram_ = 0;
	unsigned indexMatProjView_ = 0;
	unsigned VAO_ = 0;
	unsigned VBO_ = 0;
	unsigned IBO_ = 0;
};

#endif /* RENDEROPENGL_SHAPE3D_H_ */
