#include "main.h"

#ifndef LSG_SLIDER_H
#define LSG_SLIDER_H

class LSG_Slider : public LSG_Component, public LSG_IEvent
{
public:
	LSG_Slider(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Slider() {}

private:
	bool        fillProgress;
	bool        isSlideActive;
	std::string orientation;
	SDL_Color   progressColor;
	SDL_Rect    thumb;
	int         thumbBorder;
	SDL_Color   thumbBorderColor;
	SDL_Color   thumbColor;
	int         thumbWidth;
	double      value;

public:
	double       GetValue();
	virtual bool MouseClick(const SDL_MouseButtonEvent& event) override;
	void         MouseMove(const SDL_Point& mousePosition);
	void         MouseScroll(int offset);
	void         MouseUp();
	virtual void Render(SDL_Renderer* renderer) override;
	virtual void SetColors() override;
	void         SetValue(double value);

private:
	virtual void sendEvent(LSG_EventType type) override;
	void         setValue(const SDL_Point& mousePosition);
	void         setValue(int offset);

};

#endif
