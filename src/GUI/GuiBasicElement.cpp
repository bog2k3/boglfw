/*
 * GuiBasicElement.cpp
 *
 *  Created on: Mar 25, 2015
 *      Author: bog
 */

#include <boglfw/GUI/GuiBasicElement.h>
#include <boglfw/GUI/GuiContainerElement.h>
#include <boglfw/utils/assert.h>

#include <glm/geometric.hpp>

GuiBasicElement::GuiBasicElement() {
	size_ = userSize_;
	updateBBox();
}

GuiBasicElement::~GuiBasicElement() {
	assertDbg(!parent_ || !parent_->findElement(this));
}

bool GuiBasicElement::containsPoint(glm::vec2 const& p) const {
	return true; // default to rectangular shape which contains all points within the bounding box
}

void GuiBasicElement::setPosition(glm::vec2 position) {
	userPosition_ = position;
	if (parent_)
		parent_->refreshLayout();
	else
		updateBBox();
}

void GuiBasicElement::setSize(glm::vec2 size) {
	userSize_ = size;
	if (parent_)
		parent_->refreshLayout();
	else
		updateBBox();
}

void GuiBasicElement::setMinSize(glm::vec2 minSize) {
	minSize_ = minSize;
	if (parent_)
		parent_->refreshLayout();
	else {
		size_ = userSize_;
		if (minSize_.x != 0 && size_.x < minSize_.x)
			size_.x = minSize_.x;
		if (minSize_.y != 0 && size_.y < minSize_.y)
			size_.y = minSize_.y;
		updateBBox();
	}
}

void GuiBasicElement::setMaxSize(glm::vec2 maxSize) {
	maxSize_ = maxSize;
	if (parent_)
		parent_->refreshLayout();
	else {
		size_ = userSize_;
		if (maxSize_.x != 0 && size_.x > maxSize_.x)
			size_.x = maxSize_.x;
		if (maxSize_.y != 0 && size_.y > maxSize_.y)
			size_.y = maxSize_.y;
		updateBBox();
	}
}

void GuiBasicElement::mouseEnter() {
	isMouseIn_ = true;
}

void GuiBasicElement::mouseLeave() {
	isMouseIn_ = false;
}

void GuiBasicElement::mouseDown(MouseButtons button) {
	if (isMouseIn_) {
		isMousePressed_[(int)button] = true;
		mouseTravel_[(int)button] = glm::vec2(0);
	}
}

void GuiBasicElement::mouseUp(MouseButtons button) {
	isMousePressed_[(int)button] = false;
	if (isMouseIn_ && glm::length(mouseTravel_[(int)button]) <= MAX_CLICK_TRAVEL)
		clicked(lastMousePosition_, button);
}

void GuiBasicElement::mouseMoved(glm::vec2 delta, glm::vec2 position) {
	for (int i=0; i<3; i++)
		mouseTravel_[i] += delta;
	lastMousePosition_ = position;
}

void GuiBasicElement::updateBBox() {
	bboxMin_ = position_;
	bboxMax_ = bboxMin_ + size_;
}

ICaptureManager* GuiBasicElement::getCaptureManager() const {
	if (!captureManager_ && parent_) {
		captureManager_ = parent_->getCaptureManager();
	}
	return captureManager_;
}
