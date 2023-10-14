#include "LSG_Graphics.h"

SDL_Color LSG_Graphics::GetFillColor(const SDL_Color& backgroundColor)
{
	const int OFFSET  = 20;
	const int DEFAULT = (255 - OFFSET);
	const int MAX     = (DEFAULT - OFFSET);

	auto r = (uint8_t)(backgroundColor.r < MAX ? (backgroundColor.r + OFFSET) : DEFAULT);
	auto g = (uint8_t)(backgroundColor.g < MAX ? (backgroundColor.g + OFFSET) : DEFAULT);
	auto b = (uint8_t)(backgroundColor.b < MAX ? (backgroundColor.b + OFFSET) : DEFAULT);

	SDL_Color fillColor = { r, g, b, 255 };

	return fillColor;
}

std::vector<SDL_Vertex> LSG_Graphics::GetGeometryTriangleHorizontal(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation)
{
	return LSG_Graphics::getGeometryTriangle(
		background,
		(LSG_ConstScrollBar::Padding + 2),
		LSG_ConstScrollBar::Padding,
		color,
		orientation
	);
}

std::vector<SDL_Vertex> LSG_Graphics::GetGeometryTriangleVertical(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation)
{
	return LSG_Graphics::getGeometryTriangle(
		background,
		LSG_ConstScrollBar::Padding,
		(LSG_ConstScrollBar::Padding + 2),
		color,
		orientation
	);
}

std::vector<SDL_Vertex> LSG_Graphics::getGeometryTriangle(const SDL_Rect& background, float paddingX, float paddingY, const SDL_Color& color, LSG_TriangleOrientation orientation)
{
	auto left   = (float)(background.x + paddingX);
	auto center = (float)(background.x + (background.w / 2));
	auto right  = (float)(background.x + background.w - paddingX);
	auto top    = (float)(background.y + paddingY);
	auto middle = (float)(background.y + (background.h / 2));
	auto bottom = (float)(background.y + background.h - paddingY);

	// LSG_TRIANGLE_ORIENTATION_TOP
	auto x1 = left;
	auto y1 = bottom;
	auto x2 = center;
	auto y2 = top;
	auto x3 = right;
	auto y3 = bottom;

	switch (orientation) {
	case LSG_TRIANGLE_ORIENTATION_LEFT:
		y1 = middle;
		x2 = right;
		break;
	case LSG_TRIANGLE_ORIENTATION_RIGHT:
		x2 = right;
		y2 = middle;
		x3 = left;
		y3 = top;
		break;
	case LSG_TRIANGLE_ORIENTATION_DOWN:
		y1 = top;
		y2 = bottom;
		y3 = top;
		break;
	default:
		break;
	}

	SDL_Vertex vertex1 = { { x1, y1 }, { color.r, color.g, color.b, color.a }, {} };
	SDL_Vertex vertex2 = { { x2, y2 }, { color.r, color.g, color.b, color.a }, {} };
	SDL_Vertex vertex3 = { { x3, y3 }, { color.r, color.g, color.b, color.a }, {} };

	return { vertex1, vertex2, vertex3 };
}

SDL_Color LSG_Graphics::GetOffsetColor(const SDL_Color& color, int offset)
{
	const int DEFAULT = (255 - offset);
	const int MAX     = (DEFAULT - offset);

	auto r = (uint8_t)(color.r < MAX ? (color.r + offset) : DEFAULT);
	auto g = (uint8_t)(color.g < MAX ? (color.g + offset) : DEFAULT);
	auto b = (uint8_t)(color.b < MAX ? (color.b + offset) : DEFAULT);

	SDL_Color offsetColor = { r, g, b, 255 };

	return offsetColor;
}

SDL_Size LSG_Graphics::GetTextureSize(SDL_Texture* texture)
{
	if (!texture)
		return {};

	SDL_Size textureSize = {};
	SDL_QueryTexture(texture, nullptr, nullptr, &textureSize.width, &textureSize.height);

	return textureSize;
}

SDL_Color LSG_Graphics::GetThumbColor(const SDL_Color& backgroundColor)
{
	auto r = (uint8_t)std::max(50, std::min(200, (255 - backgroundColor.r)));
	auto g = (uint8_t)std::max(50, std::min(200, (255 - backgroundColor.g)));
	auto b = (uint8_t)std::max(50, std::min(200, (255 - backgroundColor.b)));

	SDL_Color thumbColor = { r, g, b, 255 };

	return thumbColor;
}

bool LSG_Graphics::IsColorEquals(const SDL_Color& a, const SDL_Color& b)
{
	return ((a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a));
}
