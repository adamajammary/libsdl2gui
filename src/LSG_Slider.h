#include "main.h"

#ifndef LSG_SLIDER_H
#define LSG_SLIDER_H

struct LSG_SliderThumb
{
	SDL_Color   borderColor  = {};
	int         borderRadius = 0;
	int         borderWidth  = 0;
	SDL_Color   color        = {};
	int         width        = 0;
	int         widthDefault = 0;
};

class LSG_Slider : public LSG_ProgressBar, public LSG_IEvent
{
public:
	LSG_Slider(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Slider() {}

private:
	static inline const SDL_Color DefaultThumbColor = { 128, 128, 128, 255 };

private:
	static const int DefaultPartSize   = 10;
	static const int DefaultThumbWidth = 20;

private:
	bool                fillProgress;
	bool                isSlideActive;
	std::string         orientation;
	int                 partSize;
	std::vector<double> parts;
	LSG_SliderThumb     thumb;

public:
	std::vector<double> GetParts();
	virtual void        OnMouseClick(const SDL_Point& mousePosition) override;
	bool                OnMouseClickThumb(const SDL_Point& mousePosition);
	bool                OnMouseMove(const SDL_Point& mousePosition);
	void                OnMouseScroll(int offset);
	void                OnMouseUp();
	virtual void        Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	void                Render(SDL_Renderer* renderer);
	virtual void        SetColors() override;
	void                SetParts(const std::vector<double>& parts);

private:
	SDL_Rect     getBackground() const;
	int          getProgressWidth(const SDL_Rect& background) const;
	SDL_Rect     getThumb(const SDL_Rect& background, int progressWidth) const;
	void         render(SDL_Renderer* renderer);
	void         renderBackground(SDL_Renderer* renderer, const SDL_Rect& background);
	void         renderParts(SDL_Renderer* renderer, const SDL_Rect& background);
	void         renderThumb(SDL_Renderer* renderer, const SDL_Rect& background, int progressWidth);
	virtual void sendEvent(LSG_EventType type) const override;
	void         setValue(const SDL_Point& mousePosition);
	void         setValue(int offset);
};

#endif
