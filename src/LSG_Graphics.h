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
	static SDL_Point               GetDownscaleFactor(const SDL_Size& fullSize, const SDL_Size& maxSize);
	static SDL_Texture*            GetDownScaledTexture(const std::string& imageFile, const SDL_Point& downscaleFactor);
	static SDL_Rect                GetFillArea(const SDL_Rect& background, int border, int padding = 0);
	static SDL_Color               GetFillColor(const SDL_Color& backgroundColor);
	static std::vector<SDL_Vertex> GetGeometryTriangleHorizontal(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation);
	static std::vector<SDL_Vertex> GetGeometryTriangleVertical(const SDL_Rect& background,   const SDL_Color& color, LSG_TriangleOrientation orientation);
	static SDL_Color               GetInverseColor(const SDL_Color& color);
	static SDL_Color               GetOffsetColor(const SDL_Color& color, int offset);
	static SDL_Size                GetTextureSize(SDL_Texture* texture);
	static SDL_Surface*            GetThumbnail(const std::string& imageFile, const SDL_Size& maxSize);
	static SDL_Surface*            GetThumbnail(SDL_Surface* surface, const SDL_Size& maxSize);
	static SDL_Color               GetThumbColor(const SDL_Color& backgroundColor);
	static SDL_Texture*            GetVector(LSG_VectorIcon icon, const SDL_Color& color, const SDL_Size& size);
	static bool                    IsColorEquals(const SDL_Color& a, const SDL_Color& b);
	static void                    RenderBorder(SDL_Renderer* renderer, int border, const SDL_Color& color, const SDL_Rect& background);
	static void                    RenderFill(SDL_Renderer* renderer, int border, const SDL_Color& color, const SDL_Rect& background);
	static void                    RenderLine(SDL_Renderer* renderer, const SDL_Color& color, int x1, int y1, int x2, int y2);
	static void                    RenderTexture(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, SDL_Texture* texture, const SDL_Size& size);
	static SDL_Color               ToSdlColor(const std::string& color);
	static std::string             ToXmlAttribute(const SDL_Color& color);

private:
	static SDL_Surface*            getDownScaledSurface(const std::string& imageFile, const SDL_Point& downscaleFactor);
	static SDL_Surface*            getDownScaledSurface(SDL_Surface* surface, const SDL_Point& downscaleFactor);
	static std::vector<SDL_Vertex> getGeometryTriangle(const SDL_Rect& background, int paddingX, int paddingY, const SDL_Color& color, LSG_TriangleOrientation orientation);
	static SDL_Texture*            getVector(const std::string& svg);
	static std::string             getVectorBack(const  SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorClose(const SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorMenu(const  SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorNext(const  SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorPageBack(const  SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorPageEnd(const   SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorPageNext(const  SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorPageStart(const SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorToggleOff(const SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorToggleOn(const  SDL_Color& color, const SDL_Size& size);
};

#endif
