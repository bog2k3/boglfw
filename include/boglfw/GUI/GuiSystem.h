/*
 * GuiSystem.h
 *
 *  Created on: Mar 24, 2015
 *      Author: bog
 */

#ifndef GUI_GUISYSTEM_H_
#define GUI_GUISYSTEM_H_

#include <boglfw/GUI/ICaptureManager.h>
#include <boglfw/GUI/GuiContainerElement.h>
#include <memory>

class GuiBasicElement;
class Viewport;
class RenderContext;
class InputEvent;

class GuiSystem : public ICaptureManager {
public:
	// initialize the GUI System on a specified area of the viewport
	GuiSystem(const Viewport* viewport, glm::vec2 position, glm::vec2 size);
	virtual ~GuiSystem() = default;

	void setMouseCapture(GuiBasicElement* elementOrNull) override;

	void addElement(std::shared_ptr<GuiBasicElement> e);
	void addElement(...) = delete;
	void removeElement(std::shared_ptr<GuiBasicElement> e);
	// removes all UI elements from the GUI system
	void clear();
	void draw(RenderContext const& ctx);
	void handleInput(InputEvent &ev);

	glm::vec2 getViewportSize() const;

private:
	const Viewport* viewport_;
	GuiContainerElement rootElement_;
	std::weak_ptr<GuiBasicElement> pFocusedElement_;
	std::weak_ptr<GuiBasicElement> pCaptured_;
	std::weak_ptr<GuiBasicElement> lastUnderMouse_;

	glm::vec2 screenToViewport(glm::vec2 sp) const;
};

#endif /* GUI_GUISYSTEM_H_ */
