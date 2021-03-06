/*
 * Shape2D.cpp
 *
 *  Created on: Nov 14, 2014
 *      Author: bogdan
 */
#include <boglfw/renderOpenGL/Shape2D.h>
#include <boglfw/renderOpenGL/Viewport.h>
#include <boglfw/renderOpenGL/Camera.h>
#include <boglfw/renderOpenGL/shader.h>
#include <boglfw/renderOpenGL/glToolkit.h>
#include <boglfw/renderOpenGL/RenderHelpers.h>
#include <boglfw/math/math3D.h>
#include <boglfw/utils/tesselate-vec2.h>
#include <boglfw/utils/log.h>
#include <boglfw/utils/arrayContainer.h>
#include <boglfw/perf/marker.h>

#include <glm/mat4x4.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLEW_NO_GLU
#include <GL/glew.h>
#include <stdexcept>

static Shape2D* instance = nullptr;

void Shape2D::init() {
	instance = new Shape2D();
}

Shape2D* Shape2D::get() {
	assertDbg(instance && "must be initialized first - call RenderHelpers::load()!");
	return instance;
}

Shape2D::Shape2D() {
	LOGPREFIX("Shape2D");
	glGenVertexArrays(1, &lineVAO_);
	glGenBuffers(1, &lineVBO_);
	glGenBuffers(1, &lineIBO_);
	glGenVertexArrays(1, &triangleVAO_);
	glGenBuffers(1, &triangleVBO_);
	glGenBuffers(1, &triangleIBO_);

	Shaders::createProgram("data/shaders/shape2d.vert", "data/shaders/shape2d.frag", [this](unsigned id) {
		shaderProgram_ = id;
		if (shaderProgram_ == 0) {
			ERROR("Unable to load shape2D shaders!!");
			return;
		}
		indexMatViewport_ = glGetUniformLocation(shaderProgram_, "mViewportInverse");

		unsigned indexPos = glGetAttribLocation(shaderProgram_, "vPos");
		unsigned indexColor = glGetAttribLocation(shaderProgram_, "vColor");

		glBindVertexArray(lineVAO_);
		glBindBuffer(GL_ARRAY_BUFFER, lineVBO_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIBO_);
		glEnableVertexAttribArray(indexPos);
		glEnableVertexAttribArray(indexColor);
		glVertexAttribPointer(indexPos, 3, GL_FLOAT, GL_FALSE, sizeof(s_lineVertex), (void*)offsetof(s_lineVertex, pos));
		glVertexAttribPointer(indexColor, 4, GL_FLOAT, GL_FALSE, sizeof(s_lineVertex), (void*)offsetof(s_lineVertex, rgba));

		glBindVertexArray(triangleVAO_);
		glBindBuffer(GL_ARRAY_BUFFER, triangleVBO_);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIBO_);
		glEnableVertexAttribArray(indexPos);
		glEnableVertexAttribArray(indexColor);
		glVertexAttribPointer(indexPos, 3, GL_FLOAT, GL_FALSE, sizeof(s_lineVertex), (void*)offsetof(s_lineVertex, pos));
		glVertexAttribPointer(indexColor, 4, GL_FLOAT, GL_FALSE, sizeof(s_lineVertex), (void*)offsetof(s_lineVertex, rgba));

		glBindVertexArray(0);
	});

	checkGLError("Shape2D:: vertex array creation");
}

Shape2D::~Shape2D() {
	glDeleteProgram(shaderProgram_);
	glDeleteVertexArrays(1, &triangleVAO_);
	glDeleteVertexArrays(1, &lineVAO_);
	glDeleteBuffers(1, &triangleVBO_);
	glDeleteBuffers(1, &triangleIBO_);
	glDeleteBuffers(1, &lineVBO_);
	glDeleteBuffers(1, &lineIBO_);
}

void Shape2D::unload() {
	delete instance, instance = nullptr;
}

void Shape2D::flush() {
	PERF_MARKER_FUNC;

	// populate device buffers
	glBindBuffer(GL_ARRAY_BUFFER, lineVBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_lineVertex) * lineBuffer_.size(), &lineBuffer_[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lineIBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(lineIndices_[0]) * lineIndices_.size(), &lineIndices_[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, triangleVBO_);
	glBufferData(GL_ARRAY_BUFFER, sizeof(s_lineVertex) * triangleBuffer_.size(), &triangleBuffer_[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triangleIBO_);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangleIndices_[0]) * triangleIndices_.size(), &triangleIndices_[0], GL_DYNAMIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glUseProgram(shaderProgram_);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendEquation(GL_BLEND_EQUATION_ALPHA);
	glDisable(GL_DEPTH_TEST);

	// set up viewport space settings:
	Viewport* vp = RenderHelpers::getActiveViewport();
	if (!vp) {
		assertDbg(!!!"No viewport is currently rendering!");
		return;
	}
	glm::mat4x4 matVP_to_Uniform = vp->viewport2Uniform();
	glUniformMatrix4fv(indexMatViewport_, 1, GL_FALSE, glm::value_ptr(matVP_to_Uniform));

	checkGLError("Shape2D::render() : setup");

	// render triangle primitives:
	auto nTriIndices = triangleIndices_.size();
	if (nTriIndices) {
		glBindVertexArray(triangleVAO_);
		glDrawElements(GL_TRIANGLES, nTriIndices, GL_UNSIGNED_SHORT, 0);
		checkGLError("Shape2D::render() : glDrawElements #1");
	}

	// render line primitives
	glBindVertexArray(lineVAO_);
	for (unsigned l=0; l < lineStrips_.size(); l++) {
		glDrawElements(GL_LINES, lineStrips_[l].length, GL_UNSIGNED_SHORT, (void*)(sizeof(lineIndices_[0]) * lineStrips_[l].offset));
		checkGLError("Shape2D::render() : glDrawElements #2");
	}

	glBindVertexArray(0);
	glUseProgram(0);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	// purge cached data
	lineBuffer_.clear();
	lineIndices_.clear();
	triangleBuffer_.clear();
	triangleIndices_.clear();
	lineStrips_.clear();
}

void Shape2D::drawLine(glm::vec2 point1, glm::vec2 point2, glm::vec3 rgb) {
	drawLine(point1, point2, glm::vec4(rgb, 1));
}

void Shape2D::drawLine(glm::vec2 point1, glm::vec2 point2, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	lineStrips_.push_back({
		lineIndices_.size(),
		2
	});
	lineBuffer_.emplace_back(point1, rgba);
	lineIndices_.push_back(lineBuffer_.size()-1);
	lineBuffer_.emplace_back(point2, rgba);
	lineIndices_.push_back(lineBuffer_.size()-1);
}

void Shape2D::drawLineList(glm::vec2 verts[], int nVerts, glm::vec3 rgb) {
	drawLineList(verts, nVerts, glm::vec4(rgb, 1));
}

void Shape2D::drawLineList(glm::vec2 verts[], int nVerts, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	lineStrips_.push_back({
		lineIndices_.size(),
		nVerts
	});
	for (int i=0; i<nVerts; i++) {
		lineBuffer_.emplace_back(verts[i], rgba);
		lineIndices_.push_back(lineBuffer_.size()-1);
	}
}

void Shape2D::drawLineStrip(glm::vec2 verts[], int nVerts, glm::vec3 rgb) {
	drawLineStrip(verts, nVerts, glm::vec4(rgb, 1));
}

void Shape2D::drawLineStrip(glm::vec2 verts[], int nVerts, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	lineStrips_.push_back({
		lineIndices_.size(),
		(nVerts-1) * 2
	});
	for (int i=0; i<nVerts; i++) {
		lineBuffer_.emplace_back(verts[i], rgba);
		lineIndices_.push_back(lineBuffer_.size()-1);
		if (i > 0 && i < nVerts-1)
			lineIndices_.push_back(lineBuffer_.size()-1);
	}
}

void Shape2D::drawPolygon(glm::vec2 verts[], int nVerts, glm::vec3 rgb) {
	drawPolygon(verts, nVerts, glm::vec4(rgb, 1));
}

void Shape2D::drawPolygon(glm::vec2 verts[], int nVerts, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	lineStrips_.push_back({
		lineIndices_.size(),
		nVerts * 2
	});
	for (int i=0; i<nVerts; i++) {
		lineBuffer_.emplace_back(verts[i], rgba);
		lineIndices_.push_back(lineBuffer_.size()-1);
		if (i > 0) {
			lineIndices_.push_back(lineBuffer_.size()-1);
		}
	}
	lineIndices_.push_back(lineBuffer_.size()-nVerts);
}

void Shape2D::drawPolygonFilled(glm::vec2 verts[], int nVerts, glm::vec3 rgb) {
	drawPolygonFilled(verts, nVerts, glm::vec4(rgb, 1));
}

void Shape2D::drawPolygonFilled(glm::vec2 verts[], int nVerts, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	arrayContainer<glm::vec2> vtx(verts, nVerts);
	arrayContainer<decltype(vtx)> vtxWrap(&vtx, 1);
	std::vector<uint16_t> inds = mapbox::earcut<uint16_t>(vtxWrap);
	assertDbg(inds.size() % 3 == 0);

	triangleBuffer_.reserve(triangleBuffer_.size() + nVerts);
	unsigned base = triangleBuffer_.size();
	for (auto v = verts; v < verts+nVerts; v++)
		triangleBuffer_.emplace_back(*v, rgba);
	triangleIndices_.reserve(triangleIndices_.size() + inds.size());
	for (unsigned i=0; i<inds.size(); i++)
		triangleIndices_.push_back(base + inds[i]);
}

void Shape2D::drawRectangle(glm::vec2 pos, glm::vec2 size, glm::vec3 rgb) {
	drawRectangle(pos, size, glm::vec4(rgb, 1));
}

void Shape2D::drawRectangle(glm::vec2 pos, glm::vec2 size, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	glm::vec2 coords[] {
		pos,
		{pos.x, pos.y + size.y},
		pos + size,
		{pos.x + size.x, pos.y}
	};
	drawPolygon(coords, 4, rgba);
}

void Shape2D::drawRectangleCentered(glm::vec2 pos, glm::vec2 size, glm::vec3 rgb) {
	drawRectangleCentered(pos, size, glm::vec4(rgb, 1));
}

void Shape2D::drawRectangleCentered(glm::vec2 pos, glm::vec2 size, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	auto hSize = size * 0.5f;
	glm::vec2 coords[] {
		pos + glm::vec2{-hSize.x, -hSize.y},
		pos + glm::vec2{-hSize.x, +hSize.y},
		pos + glm::vec2{+hSize.x, +hSize.y},
		pos + glm::vec2{+hSize.x, -hSize.y}
	};
	drawPolygon(coords, 4, rgba);
}

void Shape2D::drawRectangleFilled(glm::vec2 pos, glm::vec2 size, glm::vec3 rgb) {
	drawRectangleFilled(pos, size, glm::vec4(rgb, 1));
}

void Shape2D::drawRectangleFilled(glm::vec2 pos, glm::vec2 size, glm::vec4 rgba) {
	glm::vec2 coords[] {
		pos,
		pos + glm::vec2{0, size.y},
		pos + size,
		pos + glm::vec2{size.x, 0}
	};
	drawPolygonFilled(coords, 4, rgba);
}

void makeCircle(glm::vec2 pos, float radius, int nSides, glm::vec2* outV) {
	PERF_MARKER_FUNC;
	// make a polygon out of the circle
	float phiStep = 2 * PI * 1.f / nSides;
	float phi = 0;
	for (int i=0; i<nSides; i++) {
		outV[i] = pos + glm::vec2{cosf(phi) * radius, sinf(phi) * radius};
		phi += phiStep;
	}
}

void Shape2D::drawCircle(glm::vec2 pos, float radius, int nSides, glm::vec3 rgb) {
	drawCircle(pos, radius, nSides, glm::vec4(rgb, 1));
}

void Shape2D::drawCircle(glm::vec2 pos, float radius, int nSides, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	glm::vec2 *v = new glm::vec2[nSides];
	makeCircle(pos, radius, nSides, v);
	drawPolygon(v, nSides, rgba);
	delete [] v;
}

void Shape2D::drawCircleFilled(glm::vec2 pos, float radius, int nSides, glm::vec3 rgb) {
	drawCircleFilled(pos, radius, nSides, glm::vec4{rgb, 1});
}

void Shape2D::drawCircleFilled(glm::vec2 pos, float radius, int nSides, glm::vec4 rgba) {
	PERF_MARKER_FUNC;
	glm::vec2 *v = new glm::vec2[nSides];
	makeCircle(pos, radius, nSides, v);
	drawPolygonFilled(v, nSides, rgba);
	delete [] v;
}
