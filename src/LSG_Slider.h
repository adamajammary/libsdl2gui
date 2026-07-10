#include "main.h"

#ifndef LSG_SLIDER_H
#define LSG_SLIDER_H

struct LSG_SliderPartItem : LSG_SliderPart
{
	SDL_Rect destination = {};

	LSG_SliderPartItem(const LSG_SliderPart& part) : LSG_SliderPart(part.value, part.tooltip) {}
};

using LSG_SliderPartItems = std::vector<LSG_SliderPartItem>;

struct LSG_SliderThumb
{
	SDL_Color   borderColor = {};
	int         borderWidth = 0;
	SDL_Color   color       = {};
	int         size        = 0;
};

struct LSG_SliderTooltip
{
	std::string id            = "";
	SDL_Point   mousePosition = {};
	std::string onHoverText   = "";
	std::string text          = "";
};

class LSG_Slider : public LSG_ProgressBar
{
public:
	LSG_Slider(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Slider() {}

private:
	static inline const SDL_Color DefaultThumbColor = { 128, 128, 128, 255 };

private:
	static const int DefaultPartSize = 5;

private:
	int                 barWidth;
	bool                fillProgress;
	bool                isSlideActive;
	LSG_OnHoverCallback onHoverCB;
	std::string         orientation;
	int                 partSize;
	LSG_SliderPartItems parts;
	LSG_SliderThumb     thumb;

public:
	void            AddPart(LibXml::xmlNode* node);
	LSG_SliderParts GetParts() const;
	double          GetValue(const SDL_Point& mousePosition) const;
	void            OnMouseClick(const SDL_Point& mousePosition);
	bool            OnMouseClickThumb(const SDL_Point& mousePosition);
	bool            OnMouseMove(const SDL_Point& mousePosition);
	void            OnMouseScroll(int offset);
	void            OnMouseUp();
	void            Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void    Render(SDL_Renderer* renderer) override;
	virtual void    RenderTooltip(SDL_Renderer* renderer) const override;
	virtual void    SetColors() override;
	void            SetOnHoverCallback(const LSG_OnHoverCallback& callback);
	void            SetParts(const LSG_SliderParts& parts);

private:
	SDL_Rect     getBar() const;
	int          getBarWidth(bool isVertical) const;
	int          getProgressWidth() const;
	SDL_Rect     getThumb() const;
	int          getThumbSize(bool isVertical) const;
	std::string  getTooltip(const LSG_SliderTooltip& tooltip) const;
	bool         isMouseOverPart(const SDL_Point& mousePosition, const SDL_Rect& bar, size_t index) const;
	void         render(SDL_Renderer*      renderer);
	void         renderBar(SDL_Renderer*   renderer, const SDL_Rect& bar) const;
	void         renderParts(SDL_Renderer* renderer, const SDL_Rect& bar);
	void         renderThumb(SDL_Renderer* renderer) const;
	void         renderTooltip(SDL_Renderer* renderer, const LSG_SliderTooltip& tooltip) const;
	virtual void sendEvent(LSG_EventType type) const override;
	void         setValue(const SDL_Point& mousePosition);
	void         setValue(int offset);
};

#endif
