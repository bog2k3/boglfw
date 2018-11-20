/*
 * IGuiElement.h
 *
 *  Created on: Mar 24, 2015
 *      Author: bog
 */

#ifndef GUI_IGUIELEMENT_H_
#define GUI_IGUIELEMENT_H_

#include "constants.h"
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

class Viewport;
class ICaptureManager;

class IGuiElement {
public:
	virtual ~IGuiElement() {}

	virtual void getBoundingBox(glm::vec2 &outMin, glm::vec2 &outMax) const = 0;
	//virtual int zIndex() const = 0;
	//virtual void setZIndex(int z) = 0;
	virtual bool isVisible() const = 0;

	void setCaptureManager(ICaptureManager* mgr) { captureManager_ = mgr; }
	ICaptureManager* getCaptureManager() const { return captureManager_; }

protected:
	friend class GuiSystem;

	virtual void draw(Viewport* vp, glm::vec2 frameTranslation, glm::vec2 frameScale) = 0;

	virtual void mouseEnter() {}
	virtual void mouseLeave() {}
	virtual void mouseDown(MouseButtons button) {}
	virtual void mouseUp(MouseButtons button) {}
	virtual void mouseMoved(glm::vec2 delta, glm::vec2 position) {}
	virtual void mouseScroll(float delta) {}
	virtual bool keyDown(int keyCode) {return false;}	// return true if the key was consumed
	virtual bool keyUp(int keyCode) {return false;}		// return true if the key was consumed
	virtual bool keyChar(char c) {return false;}		// return true if the key was consumed

	virtual void focusGot() {}
	virtual void focusLost() {}

private:
	ICaptureManager *captureManager_ = nullptr;
};

#endif /* GUI_IGUIELEMENT_H_ */
