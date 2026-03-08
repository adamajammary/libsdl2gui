#include "LSG_Graphics.h"

LSG_UmapTextures LSG_Graphics::textures = {};

void LSG_Graphics::DestroyTexture(const std::string& id)
{
	if (!LSG_Graphics::textures.contains(id))
		return;

	SDL_DestroyTexture(LSG_Graphics::textures[id]);

	LSG_Graphics::textures.erase(id);
}

void LSG_Graphics::DestroyTextures()
{
	if (LSG_Graphics::textures.empty())
		return;

	for (const auto& texture : LSG_Graphics::textures)
		SDL_DestroyTexture(texture.second);

	LSG_Graphics::textures.clear();
}

SDL_Rect LSG_Graphics::GetDestinationAligned(const SDL_Rect& background, const SDL_Size& size, const LSG_Alignment& alignment)
{
	SDL_Rect destination = {
		background.x,
		background.y,
		std::min(size.width,  background.w),
		std::min(size.height, background.h)
	};

	if (alignment.halign == LSG_HALIGN_CENTER)
		destination.x += ((background.w - destination.w) / 2);
	else if (alignment.halign == LSG_HALIGN_RIGHT)
		destination.x += (background.w - destination.w);

	if (alignment.valign == LSG_VALIGN_MIDDLE)
		destination.y += ((background.h - destination.h) / 2);
	else if (alignment.valign == LSG_VALIGN_BOTTOM)
		destination.y += (background.h - destination.h);

	return destination;
}

SDL_Point LSG_Graphics::GetDownscaleFactor(const SDL_Size& fullSize, const SDL_Size& maxSize)
{
	if ((maxSize.width < 1) || (maxSize.height < 1))
		return { 1, 1 };

	if ((fullSize.width <= (maxSize.width * 2)) && ((fullSize.height <= (maxSize.height * 2))))
		return { 1, 1 };

	SDL_Point downScaleFactor = {
		(fullSize.width  / maxSize.width),
		(fullSize.height / maxSize.height)
	};

	return downScaleFactor;
}

/**
 * @throws runtime_error
 */
SDL_Surface* LSG_Graphics::getDownScaledSurface(const std::string& imageFile, const SDL_Point& downscaleFactor)
{
	if (imageFile.empty() || ((downscaleFactor.x < 2) && (downscaleFactor.y < 2)))
		return nullptr;

	auto filePath = LSG_Text::GetFullPath(imageFile);
	auto surface  = IMG_Load(filePath.c_str());

	if (!surface)
		throw std::runtime_error(std::format("Failed to load image '{}': {}", filePath, SDL_GetError()));

	return LSG_Graphics::getDownScaledSurface(surface, downscaleFactor);
}

SDL_Surface* LSG_Graphics::getDownScaledSurface(SDL_Surface* surface, const SDL_Point& downscaleFactor)
{
	if (!surface)
		return nullptr;

	// https://github.com/sabdul-khabir/SDL3_gfx/blob/master/SDL3_rotozoom.c#L87

	auto srcSurface = surface;

	if (surface->format->BitsPerPixel != 32) {
		srcSurface = SDL_ConvertSurfaceFormat(surface, SDL_PIXELFORMAT_RGBA32, 0);
		SDL_FreeSurface(surface);
	}

	if (SDL_MUSTLOCK(srcSurface))
		SDL_LockSurface(srcSurface);

	auto bits   = srcSurface->format->BitsPerPixel;
	auto bytes  = srcSurface->format->BytesPerPixel;
	auto format = srcSurface->format->format;

	SDL_Size destSize = {
		(srcSurface->w / downscaleFactor.x),
		(srcSurface->h / downscaleFactor.y)
	};

	auto destSurface = SDL_CreateRGBSurfaceWithFormat(0, destSize.width, (destSize.height + 2), bits, format);

	destSurface->h = destSize.height;

	auto divisionFactor    = (downscaleFactor.x * downscaleFactor.y);
	auto divisionFactorRGB = (downscaleFactor.x * bytes);

	auto destGap = (destSurface->pitch - (destSurface->w * bytes));

	auto srcPixel  = (SDL_Color*)srcSurface->pixels;
	auto destPixel = (SDL_Color*)destSurface->pixels;

	for (int y = 0; y < destSurface->h; y++)
	{
		auto srcPixelY = srcPixel;

		for (int x = 0; x < destSurface->w; x++)
		{
			auto srcPixelX = srcPixel;

			int r = 0, g = 0, b = 0, a = 0;

			for (int i = 0; i < downscaleFactor.y; i++)
			{
				for (int j = 0; j < downscaleFactor.x; j++)
				{
					r += srcPixel->r;
					g += srcPixel->g;
					b += srcPixel->b;
					a += srcPixel->a;

					srcPixel++;
				}

				srcPixel = (SDL_Color*)((uint8_t*)srcPixel + (srcSurface->pitch - divisionFactorRGB));
			}

			srcPixel = (SDL_Color*)((uint8_t*)srcPixelX + divisionFactorRGB);

			destPixel->r = (r / divisionFactor);
			destPixel->g = (g / divisionFactor);
			destPixel->b = (b / divisionFactor);
			destPixel->a = (a / divisionFactor);

			destPixel++;
		}

		srcPixel  = (SDL_Color*)((uint8_t*)srcPixelY + (srcSurface->pitch * downscaleFactor.y));
		destPixel = (SDL_Color*)((uint8_t*)destPixel + destGap);
	}

	if (SDL_MUSTLOCK(srcSurface))
		SDL_UnlockSurface(srcSurface);

	SDL_FreeSurface(srcSurface);

	return destSurface;
}

SDL_Texture* LSG_Graphics::GetDownScaledTexture(const std::string& imageFile, const SDL_Point& downscaleFactor)
{
	auto surface = LSG_Graphics::getDownScaledSurface(imageFile, downscaleFactor);

	if (!surface)
		return nullptr;

	auto texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);

	return texture;
}

SDL_Rect LSG_Graphics::GetFillArea(const SDL_Rect& background, int borderWidth, int padding)
{
	SDL_Rect fillArea = background;

	auto border2x  = (borderWidth + borderWidth);
	auto padding2x = (padding + padding);

	fillArea.x += (borderWidth + padding);
	fillArea.y += (borderWidth + padding);
	fillArea.w -= (border2x + padding2x);
	fillArea.h -= (border2x + padding2x);

	return fillArea;
}

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
		LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding + 2),
		LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding),
		color,
		orientation
	);
}

std::vector<SDL_Vertex> LSG_Graphics::GetGeometryTriangleVertical(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation)
{
	return LSG_Graphics::getGeometryTriangle(
		background,
		LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding),
		LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding + 2),
		color,
		orientation
	);
}

std::vector<SDL_Vertex> LSG_Graphics::getGeometryTriangle(const SDL_Rect& background, int paddingX, int paddingY, const SDL_Color& color, LSG_TriangleOrientation orientation)
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

SDL_Color LSG_Graphics::GetInverseColor(const SDL_Color& color)
{
	const int MAX = 255;

	SDL_Color inverseColor = {
		(uint8_t)(MAX - color.r),
		(uint8_t)(MAX - color.g),
		(uint8_t)(MAX - color.b),
		color.a
	};

	return inverseColor;
}

SDL_Color LSG_Graphics::GetOffsetColor(const SDL_Color& color, int offset)
{
	const int DEFAULT = (255 - offset);
	const int MAX     = (DEFAULT - offset);

	SDL_Color offsetColor = {
		(uint8_t)(color.r < MAX ? (color.r + offset) : DEFAULT),
		(uint8_t)(color.g < MAX ? (color.g + offset) : DEFAULT),
		(uint8_t)(color.b < MAX ? (color.b + offset) : DEFAULT),
		color.a
	};

	return offsetColor;
}

float LSG_Graphics::getOpacity(const SDL_Color& color)
{
	return (color.a < 255 ? ((float)color.a / 255.0f) : 1.0f);
}

std::string LSG_Graphics::getPathCornerBottomLeft(int radius, const SDL_Size& size)
{
	auto bottom = (size.height - radius);

	// A rx ry x-axis-rotation large-arc-flag sweep-flag x y
	auto svg = std::format(
		"<path d='M 0  {} v {} h {} A {} {} 0 0 1 0  {}' />",
		bottom, radius, radius, radius, radius, bottom
	);

	return svg;
}

std::string LSG_Graphics::getPathCornerBottomRight(int radius, const SDL_Size& size)
{
	auto bottom = (size.height - radius);

	// A rx ry x-axis-rotation large-arc-flag sweep-flag x y
	auto svg = std::format(
		"<path d='M {} {} v {} h {} A {} {} 0 0 0 {} {}' />",
		size.width, bottom, radius, -radius, radius, radius, size.width, bottom
	);

	return svg;
}

std::string LSG_Graphics::getPathCornerTopLeft(int radius, const SDL_Size& size)
{
	// A rx ry x-axis-rotation large-arc-flag sweep-flag x y
	auto svg = std::format(
		"<path d='M 0  {} V 0  H {} A {} {} 0 0 0 0  {}' />",
		radius, radius, radius, radius, radius
	);

	return svg;
}

std::string LSG_Graphics::getPathCornerTopRight(int radius, const SDL_Size& size)
{
	// A rx ry x-axis-rotation large-arc-flag sweep-flag x y
	auto svg = std::format(
		"<path d='M {} {} V 0  h {} A {} {} 0 0 1 {} {}' />",
		size.width, radius, -radius, radius, radius, size.width, radius
	);

	return svg;
}

SDL_Size LSG_Graphics::GetTextureSize(SDL_Texture* texture)
{
	if (!texture)
		return {};

	SDL_Size textureSize = {};
	SDL_QueryTexture(texture, nullptr, nullptr, &textureSize.width, &textureSize.height);

	return textureSize;
}

SDL_Surface* LSG_Graphics::GetThumbnail(const std::string& imageFile, const SDL_Size& maxSize)
{
	if (imageFile.empty())
		return nullptr;

	auto filePath = LSG_Text::GetFullPath(imageFile);
	auto surface  = IMG_Load(filePath.c_str());

	if (!surface)
		return nullptr;

	auto downscaleFactor = LSG_Graphics::GetDownscaleFactor({ surface->w, surface->h }, maxSize);

	if ((downscaleFactor.x > 1) || (downscaleFactor.y > 1))
	{
		SDL_FreeSurface(surface);

		surface = LSG_Graphics::getDownScaledSurface(imageFile, downscaleFactor);
	}

	return surface;
}

SDL_Surface* LSG_Graphics::GetThumbnail(SDL_Surface* surface, const SDL_Size& maxSize)
{
	if (!surface)
		return nullptr;

	auto downscaleFactor = LSG_Graphics::GetDownscaleFactor({ surface->w, surface->h }, maxSize);

	if ((downscaleFactor.x > 1) || (downscaleFactor.y > 1))
		surface = LSG_Graphics::getDownScaledSurface(surface, downscaleFactor);

	return surface;
}

SDL_Color LSG_Graphics::GetThumbColor(const SDL_Color& backgroundColor)
{
	uint8_t MIN = 50;
	uint8_t MAX = 200;

	auto inverseColor = LSG_Graphics::GetInverseColor(backgroundColor);

	SDL_Color thumbColor = {
		(uint8_t)std::max(MIN, std::min(MAX, inverseColor.r)),
		(uint8_t)std::max(MIN, std::min(MAX, inverseColor.g)),
		(uint8_t)std::max(MIN, std::min(MAX, inverseColor.b)),
		255
	};

	return thumbColor;
}

SDL_Texture* LSG_Graphics::GetVector(LSG_Vector vector, const SDL_Color& color, const SDL_Size& size)
{
	std::string svg = "";

	switch (vector) {
		case LSG_VECTOR_BACK:       svg = LSG_Graphics::getVectorBack(color,  size); break;
		case LSG_VECTOR_CHECK:      svg = LSG_Graphics::getVectorCheck(color, size); break;
		case LSG_VECTOR_CLOSE:      svg = LSG_Graphics::getVectorClose(color, size); break;
		case LSG_VECTOR_MENU:       svg = LSG_Graphics::getVectorMenu(color,  size); break;
		case LSG_VECTOR_NEXT:       svg = LSG_Graphics::getVectorNext(color,  size); break;
		case LSG_VECTOR_PAGE_BACK:  svg = LSG_Graphics::getVectorPageBack(color,  size); break;
		case LSG_VECTOR_PAGE_END:   svg = LSG_Graphics::getVectorPageEnd(color,   size); break;
		case LSG_VECTOR_PAGE_NEXT:  svg = LSG_Graphics::getVectorPageNext(color,  size); break;
		case LSG_VECTOR_PAGE_START: svg = LSG_Graphics::getVectorPageStart(color, size); break;
		case LSG_VECTOR_TOGGLE_ON:  svg = LSG_Graphics::getVectorToggleOn(color,  size); break;
		case LSG_VECTOR_TOGGLE_OFF: svg = LSG_Graphics::getVectorToggleOff(color, size); break;
		default: break;
	}

	return LSG_Graphics::getVector(svg);
}

SDL_Texture* LSG_Graphics::getVector(const std::string& svg)
{
	if (svg.empty())
		return nullptr;

	auto svgRW   = SDL_RWFromConstMem(svg.c_str(), (int)svg.size());
	auto surface = IMG_LoadSVG_RW(svgRW);

	if (!surface)
		return nullptr;

	auto texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);

	return texture;
}

std::string LSG_Graphics::getVectorBack(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 24 24' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; stroke-linecap: round; stroke-linejoin: round'>" \
		"<polygon points='17.2,23.7 5.4,12 17.2,0.3 18.5,1.7 8.4,12 18.5,22.3' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorCheck(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 24 24' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}'>" \
		"<path d='M5.341,12.247a1,1,0,0,0,1.317,1.505l4-3.5a1,1,0,0,0,.028-1.48l-9-8.5A1,1,0,0,0,.313,1.727l8.2,7.745Z' transform='translate(19 6.5) rotate(90)'></path>" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorClose(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 24 24' width='{}px' height='{}px' style='stroke: rgb({},{},{}); stroke-opacity: {}; stroke-width: 2; stroke-linecap: round; stroke-linejoin: round'>" \
		"<path d='M18 18L12 12M12 12L6 6M12 12L18 6M12 12L6 18' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorMenu(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 24 24' width='{}px' height='{}px' style='stroke: rgb({},{},{}); stroke-opacity: {}; stroke-width: 2; stroke-linecap: round; stroke-linejoin: round'>" \
		"<path d='M4 6H20M4 12H20M4 18H20' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorNext(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 24 24' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; stroke-linecap: round; stroke-linejoin: round'>" \
		"<polygon points='6.8, 23.7 5.4, 22.3 15.7, 12 5.4, 1.7 6.8, 0.3 18.5, 12' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorPageBack(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 192.701 192.701' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; stroke-linecap: round; stroke-linejoin: round'>" \
		"<path d='M29.641,96.345l74.54-75.61c4.704-4.74,4.704-12.439,0-17.179c-4.704-4.74-12.319-4.74-17.011,0l-82.997,84.2 c-4.511,4.559-4.535,12.608,0,17.191l83.009,84.2c4.692,4.74,12.319,4.74,17.011,0c4.704-4.74,4.704-12.439,0-17.179 L29.641,96.345z' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorPageEnd(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 192.689 192.689' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; stroke-linecap: round; stroke-linejoin: round'>" \
		"<path d='M188.527,87.755l-83.009-84.2c-4.692-4.74-12.319-4.74-17.011,0c-4.704,4.74-4.704,12.439,0,17.179l74.54,75.61 l-74.54,75.61c-4.704,4.74-4.704,12.439,0,17.179c4.704,4.74,12.319,4.74,17.011,0l82.997-84.2 C193.05,100.375,193.062,92.327,188.527,87.755z' />" \
		"<path d='M104.315,87.755l-82.997-84.2c-4.704-4.74-12.319-4.74-17.011,0c-4.704,4.74-4.704,12.439,0,17.179l74.528,75.61 l-74.54,75.61c-4.704,4.74-4.704,12.439,0,17.179s12.319,4.74,17.011,0l82.997-84.2C108.838,100.375,108.85,92.327,104.315,87.755 z' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorPageNext(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 192.689 192.689' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; stroke-linecap: round; stroke-linejoin: round'>" \
		"<path d='M104.315,87.755l-82.997-84.2c-4.704-4.74-12.319-4.74-17.011,0c-4.704,4.74-4.704,12.439,0,17.179l74.528,75.61 l-74.54,75.61c-4.704,4.74-4.704,12.439,0,17.179s12.319,4.74,17.011,0l82.997-84.2C108.838,100.375,108.85,92.327,104.315,87.755 z' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorPageStart(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 192.701 192.701' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; stroke-linecap: round; stroke-linejoin: round'>" \
		"<path d='M29.641,96.345l74.54-75.61c4.704-4.74,4.704-12.439,0-17.179c-4.704-4.74-12.319-4.74-17.011,0l-82.997,84.2 c-4.511,4.559-4.535,12.608,0,17.191l83.009,84.2c4.692,4.74,12.319,4.74,17.011,0c4.704-4.74,4.704-12.439,0-17.179 L29.641,96.345z' />" \
		"<path d='M113.853,96.345l74.54-75.61c4.704-4.74,4.704-12.439,0-17.179c-4.704-4.74-12.319-4.74-17.011,0l-82.997,84.2 c-4.511,4.559-4.535,12.608,0,17.191l82.997,84.2c4.704,4.74,12.319,4.74,17.011,0c4.704-4.74,4.704-12.439,0-17.179 L113.853,96.345z' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

SDL_Texture* LSG_Graphics::getVectorRoundedCorners(const SDL_Color& backgroundColor, int borderRadius, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}'>{} {} {} {}</svg>",
		size.width, size.height,
		backgroundColor.r, backgroundColor.g, backgroundColor.b, LSG_Graphics::getOpacity(backgroundColor),
		LSG_Graphics::getPathCornerTopLeft(borderRadius,     size),
		LSG_Graphics::getPathCornerTopRight(borderRadius,    size),
		LSG_Graphics::getPathCornerBottomRight(borderRadius, size),
		LSG_Graphics::getPathCornerBottomLeft(borderRadius,  size)
	);

	return LSG_Graphics::getVector(svg);
}

SDL_Texture* LSG_Graphics::getVectorRoundedCornersBottom(const SDL_Color& backgroundColor, int borderRadius, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}'>{} {}</svg>",
		size.width, size.height,
		backgroundColor.r, backgroundColor.g, backgroundColor.b, LSG_Graphics::getOpacity(backgroundColor),
		LSG_Graphics::getPathCornerBottomRight(borderRadius, size),
		LSG_Graphics::getPathCornerBottomLeft(borderRadius,  size)
	);

	return LSG_Graphics::getVector(svg);
}

SDL_Texture* LSG_Graphics::getVectorRoundedCornersTop(const SDL_Color& backgroundColor, int borderRadius, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}'>{} {}</svg>",
		size.width, size.height,
		backgroundColor.r, backgroundColor.g, backgroundColor.b, LSG_Graphics::getOpacity(backgroundColor),
		LSG_Graphics::getPathCornerTopLeft(borderRadius,  size),
		LSG_Graphics::getPathCornerTopRight(borderRadius, size)
	);

	return LSG_Graphics::getVector(svg);
}

SDL_Texture* LSG_Graphics::getVectorRoundedRectangleFill(const SDL_Color& color, int borderRadius, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}'>" \
		"<rect width='{}px' height='{}px' rx='{}' ry='{}' />" \
		"</svg>",
		size.width, size.height,
		color.r, color.g, color.b, LSG_Graphics::getOpacity(color),
		size.width, size.height,
		borderRadius, borderRadius
	);

	return LSG_Graphics::getVector(svg);
}

SDL_Texture* LSG_Graphics::getVectorRoundedRectangleWithBorder(const SDL_Color& fillColor, const SDL_Color& borderColor, int borderRadius, int borderWidth, const SDL_Size& size)
{
	auto borderx = (borderWidth + borderWidth);

	auto svg = std::format(
		"<svg width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; stroke: rgb({},{},{}); stroke-opacity: {}; stroke-width: {}'>" \
		"<rect x='{}' y='{}' width='{}px' height='{}px' rx='{}' ry='{}' />" \
		"</svg>",
		(size.width + borderx), (size.height + borderx),
		fillColor.r, fillColor.g, fillColor.b, LSG_Graphics::getOpacity(fillColor),
		borderColor.r, borderColor.g, borderColor.b, LSG_Graphics::getOpacity(borderColor),
		borderWidth,
		borderWidth, borderWidth,
		size.width, size.height,
		borderRadius, borderRadius
	);

	return LSG_Graphics::getVector(svg);
}

std::string LSG_Graphics::getVectorToggleOff(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 64 64' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; fill-rule: evenodd; clip-rule: evenodd; stroke-linejoin: round; stroke-miterlimit: 2'>" \
		"<path style='fill-rule:nonzero;' d='M41.309,17.112c9.416,0.179 17.545,10.374 13.732,20.395c-2.105,5.532 -7.689,9.487 -13.732,9.602c-6.201,0.04 -12.402,0.04 -18.603,0c-9.653,-0.183 -17.944,-11.153 -13.383,-21.233c2.32,-5.128 7.685,-8.656 13.383,-8.764c6.201,-0.04 12.402,-0.04 18.603,0Zm-18.398,3.998c-7.151,0.046 -13.348,8.061 -9.944,15.586c1.694,3.744 5.614,6.334 9.789,6.413c6.168,0.039 12.335,0.039 18.503,0c6.934,-0.131 12.825,-7.661 10.043,-14.973c-1.54,-4.049 -5.615,-6.941 -10.044,-7.025c-6.115,-0.039 -12.231,-0.001 -18.347,-0.001Z' />" \
		"<circle cx='41.117' cy='32.11' r='10.015' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

std::string LSG_Graphics::getVectorToggleOn(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = std::format(
		"<svg viewBox='0 0 64 64' width='{}px' height='{}px' style='fill: rgb({},{},{}); fill-opacity: {}; fill-rule: evenodd; clip-rule: evenodd; stroke-linejoin: round; stroke-miterlimit: 2'>" \
		"<path style='fill-rule:nonzero;' d='M25.022,17.099c2.715,-0.012 12.015,0.058 13.952,0c22.08,-0.662 22.961,30.643 0,30.023c-3.488,0.015 -12.792,-0.064 -13.952,0c-10.359,0.572 -17.04,-6.822 -16.997,-15.272c0.042,-8.451 7.53,-15.72 16.997,-14.751Zm7.882,15.011c0.143,-5.363 -4.664,-10.096 -10.015,-10.015c-7.31,0.111 -10.482,6.7 -10.016,10.947c0.625,5.691 5.193,9.06 10.016,9.084c5.536,0.026 9.862,-4.308 10.015,-10.016Z' />" \
		"</svg>",
		size.width, size.height, color.r, color.g, color.b, LSG_Graphics::getOpacity(color)
	);

	return svg;
}

bool LSG_Graphics::IsColorEquals(const SDL_Color& a, const SDL_Color& b)
{
	return ((a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a));
}

void LSG_Graphics::RenderBorder(SDL_Renderer* renderer, int borderWidth, const SDL_Color& color, const SDL_Rect& background)
{
	if (borderWidth < 1)
		return;

	SDL_SetRenderDrawBlendMode(renderer, (color.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	SDL_Rect borderArea = SDL_Rect(background);

	// TOP
	borderArea.h = borderWidth;

	SDL_RenderFillRect(renderer, &borderArea);

	// BOTTOM
	borderArea.y += (background.h - borderWidth);

	SDL_RenderFillRect(renderer, &borderArea);

	// LEFT
	borderArea.y = background.y;
	borderArea.w = borderWidth;
	borderArea.h = background.h;

	SDL_RenderFillRect(renderer, &borderArea);

	// RIGHT
	borderArea.x += (background.w - borderWidth);

	SDL_RenderFillRect(renderer, &borderArea);
}

void LSG_Graphics::RenderFill(SDL_Renderer* renderer, int borderWidth, const SDL_Color& color, const SDL_Rect& background)
{
	auto fillArea = LSG_Graphics::GetFillArea(background, borderWidth);

	SDL_SetRenderDrawBlendMode(renderer, (color.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	SDL_RenderFillRect(renderer, &fillArea);
}

void LSG_Graphics::RenderFillRounded(
	SDL_Renderer*      renderer,
	int                borderRadius,
	const SDL_Color&   color,
	const SDL_Rect&    background,
	const std::string& id
) {
	if (!LSG_Graphics::textures.contains(id))
		LSG_Graphics::textures[id] = LSG_Graphics::getVectorRoundedRectangleFill(color, borderRadius, { background.w, background.h });

	SDL_RenderCopy(renderer, LSG_Graphics::textures[id], nullptr, &background);
}

void LSG_Graphics::RenderFillWithRoundedBorder(
	SDL_Renderer*      renderer,
	const SDL_Color&   fillColor,
	const SDL_Color&   borderColor,
	int                borderRadius,
	int                borderWidth,
	const SDL_Rect&    background,
	const std::string& id
) {
	if (!LSG_Graphics::textures.contains(id))
	{
		LSG_Graphics::textures[id] = LSG_Graphics::getVectorRoundedRectangleWithBorder(
			fillColor,
			borderColor,
			borderRadius,
			borderWidth,
			{ background.w, background.h }
		);
	}

	SDL_RenderCopy(renderer, LSG_Graphics::textures[id], nullptr, &background);
}

void LSG_Graphics::RenderLine(SDL_Renderer* renderer, const SDL_Color& color, int x1, int y1, int x2, int y2)
{
	SDL_SetRenderDrawBlendMode(renderer, (color.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}

void LSG_Graphics::RenderRoundedCorners(
	SDL_Renderer*      renderer,
	const SDL_Color&   fillColor,
	int                borderRadius,
	const SDL_Rect&    background,
	const std::string& id
) {
	if (!LSG_Graphics::textures.contains(id))
		LSG_Graphics::textures[id] = LSG_Graphics::getVectorRoundedCorners(fillColor, borderRadius, { background.w, background.h });

	SDL_RenderCopy(renderer, LSG_Graphics::textures[id], nullptr, &background);
}

void LSG_Graphics::RenderRoundedCornersBottom(
	SDL_Renderer*      renderer,
	const SDL_Color&   fillColor,
	int                borderRadius,
	const SDL_Rect&    background,
	const std::string& id
) {
	if (!LSG_Graphics::textures.contains(id))
		LSG_Graphics::textures[id] = LSG_Graphics::getVectorRoundedCornersBottom(fillColor, borderRadius, { background.w, background.h });

	SDL_RenderCopy(renderer, LSG_Graphics::textures[id], nullptr, &background);
}

void LSG_Graphics::RenderRoundedCornersTop(
	SDL_Renderer*      renderer,
	const SDL_Color&   fillColor,
	int                borderRadius,
	const SDL_Rect&    background,
	const std::string& id
) {
	if (!LSG_Graphics::textures.contains(id))
		LSG_Graphics::textures[id] = LSG_Graphics::getVectorRoundedCornersTop(fillColor, borderRadius, { background.w, background.h });

	SDL_RenderCopy(renderer, LSG_Graphics::textures[id], nullptr, &background);
}

void LSG_Graphics::RenderTexture(
	SDL_Renderer*        renderer,
	const SDL_Rect&      background,
	const LSG_Alignment& alignment,
	SDL_Texture*         texture,
	const SDL_Size&      size
) {
	SDL_Rect clip = {
		0,
		0,
		std::min(size.width,  background.w),
		std::min(size.height, background.h)
	};

	auto destination = LSG_Graphics::GetDestinationAligned(background, size, alignment);

	SDL_RenderCopy(renderer, texture, &clip, &destination);
}

void LSG_Graphics::RenderTextureWithRoundedCorners(
	SDL_Renderer*      renderer,
	SDL_Texture*       texture,
	const SDL_Rect&    destination,
	const SDL_Rect*    clip,
	int                radius,
	const SDL_Color&   backgroundColor,
	const std::string& id
) {
	SDL_RenderCopy(renderer, texture, clip, &destination);

	if (radius < 1)
		return;

	LSG_Graphics::RenderRoundedCorners(
		renderer,
		backgroundColor,
		radius,
		destination,
		std::format("{}_texture_with_rounded_corners", id)
	);
}

void LSG_Graphics::Rotate(LSG_ItemImage& image)
{
	auto exif        = LSG_Exif::Get(LSG_Text::GetFullPath(image.filePath));
	auto orientation = LSG_Exif::GetOrientation(exif.tags);

	if (orientation.rotation > 0.0)
	{
		auto maxSize = std::max(image.surface->w, image.surface->h);

		image.texture.size    = { maxSize, maxSize };
		image.texture.texture = LSG_Window::RotateTexture(image.texture.texture, orientation, image.texture.size, image.surface->format->format);
	}
}

/**
* Valid color strings:
* "#00FF00"           0 red, 255 green, 0 blue
* "#00FF0080"         0 red, 255 green, 0 blue, 128 alpha
* "rgb(0,0,255)"      0 red, 255 green, 0 blue
* "rgba(0,0,255,0.5)" 0 red, 255 green, 0 blue, 128 alpha
*/
SDL_Color LSG_Graphics::ToSdlColor(const std::string &color)
{
	if (color.empty())
		return {};

	SDL_Color sdlColor = { 0, 0, 0, 255 };

	// HEX: "#00FF0080" / "#00FF00"
	if ((color[0] == '#') && (color.size() >= 7))
	{
		sdlColor.r = (uint8_t)std::strtoul(std::format("0x{}", color.substr(1, 2)).c_str(), nullptr, 16);
		sdlColor.g = (uint8_t)std::strtoul(std::format("0x{}", color.substr(3, 2)).c_str(), nullptr, 16);
		sdlColor.b = (uint8_t)std::strtoul(std::format("0x{}", color.substr(5, 2)).c_str(), nullptr, 16);

		if (color.size() == 9)
			sdlColor.a = (uint8_t)std::strtoul(std::format("0x{}", color.substr(7, 2)).c_str(), nullptr, 16);
	}
	else if (color.substr(0, 4) == "rgb(")
	{
		auto values = color.substr(4, color.size() - 5);

		sdlColor.r = (uint8_t)std::atoi(std::strtok((char*)values.c_str(), ","));
		sdlColor.g = (uint8_t)std::atoi(std::strtok(nullptr, ","));
		sdlColor.b = (uint8_t)std::atoi(std::strtok(nullptr, ","));
	}
	else if (color.substr(0, 5) == "rgba(")
	{
		auto values = color.substr(5, color.size() - 6);

		sdlColor.r = (uint8_t)std::atoi(std::strtok((char*)values.c_str(), ","));
		sdlColor.g = (uint8_t)std::atoi(std::strtok(nullptr, ","));
		sdlColor.b = (uint8_t)std::atoi(std::strtok(nullptr, ","));
		sdlColor.a = (uint8_t)(256.0 * std::atof(std::strtok(nullptr, ",")));
	}

	return sdlColor;
}

std::string LSG_Graphics::ToXmlAttribute(const SDL_Color& color)
{
	return std::format("rgba({},{},{},{})", color.r, color.g, color.b, color.a);
}

void LSG_Graphics::UpdateTexture(SDL_Texture* texture, SDL_Surface* surface)
{
	if (!texture || !surface)
		return;

	SDL_UpdateTexture(texture, nullptr, surface->pixels, surface->pitch);

	if (!SDL_ISPIXELFORMAT_ALPHA(surface->format->format))
		return;

	SDL_BlendMode blendMode;
	SDL_GetSurfaceBlendMode(surface, &blendMode);

	SDL_SetTextureBlendMode(texture, blendMode);
}
