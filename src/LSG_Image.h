#include "main.h"

#ifndef LSG_IMAGE_H
#define LSG_IMAGE_H

class LSG_Image : public LSG_Component
{
public:
	LSG_Image(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Image();

private:
	SDL_Texture*         downscaledTexture;
	std::string          file;
	bool                 fill;
	SDL_Size             imageSize;
	LSG_ImageOrientation orientation;

public:
	SDL_Size     GetImageSize() const;
	SDL_Size     GetSize();
	SDL_Size     GetTextureSize();
	void         Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetImage(const std::string& file, bool fill = false);
	void         SetImage();

private:
	virtual void destroyTextures() override;
	SDL_Size     getMaxSize(const SDL_Rect& background);
	void         render(SDL_Renderer* renderer);
};

#endif
