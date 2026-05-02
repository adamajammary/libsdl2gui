#include "main.h"

#ifndef LSG_BUTTON_H
#define LSG_BUTTON_H

class LSG_Button : public LSG_Text
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
	std::string  GetIconPath() const;
	std::string  GetText() const;
	void         OnMouseClick(const SDL_Point& mousePosition) const;
	void         Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void Render(SDL_Renderer* renderer) override;
	void         Set(const std::string& text, const std::string& iconPath);
	void         Set();

private:
	void         downscaleTextureIcon(int maxSize);
	SDL_Rect     getIconDestination(int iconSize) const;
	SDL_Rect     getTextClip() const;
	SDL_Rect     getTextDestination(const SDL_Rect& clip) const;
	void         render(SDL_Renderer* renderer);
	bool         scaleDown(const SDL_Point& downscaleFactor) const;
	bool         scaleUp(const SDL_Size& textureSize, int maxSize) const;
	void         setLayoutHorizontal(SDL_Rect& iconDestination, SDL_Rect& textClip, SDL_Rect& textDestination) const;
	void         setLayoutVertical(SDL_Rect&   iconDestination, SDL_Rect& textClip, SDL_Rect& textDestination) const;
};

#endif
