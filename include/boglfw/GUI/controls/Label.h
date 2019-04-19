/*
 * Label.h
 *
 *  Created on: Apr 15, 2019
 *      Author: bogdan
 */

#ifndef GUI_CONTROLS_LABEL_H_
#define GUI_CONTROLS_LABEL_H_

#include <boglfw/GUI/GuiBasicElement.h>
#include <string>

class Label: public GuiBasicElement {
public:
	Label(glm::vec2 pos, int fontSize, std::string text);
	virtual ~Label();

	std::string text() const { return text_; };

	virtual void draw(RenderContext const& ctx, glm::vec2 frameTranslation, glm::vec2 frameScale) override;

protected:
	std::string text_;
	int fontSize_;
};

#endif /* GUI_CONTROLS_LABEL_H_ */