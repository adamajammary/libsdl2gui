#include "main.h"

#ifndef LSG_GRAPHICS_H
#define LSG_GRAPHICS_H

class LSG_Graphics
{
private:
	LSG_Graphics()  {}
	~LSG_Graphics() {}

private:
	static const int DefaultTooltipBorderRadius = 5;
	static const int DefaultTooltipBorderWidth  = 1;
	static const int DefaultTooltipFontSize     = 14;
	static const int DefaultTooltipOffsetCursor = 25;
	static const int DefaultTooltipPadding      = 5;

private:
	static LSG_UmapTextures textures;

public:
	static void                    DestroyTexture(const std::string& id);
	static void                    DestroyTextures();
	static void                    DestroyTextures(const std::string& prefixId);
	static SDL_Rect                GetDestinationAligned(const SDL_Rect& background, const SDL_Size& size, const LSG_Alignment& alignment);
	static SDL_Point               GetDownscaleFactor(const SDL_Size& fullSize, const SDL_Size& maxSize);
	static SDL_Texture*            GetDownScaledTexture(const std::string& imageFile, const SDL_Point& downscaleFactor);
	static SDL_Rect                GetFillArea(const SDL_Rect& background, int borderWidth, int padding = 0);
	static std::vector<SDL_Vertex> GetGeometryTriangleHorizontal(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation);
	static std::vector<SDL_Vertex> GetGeometryTriangleVertical(const SDL_Rect& background,   const SDL_Color& color, LSG_TriangleOrientation orientation);
	static SDL_Color               GetInverseColor(const SDL_Color& color);
	static SDL_Color               GetOffsetColor(const SDL_Color& color, int offset);
	static SDL_Size                GetTextureSize(SDL_Texture* texture);
	static SDL_Surface*            GetThumbnail(const std::string& imageFile, const SDL_Size& maxSize);
	static SDL_Surface*            GetThumbnail(SDL_Surface* surface, const SDL_Size& maxSize);
	static SDL_Color               GetThumbColor(const SDL_Color& backgroundColor);
	static SDL_Texture*            GetVector(LSG_Vector vector, const SDL_Color& color, const SDL_Size& size);
	static bool                    IsColorEquals(const SDL_Color& a, const SDL_Color& b);
	static void                    RenderBorder(SDL_Renderer* renderer, int borderWidth, const SDL_Color& color, const SDL_Rect& background);
	static void                    RenderFill(SDL_Renderer*   renderer, int borderWidth, const SDL_Color& color, const SDL_Rect& background);
	static void                    RenderFillCircle(SDL_Renderer* renderer, const SDL_Color& color, const SDL_Rect& background, const std::string& id);
	static void                    RenderFillCircleWithBorder(SDL_Renderer* renderer, const SDL_Color& fillColor, const SDL_Rect& background, const SDL_Color& borderColor, int borderWidth, const std::string& id);
	static void                    RenderFillRounded(SDL_Renderer*       renderer, int borderRadius, const SDL_Color& color, const SDL_Rect& background, const std::string& id);
	static void                    RenderFillRoundedBottom(SDL_Renderer* renderer, int borderRadius, const SDL_Color& color, const SDL_Rect& background, const std::string& id);
	static void                    RenderFillRoundedLeft(SDL_Renderer*   renderer, int borderRadius, const SDL_Color& color, const SDL_Rect& background, const std::string& id);
	static void                    RenderFillWithRoundedBorder(SDL_Renderer* renderer, const SDL_Color& fillColor, const SDL_Color& borderColor, int borderRadius, int borderWidth, const SDL_Rect& background, const std::string& id);
	static void                    RenderLine(SDL_Renderer* renderer, const SDL_Color& color, int x1, int y1, int x2, int y2);
	static void                    RenderRoundedCorners(SDL_Renderer*       renderer, const SDL_Color& fillColor, int borderRadius, const SDL_Rect& background, const std::string& id);
	static void                    RenderRoundedCornersBottom(SDL_Renderer* renderer, const SDL_Color& fillColor, int borderRadius, const SDL_Rect& background, const std::string& id);
	static void                    RenderRoundedCornersTop(SDL_Renderer*    renderer, const SDL_Color& fillColor, int borderRadius, const SDL_Rect& background, const std::string& id);
	static void                    RenderTexture(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, SDL_Texture* texture, const SDL_Size& size);
	static void                    RenderTextureWithRoundedCorners(SDL_Renderer* renderer, SDL_Texture* texture, const SDL_Rect& destination, const SDL_Rect* clip, int radius, const SDL_Color& backgroundColor, const std::string& id);
	static void                    RenderTooltip(SDL_Renderer* renderer, const std::string& text, const SDL_Point& position, const std::string& id);
	static void                    Rotate(LSG_ItemImage& image);
	static SDL_Color               ToSdlColor(const std::string& color);
	static std::string             ToXmlAttribute(const SDL_Color& color);
	static void                    UpdateTexture(SDL_Texture* texture, SDL_Surface* surface);

private:
	static SDL_Surface*            getDownScaledSurface(const std::string& imageFile, const SDL_Point& downscaleFactor);
	static SDL_Surface*            getDownScaledSurface(SDL_Surface* surface, const SDL_Point& downscaleFactor);
	static std::vector<SDL_Vertex> getGeometryTriangle(const SDL_Rect& background, int paddingX, int paddingY, const SDL_Color& color, LSG_TriangleOrientation orientation);
	static float                   getOpacity(const SDL_Color& color);
	static std::string             getPathCornerBottomLeft(int  radius, const SDL_Size& size);
	static std::string             getPathCornerBottomRight(int radius, const SDL_Size& size);
	static std::string             getPathCornerTopLeft(int     radius, const SDL_Size& size);
	static std::string             getPathCornerTopRight(int    radius, const SDL_Size& size);
	static SDL_Texture*            getVector(const std::string& svg);
	static std::string             getVectorBack(const SDL_Color&  color, const SDL_Size& size);
	static std::string             getVectorCheck(const SDL_Color& color, const SDL_Size& size);
	static SDL_Texture*            getVectorCircle(const SDL_Color& color, int radius);
	static SDL_Texture*            getVectorCircleWithBorder(const SDL_Color& fillColor, const SDL_Color& borderColor, int borderWidth, int radius);
	static std::string             getVectorClose(const SDL_Color&     color, const SDL_Size& size);
	static std::string             getVectorMenu(const SDL_Color&      color, const SDL_Size& size);
	static std::string             getVectorNext(const SDL_Color&      color, const SDL_Size& size);
	static std::string             getVectorPageBack(const SDL_Color&  color, const SDL_Size& size);
	static std::string             getVectorPageEnd(const SDL_Color&   color, const SDL_Size& size);
	static std::string             getVectorPageNext(const SDL_Color&  color, const SDL_Size& size);
	static std::string             getVectorPageStart(const SDL_Color& color, const SDL_Size& size);
	static SDL_Texture*            getVectorRectangleFillRoundedBottom(const SDL_Color& color, int borderRadius, const SDL_Size& size);
	static SDL_Texture*            getVectorRectangleFillRoundedLeft(const SDL_Color&   color, int borderRadius, const SDL_Size& size);
	static SDL_Texture*            getVectorRoundedCorners(const SDL_Color&       backgroundColor, int borderRadius, const SDL_Size& size);
	static SDL_Texture*            getVectorRoundedCornersBottom(const SDL_Color& backgroundColor, int borderRadius, const SDL_Size& size);
	static SDL_Texture*            getVectorRoundedCornersTop(const SDL_Color&    backgroundColor, int borderRadius, const SDL_Size& size);
	static SDL_Texture*            getVectorRoundedRectangleFill(const SDL_Color& color, int borderRadius, const SDL_Size& size);
	static SDL_Texture*            getVectorRoundedRectangleWithBorder(const SDL_Color& fillColor, const SDL_Color& borderColor, int borderRadius, int borderWidth, const SDL_Size& size);
	static std::string             getVectorToggleOff(const SDL_Color& color, const SDL_Size& size);
	static std::string             getVectorToggleOn(const SDL_Color&  color, const SDL_Size& size);
};

#endif
