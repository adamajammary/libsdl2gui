#include "main.h"

#ifndef LSG_BUTTON_H
#define LSG_BUTTON_H

class LSG_Button : public LSG_IEvent, public LSG_Text
{
public:
	LSG_Button(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Button() {}

private:
	static const int DefaultSpacingX = 10;
	static const int DefaultSpacingY = 5;

private:
	std::string iconPath;
	SDL_Size    iconSize;

public:
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	virtual void Render(SDL_Renderer* renderer);
	void         Set(const std::string& text, const std::string& iconPath);
	void         Set();

private:
	void         downscaleTextureIcon(int maxSize);
	SDL_Rect     getIconDestination(int iconSize);
	SDL_Rect     getTextClip();
	SDL_Rect     getTextDestination(const SDL_Rect& clip);
	void         render(SDL_Renderer* renderer);
	bool         scaleDown(const SDL_Point& downscaleFactor) const;
	bool         scaleUp(const SDL_Size& textureSize, int maxSize) const;
	virtual void sendEvent(LSG_EventType type) const override;
	void         setLayoutHorizontal(SDL_Rect& iconDestination, SDL_Rect& textClip, SDL_Rect& textDestination);
	void         setLayoutVertical(SDL_Rect& iconDestination, SDL_Rect& textClip, SDL_Rect& textDestination);
};

#endif
