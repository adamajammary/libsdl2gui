#include "LSG_Graphics.h"

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
		throw std::runtime_error(LSG_Text::Format("Failed to load image '%s': %s", filePath.c_str(), SDL_GetError()));

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

int LSG_Graphics::GetDPIScaled(int value)
{
	#if defined _android
		auto scale = (LSG_Window::GetDPI() / 160.0f);
	#else
		auto scale = LSG_Window::GetSizeScale().x;
	#endif

	return (int)((float)value * scale);
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
		LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding + 2),
		LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding),
		color,
		orientation
	);
}

std::vector<SDL_Vertex> LSG_Graphics::GetGeometryTriangleVertical(const SDL_Rect& background, const SDL_Color& color, LSG_TriangleOrientation orientation)
{
	return LSG_Graphics::getGeometryTriangle(
		background,
		LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding),
		LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding + 2),
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

SDL_Texture* LSG_Graphics::GetVectorBack(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 24 24' width='%dpx' height='%dpx' fill='rgb(%u,%u,%u)' stroke-linecap='round' stroke-linejoin='round'>" \
		"<polygon points='17.2,23.7 5.4,12 17.2,0.3 18.5,1.7 8.4,12 18.5,22.3' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

SDL_Texture* LSG_Graphics::GetVectorClose(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 24 24' width='%dpx' height='%dpx' stroke='rgb(%u,%u,%u)' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'>" \
		"<path d='M18 18L12 12M12 12L6 6M12 12L18 6M12 12L6 18' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

SDL_Texture* LSG_Graphics::GetVectorMenu(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 24 24' width='%dpx' height='%dpx' stroke='rgb(%u,%u,%u)' stroke-width='2' stroke-linecap='round' stroke-linejoin='round'>" \
		"<path d='M4 6H20M4 12H20M4 18H20' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

SDL_Texture* LSG_Graphics::GetVectorNext(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 24 24' width='%dpx' height='%dpx' fill='rgb(%u,%u,%u)' stroke-linecap='round' stroke-linejoin='round'>" \
		"<polygon points='6.8, 23.7 5.4, 22.3 15.7, 12 5.4, 1.7 6.8, 0.3 18.5, 12' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

SDL_Texture* LSG_Graphics::GetVectorPageBack(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 192.701 192.701' width='%dpx' height='%dpx' fill='rgb(%u,%u,%u)' stroke-linecap='round' stroke-linejoin='round'>" \
		"<path d='M29.641,96.345l74.54-75.61c4.704-4.74,4.704-12.439,0-17.179c-4.704-4.74-12.319-4.74-17.011,0l-82.997,84.2 c-4.511,4.559-4.535,12.608,0,17.191l83.009,84.2c4.692,4.74,12.319,4.74,17.011,0c4.704-4.74,4.704-12.439,0-17.179 L29.641,96.345z' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

SDL_Texture* LSG_Graphics::GetVectorPageEnd(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 192.689 192.689' width='%dpx' height='%dpx' fill='rgb(%u,%u,%u)' stroke-linecap='round' stroke-linejoin='round'>" \
		"<path d='M188.527,87.755l-83.009-84.2c-4.692-4.74-12.319-4.74-17.011,0c-4.704,4.74-4.704,12.439,0,17.179l74.54,75.61 l-74.54,75.61c-4.704,4.74-4.704,12.439,0,17.179c4.704,4.74,12.319,4.74,17.011,0l82.997-84.2 C193.05,100.375,193.062,92.327,188.527,87.755z' />" \
		"<path d='M104.315,87.755l-82.997-84.2c-4.704-4.74-12.319-4.74-17.011,0c-4.704,4.74-4.704,12.439,0,17.179l74.528,75.61 l-74.54,75.61c-4.704,4.74-4.704,12.439,0,17.179s12.319,4.74,17.011,0l82.997-84.2C108.838,100.375,108.85,92.327,104.315,87.755 z' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

SDL_Texture* LSG_Graphics::GetVectorPageNext(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 192.689 192.689' width='%dpx' height='%dpx' fill='rgb(%u,%u,%u)' stroke-linecap='round' stroke-linejoin='round'>" \
		"<path d='M104.315,87.755l-82.997-84.2c-4.704-4.74-12.319-4.74-17.011,0c-4.704,4.74-4.704,12.439,0,17.179l74.528,75.61 l-74.54,75.61c-4.704,4.74-4.704,12.439,0,17.179s12.319,4.74,17.011,0l82.997-84.2C108.838,100.375,108.85,92.327,104.315,87.755 z' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

SDL_Texture* LSG_Graphics::GetVectorPageStart(const SDL_Color& color, const SDL_Size& size)
{
	auto svg = "<svg viewBox='0 0 192.701 192.701' width='%dpx' height='%dpx' fill='rgb(%u,%u,%u)' stroke-linecap='round' stroke-linejoin='round'>" \
		"<path d='M29.641,96.345l74.54-75.61c4.704-4.74,4.704-12.439,0-17.179c-4.704-4.74-12.319-4.74-17.011,0l-82.997,84.2 c-4.511,4.559-4.535,12.608,0,17.191l83.009,84.2c4.692,4.74,12.319,4.74,17.011,0c4.704-4.74,4.704-12.439,0-17.179 L29.641,96.345z' />" \
		"<path d='M113.853,96.345l74.54-75.61c4.704-4.74,4.704-12.439,0-17.179c-4.704-4.74-12.319-4.74-17.011,0l-82.997,84.2 c-4.511,4.559-4.535,12.608,0,17.191l82.997,84.2c4.704,4.74,12.319,4.74,17.011,0c4.704-4.74,4.704-12.439,0-17.179 L113.853,96.345z' />" \
		"</svg>";

	return LSG_Graphics::getVector(LSG_Text::Format(svg, size.width, size.height, color.r, color.g, color.b));
}

bool LSG_Graphics::IsColorEquals(const SDL_Color& a, const SDL_Color& b)
{
	return ((a.r == b.r) && (a.g == b.g) && (a.b == b.b) && (a.a == b.a));
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
		sdlColor.r = (uint8_t)std::strtoul(LSG_Text::Format("0x%s", color.substr(1, 2).c_str()).c_str(), nullptr, 16);
		sdlColor.g = (uint8_t)std::strtoul(LSG_Text::Format("0x%s", color.substr(3, 2).c_str()).c_str(), nullptr, 16);
		sdlColor.b = (uint8_t)std::strtoul(LSG_Text::Format("0x%s", color.substr(5, 2).c_str()).c_str(), nullptr, 16);

		if (color.size() == 9)
			sdlColor.a = (uint8_t)std::strtoul(LSG_Text::Format("0x%s", color.substr(7, 2).c_str()).c_str(), nullptr, 16);
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
	return LSG_Text::Format("rgba(%u,%u,%u,%u)", color.r, color.g, color.b, color.a);
}
