#include "main.h"

#ifndef LSG_GRAPHICS_H
#define LSG_GRAPHICS_H

enum LSG_TriangleOrientation
{
	LSG_TRIANGLE_ORIENTATION_LEFT,
	LSG_TRIANGLE_ORIENTATION_RIGHT,
	LSG_TRIANGLE_ORIENTATION_UP,
	LSG_TRIANGLE_ORIENTATION_DOWN
};

class LSG_Graphics
{
private:
	LSG_Graphics()  {}
	~LSG_Graphics() {}

public:
	static SDL_Color               GetFillColor(const SDL_Color& backgroundColor);
	static std::vector<SDL_Vertex> GetGeometryTriangleHorizontal(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation);
	static std::vector<SDL_Vertex> GetGeometryTriangleVertical(const SDL_Rect& background,   const SDL_Color& color, LSG_TriangleOrientation orientation);
	static SDL_Color               GetOffsetColor(const SDL_Color& color, int offset);
	static SDL_Size                GetTextureSize(SDL_Texture* texture);
	static SDL_Color               GetThumbColor(const SDL_Color& backgroundColor);
	static bool                    IsColorEquals(const SDL_Color& a, const SDL_Color& b);

private:
	static std::vector<SDL_Vertex> getGeometryTriangle(const SDL_Rect& background, float paddingX, float paddingY, const SDL_Color& color, LSG_TriangleOrientation orientation);

};

#endif
