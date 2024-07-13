#include "main.h"

#ifndef LSG_GRAPHICS_H
#define LSG_GRAPHICS_H

class LSG_Graphics
{
private:
	LSG_Graphics()  {}
	~LSG_Graphics() {}

public:
	static SDL_Rect                GetDestinationAligned(const SDL_Rect& background, const SDL_Size& size, const LSG_Alignment& alignment);
	static SDL_Size                GetDownscaledSize(const SDL_Size& oldSize, const SDL_Size& newSize);
	static int                     GetDPIScaled(int value);
	static SDL_Color               GetFillColor(const SDL_Color& backgroundColor);
	static std::vector<SDL_Vertex> GetGeometryTriangleHorizontal(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation);
	static std::vector<SDL_Vertex> GetGeometryTriangleVertical(const SDL_Rect& background,   const SDL_Color& color, LSG_TriangleOrientation orientation);
	static LSG_ImageOrientation    GetImageOrientation(const std::string& imageFile);
	static SDL_Texture*            GetVectorBack(const SDL_Color& color,  const SDL_Size& size);
	static SDL_Texture*            GetVectorClose(const SDL_Color& color, const SDL_Size& size);
	static SDL_Texture*            GetVectorMenu(const SDL_Color& color,  const SDL_Size& size);
	static SDL_Texture*            GetVectorNext(const SDL_Color& color,  const SDL_Size& size);
	static SDL_Texture*            GetVectorPageBack(const SDL_Color& color, const SDL_Size& size);
	static SDL_Texture*            GetVectorPageEnd(const SDL_Color& color, const SDL_Size& size);
	static SDL_Texture*            GetVectorPageNext(const SDL_Color& color, const SDL_Size& size);
	static SDL_Texture*            GetVectorPageStart(const SDL_Color& color, const SDL_Size& size);
	static SDL_Color               GetOffsetColor(const SDL_Color& color, int offset);
	static SDL_Texture*            GetTextureDownScaled(const std::string& imageFile, const SDL_Size& newSize);
	static SDL_Size                GetTextureSize(SDL_Texture* texture);
	static SDL_Size                GetTextureSize(std::vector<SDL_Texture*> textures, LSG_Orientation orientation);
	static SDL_Color               GetThumbColor(const SDL_Color& backgroundColor);
	static bool                    IsColorEquals(const SDL_Color& a, const SDL_Color& b);
	static SDL_Color               ToSdlColor(const std::string& color);
	static std::string             ToXmlAttribute(const SDL_Color& color);

private:
	static std::vector<SDL_Vertex> getGeometryTriangle(const SDL_Rect& background, int paddingX, int paddingY, const SDL_Color& color, LSG_TriangleOrientation orientation);
	static SDL_Texture*            getVector(const std::string& svg);
};

#endif
