#include "LSG_ScrollBar.h"

LSG_ScrollBar::LSG_ScrollBar()
{
	this->clipFactorX              = 0.0;
	this->clipFactorY              = 0.0;
	this->isSlideActiveX           = false;
	this->isSlideActiveY           = false;
	this->scrollOffsetX            = 0;
	this->scrollOffsetY            = 0;
	this->scrollOffsetOnMouseDownX = 0;
	this->scrollOffsetOnMouseDownY = 0;
	this->scrollBarX               = {};
	this->scrollBarY               = {};
	this->scrollArrowDown          = {};
	this->scrollArrowLeft          = {};
	this->scrollArrowRight         = {};
	this->scrollArrowUp            = {};
	this->scrollThumbX             = {};
	this->scrollThumbY             = {};
	this->showScrollX              = false;
	this->showScrollY              = false;
}

SDL_Rect LSG_ScrollBar::getClipWithOffset(const SDL_Rect& clip, const SDL_Size& textureSize)
{
	SDL_Rect offsetClip = clip;

	if (this->showScrollX)
	{
		auto maxScrollOffsetX = (textureSize.width - offsetClip.w);

		if (this->scrollOffsetX > maxScrollOffsetX)
			this->scrollOffsetX = maxScrollOffsetX;

		offsetClip.x += this->scrollOffsetX;
	} else {
		this->scrollOffsetX = 0;
	}

	if (this->showScrollY)
	{
		auto maxScrollOffsetY = (textureSize.height - offsetClip.h);

		if (this->scrollOffsetY > maxScrollOffsetY)
			this->scrollOffsetY = maxScrollOffsetY;

		offsetClip.y += this->scrollOffsetY;
	} else {
		this->scrollOffsetY = 0;
	}

	return offsetClip;
}

SDL_Rect LSG_ScrollBar::getScrollableBackground(const SDL_Rect& background, const SDL_Size& textureSize, int scrollBarSize)
{
	SDL_Rect backgroundArea = background;

	bool showScrollY = (textureSize.height > (background.h + scrollBarSize));
	bool showScrollX = (textureSize.width  > (background.w + scrollBarSize));

	if (showScrollY)
		backgroundArea.w -= scrollBarSize;

	if (showScrollX)
		backgroundArea.h -= scrollBarSize;

	this->showScrollY = (textureSize.height > backgroundArea.h);
	this->showScrollX = (textureSize.width  > backgroundArea.w);

	if (this->showScrollY && !showScrollY)
		backgroundArea.w -= scrollBarSize;

	if (this->showScrollX && !showScrollX)
		backgroundArea.h -= scrollBarSize;

	return backgroundArea;
}

SDL_Rect LSG_ScrollBar::getScrollableClip(const SDL_Rect& background, const SDL_Size& textureSize)
{
	SDL_Rect clip = {
		0,
		0,
		std::min(textureSize.width,  background.w),
		std::min(textureSize.height, background.h)
	};

	return this->getClipWithOffset(clip, textureSize);
}

int LSG_ScrollBar::GetScrollX() const
{
	return this->scrollOffsetX;
}

int LSG_ScrollBar::GetScrollY() const
{
	return this->scrollOffsetY;
}

int LSG_ScrollBar::GetSize()
{
	return LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Size);
}

int LSG_ScrollBar::GetSize2x()
{
	return LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Size2x);
}

bool LSG_ScrollBar::OnScrollHome()
{
	if (!this->showScrollX && !this->showScrollY)
		return false;

	this->scrollOffsetY = 0;
	this->scrollOffsetX = 0;

	return true;
}

bool LSG_ScrollBar::OnScrollEnd()
{
	if (!this->showScrollX && !this->showScrollY)
		return false;

	this->scrollOffsetY = LSG_ConstTexture::MaxSize;
	this->scrollOffsetX = 0;

	return true;
}

bool LSG_ScrollBar::OnScrollMouseClick(const SDL_Point& mousePosition)
{
	if (LSG_Events::IsMouseDown())
		return false;

	auto     scrollBarSize = LSG_ScrollBar::GetSize();
	SDL_Rect scrollBarY    = this->scrollBarY;

	if (this->showScrollY && this->showScrollX)
		scrollBarY.h += scrollBarSize;

	if (!this->isSlideActiveY && this->showScrollY && SDL_PointInRect(&mousePosition, &scrollBarY))
	{
		if (mousePosition.y > (this->scrollBarY.y + this->scrollBarY.h))
			return true;

		this->scrollOffsetOnMouseDownY = this->scrollOffsetY;
		this->isSlideActiveY           = false;

		if (SDL_PointInRect(&mousePosition, &this->scrollArrowUp))
			this->OnScrollVertical(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowDown))
			this->OnScrollVertical(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollThumbY))
			return true;

		if (mousePosition.y < this->scrollThumbY.y)
			this->OnScrollVertical(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.y > (this->scrollThumbY.y + this->scrollThumbY.h))
			this->OnScrollVertical(LSG_ScrollBar::UnitPage);
		else
			this->isSlideActiveY = true;

		return true;
	}
	else if (!this->isSlideActiveX && this->showScrollX && SDL_PointInRect(&mousePosition, &this->scrollBarX))
	{
		this->scrollOffsetOnMouseDownX = this->scrollOffsetX;
		this->isSlideActiveX           = false;

		if (SDL_PointInRect(&mousePosition, &this->scrollArrowLeft))
			this->OnScrollHorizontal(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowRight))
			this->OnScrollHorizontal(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollThumbX))
			return true;

		if (mousePosition.x < this->scrollThumbX.x)
			this->OnScrollHorizontal(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.x > (this->scrollThumbX.x + this->scrollThumbX.w))
			this->OnScrollHorizontal(LSG_ScrollBar::UnitPage);
		else
			this->isSlideActiveX = true;

		return true;
	}

	return false;
}

bool LSG_ScrollBar::OnScrollMouseDown(const SDL_Point& mousePosition)
{
	if (!LSG_Events::IsMouseDown())
		return false;

	if (!this->isSlideActiveY && this->showScrollY && SDL_PointInRect(&mousePosition, &this->scrollBarY))
	{
		if (SDL_PointInRect(&mousePosition, &this->scrollArrowUp))
			return this->OnScrollVertical(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowDown))
			return this->OnScrollVertical(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollThumbY))
			return false;

		if (mousePosition.y < this->scrollThumbY.y)
			return this->OnScrollVertical(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.y > (this->scrollThumbY.y + this->scrollThumbY.h))
			return this->OnScrollVertical(LSG_ScrollBar::UnitPage);
	}
	else if (!this->isSlideActiveX && this->showScrollX && SDL_PointInRect(&mousePosition, &this->scrollBarX))
	{
		if (SDL_PointInRect(&mousePosition, &this->scrollArrowLeft))
			return this->OnScrollHorizontal(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowRight))
			return this->OnScrollHorizontal(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollThumbX))
			return false;

		if (mousePosition.x < this->scrollThumbX.x)
			return this->OnScrollHorizontal(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.x > (this->scrollThumbX.x + this->scrollThumbX.w))
			return this->OnScrollHorizontal(LSG_ScrollBar::UnitPage);
	}

	return false;
}

bool LSG_ScrollBar::OnScrollMouseMove(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!LSG_Events::IsMouseDown())
		return false;

	if (this->isSlideActiveY && this->showScrollY)
		return this->onScrollSlideVertical(mousePosition, lastEventPosition);
	else if (this->isSlideActiveX && this->showScrollX)
		return this->onScrollSlideHorizontal(mousePosition, lastEventPosition);

	return false;
}

void LSG_ScrollBar::OnScrollMouseUp()
{
	this->isSlideActiveX = false;
	this->isSlideActiveY = false;
}

bool LSG_ScrollBar::OnScrollHorizontal(int offset)
{
	if (!this->showScrollX)
		return false;

	this->scrollOffsetX += offset;

	if (this->scrollOffsetX < 0)
		this->scrollOffsetX = 0;

	return true;
}

bool LSG_ScrollBar::OnScrollVertical(int offset)
{
	if (!this->showScrollY)
		return false;

	this->scrollOffsetY += offset;

	if (this->scrollOffsetY < 0)
		this->scrollOffsetY = 0;

	return true;
}

bool LSG_ScrollBar::onScrollSlideHorizontal(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!this->showScrollX)
		return false;

	auto clipFactor = (1 / this->clipFactorX);
	auto mouseDiff  = (mousePosition.x - lastEventPosition.x);
	auto offset     = (int)((double)mouseDiff * clipFactor);

	this->scrollOffsetX = (this->scrollOffsetOnMouseDownX + offset);

	if (this->scrollOffsetX < 0)
		this->scrollOffsetX = 0;

	return true;
}

bool LSG_ScrollBar::onScrollSlideVertical(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!this->showScrollY)
		return false;

	auto clipFactor = (1 / this->clipFactorY);
	auto mouseDiff  = (mousePosition.y - lastEventPosition.y);
	auto offset     = (int)((double)mouseDiff * clipFactor);

	this->scrollOffsetY = (this->scrollOffsetOnMouseDownY + offset);

	if (this->scrollOffsetY < 0)
		this->scrollOffsetY = 0;

	return true;
}

void LSG_ScrollBar::renderScrollableTexture(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, SDL_Texture* texture, const SDL_Size& size)
{
	auto scrollBarSize  = LSG_ScrollBar::GetSize();
	auto backgroundArea = this->getScrollableBackground(background, size, scrollBarSize);
	auto clip           = this->getScrollableClip(backgroundArea, size);
	auto destination    = LSG_Graphics::GetDestinationAligned(backgroundArea, size, alignment);

	SDL_RenderCopy(renderer, texture, &clip, &destination);
}

void LSG_ScrollBar::renderScrollableTextures(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, const std::vector<SDL_Texture*>& textures, const SDL_Size& size, int spacing)
{
	auto scrollBarSize  = LSG_ScrollBar::GetSize();
	auto backgroundArea = this->getScrollableBackground(background, size, scrollBarSize);
	auto clip           = this->getScrollableClip(backgroundArea, size);
	auto destination    = LSG_Graphics::GetDestinationAligned(backgroundArea, size, alignment);
	auto offsetX        = this->scrollOffsetX;
	auto remainingWidth = backgroundArea.w;

	for (auto texture : textures)
	{
		auto size = LSG_Graphics::GetTextureSize(texture);

		clip.x = std::max(0, offsetX);
		clip.w = std::max(0, std::min((size.width - clip.x), remainingWidth));

		destination.w = clip.w;
		destination.h = clip.h;

		SDL_RenderCopy(renderer, texture, &clip, &destination);

		auto width = (clip.w + spacing);

		destination.x  += width;
		remainingWidth -= width;
		offsetX        -= size.width;
	}
}

void LSG_ScrollBar::renderScrollBarHorizontal(SDL_Renderer* renderer, const SDL_Rect& background, int maxWidth, const SDL_Color& backgroundColor, bool highlighted)
{
	auto scrollBarSize   = LSG_ScrollBar::GetSize();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();

	this->scrollBarX       = {};
	this->scrollArrowLeft  = {};
	this->scrollArrowRight = {};
	this->scrollThumbX     = {};

	this->scrollBarX = background;

	this->scrollBarX.y += (background.h - scrollBarSize);
	this->scrollBarX.h  = scrollBarSize;
	
	if (this->showScrollY)
		this->scrollBarX.w -= scrollBarSize;

	if (this->scrollBarX.w < scrollBarSize2x)
		this->scrollBarX.w = scrollBarSize2x;

	auto fillColor = LSG_Graphics::GetFillColor(backgroundColor);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 255);

	SDL_RenderFillRect(renderer, &this->scrollBarX);

	auto thumbColor = (highlighted ? LSG_Graphics::GetThumbColor(backgroundColor) : LSG_ScrollBar::DefaultThumbColor);

	this->scrollArrowLeft   = SDL_Rect(this->scrollBarX);
	this->scrollArrowLeft.w = scrollBarSize;

	auto triangleLeft = LSG_Graphics::GetGeometryTriangleHorizontal(this->scrollArrowLeft, thumbColor, LSG_TRIANGLE_ORIENTATION_LEFT);

	SDL_RenderGeometry(renderer, nullptr, triangleLeft.data(), 3, nullptr, 0);

	this->scrollArrowRight    = SDL_Rect(this->scrollArrowLeft);
	this->scrollArrowRight.x += (this->scrollBarX.w - scrollBarSize);

	auto triangleRight = LSG_Graphics::GetGeometryTriangleHorizontal(this->scrollArrowRight, thumbColor, LSG_TRIANGLE_ORIENTATION_RIGHT);

	SDL_RenderGeometry(renderer, nullptr, triangleRight.data(), 3, nullptr, 0);

	this->scrollThumbX = SDL_Rect(this->scrollBarX);

	this->scrollThumbX.x += scrollBarSize;
	this->scrollThumbX.w -= scrollBarSize2x;
	this->scrollThumbX.y += LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding);
	this->scrollThumbX.h -= LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding2x);

	this->clipFactorX = (double)((double)this->scrollThumbX.w / (double)maxWidth);

	this->scrollThumbX.w  = (int)std::ceil((double)((double)this->scrollThumbX.w + (double)scrollBarSize2x) * this->clipFactorX);
	this->scrollThumbX.x += (int)((double)this->scrollOffsetX * this->clipFactorX);

	SDL_SetRenderDrawColor(renderer, thumbColor.r, thumbColor.g, thumbColor.b, thumbColor.a);
	SDL_RenderFillRect(renderer, &this->scrollThumbX);
}

void LSG_ScrollBar::renderScrollBarVertical(SDL_Renderer* renderer, const SDL_Rect& background, int maxHeight, const SDL_Color& backgroundColor, bool highlighted)
{
	auto scrollBarSize   = LSG_ScrollBar::GetSize();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();

	this->scrollBarY      = {};
	this->scrollArrowUp   = {};
	this->scrollArrowDown = {};
	this->scrollThumbY    = {};

	this->scrollBarY = background;

	this->scrollBarY.x += (background.w - scrollBarSize);
	this->scrollBarY.w  = scrollBarSize;

	if (this->showScrollX)
		this->scrollBarY.h -= scrollBarSize;

	if (this->scrollBarY.h < scrollBarSize2x)
		this->scrollBarY.h = scrollBarSize2x;

	auto fillColor = LSG_Graphics::GetFillColor(backgroundColor);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 255);

	SDL_RenderFillRect(renderer, &this->scrollBarY);

	auto thumbColor = (highlighted ? LSG_Graphics::GetThumbColor(backgroundColor) : LSG_ScrollBar::DefaultThumbColor);

	this->scrollArrowUp   = SDL_Rect(this->scrollBarY);
	this->scrollArrowUp.h = scrollBarSize;

	auto triangleUp = LSG_Graphics::GetGeometryTriangleVertical(this->scrollArrowUp, thumbColor, LSG_TRIANGLE_ORIENTATION_UP);

	SDL_RenderGeometry(renderer, nullptr, triangleUp.data(), 3, nullptr, 0);

	this->scrollArrowDown    = SDL_Rect(this->scrollArrowUp);
	this->scrollArrowDown.y += (this->scrollBarY.h - scrollBarSize);

	auto triangleDown = LSG_Graphics::GetGeometryTriangleVertical(this->scrollArrowDown, thumbColor, LSG_TRIANGLE_ORIENTATION_DOWN);

	SDL_RenderGeometry(renderer, nullptr, triangleDown.data(), 3, nullptr, 0);

	this->scrollThumbY = SDL_Rect(this->scrollBarY);

	this->scrollThumbY.x += LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding);
	this->scrollThumbY.w -= LSG_Graphics::GetDPIScaled(LSG_ScrollBar::Padding2x);
	this->scrollThumbY.y += scrollBarSize;
	this->scrollThumbY.h -= scrollBarSize2x;

	this->clipFactorY = (double)((double)this->scrollThumbY.h / (double)maxHeight);

	this->scrollThumbY.h  = (int)std::ceil((double)((double)this->scrollThumbY.h + (double)scrollBarSize2x) * this->clipFactorY);
	this->scrollThumbY.y += (int)((double)this->scrollOffsetY * this->clipFactorY);

	SDL_SetRenderDrawColor(renderer, thumbColor.r, thumbColor.g, thumbColor.b, thumbColor.a);
	SDL_RenderFillRect(renderer, &this->scrollThumbY);
}
