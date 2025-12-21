#include "main.h"

#ifndef LSG_IMAGE_H
#define LSG_IMAGE_H

class LSG_Image : public LSG_Component
{
public:
	LSG_Image(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Image();

private:
	SDL_FPoint           aspectRatio;
	std::string          file;
	bool                 fill;
	SDL_Size             imageSize;
	LSG_ImageOrientation orientation;
	SDL_FPoint           scaleFactor;

public:
	SDL_Size     GetSize();
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	void         Render(SDL_Renderer* renderer);
	void         Set(const std::string& file, bool fill = false);
	void         Set();

private:
	void     downscaleTextureIcon(SDL_Size maxSize);
	SDL_Size getMaxSize(const SDL_Rect& background) const;
	void     render(SDL_Renderer* renderer);
	void     rotate();
	bool     scaleDown(const SDL_Point& downscaleFactor) const;
	bool     scaleUp(const SDL_Size& textureSize, SDL_Size maxSize) const;
	void     set();
	void     setTexture();
};

#endif
