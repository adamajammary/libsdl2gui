#include "main.h"

#ifndef LSG_SLIDER_H
#define LSG_SLIDER_H

class LSG_Slider : public LSG_Component, public LSG_IEvent
{
public:
	LSG_Slider(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Slider() {}

private:
	static inline const SDL_Color DefaultBackgroundColor = { 192, 192, 192, 255 };
	static inline const SDL_Color DefaultProgressColor   = { 20, 130, 255, 255 };
	static inline const SDL_Color DefaultThumbColor      = { 128, 128, 128, 255 };

private:
	static const int DefaultThumbWidth = 10;

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
	int         thumbWidthDefault;
	double      value;

public:
	double       GetValue() const;
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
	bool         OnMouseClickThumb(const SDL_Point& mousePosition);
	void         OnMouseMove(const SDL_Point& mousePosition);
	void         OnMouseScroll(int offset);
	void         OnMouseUp();
	virtual void Render(SDL_Renderer* renderer) override;
	virtual void SetColors() override;
	void         SetValue(double value);

private:
	virtual void sendEvent(LSG_EventType type) override;
	void         setValue(const SDL_Point& mousePosition);
	void         setValue(int offset);

};

#endif
