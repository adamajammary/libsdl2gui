#include "LSG_ScrollBar.h"

LSG_ScrollBar::LSG_ScrollBar()
{
	this->scrollHorizontal = {};
	this->scrollVertical   = {};
}

SDL_Rect LSG_ScrollBar::getClipWithOffset(const SDL_Rect& clip, const SDL_Size& textureSize)
{
	SDL_Rect offsetClip = clip;

	if (this->scrollHorizontal.show)
	{
		auto maxScrollOffsetX = (textureSize.width - offsetClip.w);

		if (this->scrollHorizontal.offset > maxScrollOffsetX)
			this->scrollHorizontal.offset = maxScrollOffsetX;

		offsetClip.x += this->scrollHorizontal.offset;
	} else {
		this->scrollHorizontal.offset = 0;
	}

	if (this->scrollVertical.show)
	{
		auto maxScrollOffsetY = (textureSize.height - offsetClip.h);

		if (this->scrollVertical.offset > maxScrollOffsetY)
			this->scrollVertical.offset = maxScrollOffsetY;

		offsetClip.y += this->scrollVertical.offset;
	} else {
		this->scrollVertical.offset = 0;
	}

	return offsetClip;
}

SDL_Rect LSG_ScrollBar::getScrollableBackground(const SDL_Rect& fillArea, int borderWidth, const SDL_Size& textureSize)
{
	SDL_Rect backgroundArea = fillArea;
	auto     border2x       = (borderWidth + borderWidth);
	auto     scrollBarSize  = LSG_ScrollBar::GetSize();

	bool showScrollY = (textureSize.height > (backgroundArea.h + border2x + scrollBarSize));
	bool showScrollX = (textureSize.width  > (backgroundArea.w + border2x + scrollBarSize));

	if (showScrollY)
		backgroundArea.w -= scrollBarSize;

	if (showScrollX)
		backgroundArea.h -= scrollBarSize;

	this->scrollVertical.show   = (textureSize.height > (backgroundArea.h + border2x));
	this->scrollHorizontal.show = (textureSize.width  > (backgroundArea.w + border2x));

	if (this->scrollVertical.show && !showScrollY)
		backgroundArea.w -= scrollBarSize;

	if (this->scrollHorizontal.show && !showScrollX)
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

SDL_Rect LSG_ScrollBar::getScrollBarHorizontal(const SDL_Rect& background) const
{
	SDL_Rect bar  = background;
	auto     size = LSG_ScrollBar::GetSize();

	bar.y += (background.h - size);
	bar.h  = size;
	
	if (this->scrollVertical.show)
		bar.w -= size;

	auto minSize = (size * 3);

	if (bar.w < minSize)
		bar.w = minSize;

	return bar;
}

SDL_Rect LSG_ScrollBar::getScrollBarVertical(const SDL_Rect& background) const
{
	SDL_Rect bar  = background;
	auto     size = LSG_ScrollBar::GetSize();

	bar.x += (background.w - size);
	bar.w  = size;

	if (this->scrollHorizontal.show)
		bar.h -= size;

	auto minSize = (size * 3);

	if (bar.h < minSize)
		bar.h = minSize;

	return bar;
}

int LSG_ScrollBar::GetScrollHorizontal() const
{
	return this->scrollHorizontal.offset;
}

int LSG_ScrollBar::GetScrollVertical() const
{
	return this->scrollVertical.offset;
}

int LSG_ScrollBar::GetSize()
{
	return LSG_Window::GetDPIScaled(LSG_ScrollBar::Size);
}

int LSG_ScrollBar::GetSize2x()
{
	return LSG_Window::GetDPIScaled(LSG_ScrollBar::Size2x);
}

bool LSG_ScrollBar::IsMouseOverScrollbar(const SDL_Point& mousePosition) const
{
	auto scrollBarY = this->scrollVertical.bar;

	if (this->scrollVertical.show && this->scrollHorizontal.show)
		scrollBarY.h += LSG_ScrollBar::GetSize();

	if (this->scrollVertical.show && SDL_PointInRect(&mousePosition, &scrollBarY))
		return true;
	else if (this->scrollHorizontal.show && SDL_PointInRect(&mousePosition, &this->scrollHorizontal.bar))
		return true;

	return false;
}

bool LSG_ScrollBar::OnScrollHome()
{
	this->scrollVertical.offset   = 0;
	this->scrollHorizontal.offset = 0;

	return (this->scrollHorizontal.show || this->scrollVertical.show);
}

bool LSG_ScrollBar::OnScrollEnd()
{
	this->scrollVertical.offset   = LSG_ConstTexture::MaxSize;
	this->scrollHorizontal.offset = 0;

	return (this->scrollHorizontal.show || this->scrollVertical.show);
}

bool LSG_ScrollBar::OnScrollMouseClick(const SDL_Point& mousePosition)
{
	if (LSG_Events::IsMouseDown())
		return false;

	auto scrollBarY = this->scrollVertical.bar;

	if (this->scrollVertical.show && this->scrollHorizontal.show)
		scrollBarY.h += LSG_ScrollBar::GetSize();

	if (!this->scrollVertical.isSlideActive && this->scrollVertical.show && SDL_PointInRect(&mousePosition, &scrollBarY))
	{
		if (mousePosition.y > (this->scrollVertical.bar.y + this->scrollVertical.bar.h))
			return true;

		this->scrollVertical.offsetOnMouseDown = this->scrollVertical.offset;
		this->scrollVertical.isSlideActive     = false;

		if (SDL_PointInRect(&mousePosition, &this->scrollVertical.arrowBack))
			this->OnScrollVertical(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollVertical.arrowForward))
			this->OnScrollVertical(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollVertical.thumb))
			return true;

		if (mousePosition.y < this->scrollVertical.thumb.y)
			this->OnScrollVertical(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.y > (this->scrollVertical.thumb.y + this->scrollVertical.thumb.h))
			this->OnScrollVertical(LSG_ScrollBar::UnitPage);
		else
			this->scrollVertical.isSlideActive = true;

		return true;
	}
	else if (!this->scrollHorizontal.isSlideActive && this->scrollHorizontal.show && SDL_PointInRect(&mousePosition, &this->scrollHorizontal.bar))
	{
		this->scrollHorizontal.offsetOnMouseDown = this->scrollHorizontal.offset;
		this->scrollHorizontal.isSlideActive     = false;

		if (SDL_PointInRect(&mousePosition, &this->scrollHorizontal.arrowBack))
			this->OnScrollHorizontal(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollHorizontal.arrowForward))
			this->OnScrollHorizontal(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollHorizontal.thumb))
			return true;

		if (mousePosition.x < this->scrollHorizontal.thumb.x)
			this->OnScrollHorizontal(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.x > (this->scrollHorizontal.thumb.x + this->scrollHorizontal.thumb.w))
			this->OnScrollHorizontal(LSG_ScrollBar::UnitPage);
		else
			this->scrollHorizontal.isSlideActive = true;

		return true;
	}

	return false;
}

bool LSG_ScrollBar::OnScrollMouseDown(const SDL_Point& mousePosition)
{
	if (!LSG_Events::IsMouseDown())
		return false;

	if (!this->scrollVertical.isSlideActive && this->scrollVertical.show && SDL_PointInRect(&mousePosition, &this->scrollVertical.bar))
	{
		if (SDL_PointInRect(&mousePosition, &this->scrollVertical.arrowBack))
			return this->OnScrollVertical(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollVertical.arrowForward))
			return this->OnScrollVertical(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollVertical.thumb))
			return false;

		if (mousePosition.y < this->scrollVertical.thumb.y)
			return this->OnScrollVertical(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.y > (this->scrollVertical.thumb.y + this->scrollVertical.thumb.h))
			return this->OnScrollVertical(LSG_ScrollBar::UnitPage);
	}
	else if (!this->scrollHorizontal.isSlideActive && this->scrollHorizontal.show && SDL_PointInRect(&mousePosition, &this->scrollHorizontal.bar))
	{
		if (SDL_PointInRect(&mousePosition, &this->scrollHorizontal.arrowBack))
			return this->OnScrollHorizontal(-LSG_ScrollBar::Unit);
		else if (SDL_PointInRect(&mousePosition, &this->scrollHorizontal.arrowForward))
			return this->OnScrollHorizontal(LSG_ScrollBar::Unit);

		if (SDL_RectEmpty(&this->scrollHorizontal.thumb))
			return false;

		if (mousePosition.x < this->scrollHorizontal.thumb.x)
			return this->OnScrollHorizontal(-LSG_ScrollBar::UnitPage);
		else if (mousePosition.x > (this->scrollHorizontal.thumb.x + this->scrollHorizontal.thumb.w))
			return this->OnScrollHorizontal(LSG_ScrollBar::UnitPage);
	}

	return false;
}

bool LSG_ScrollBar::OnScrollMouseMove(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!LSG_Events::IsMouseDown())
		return false;

	if (this->scrollVertical.isSlideActive && this->scrollVertical.show)
		return this->onScrollSlideVertical(mousePosition, lastEventPosition);
	else if (this->scrollHorizontal.isSlideActive && this->scrollHorizontal.show)
		return this->onScrollSlideHorizontal(mousePosition, lastEventPosition);

	return false;
}

void LSG_ScrollBar::OnScrollMouseUp()
{
	this->scrollHorizontal.isSlideActive = false;
	this->scrollVertical.isSlideActive   = false;
}

bool LSG_ScrollBar::OnScrollHorizontal(int offset, bool forceScroll)
{
	if (!this->scrollHorizontal.show && !forceScroll)
		return false;

	this->scrollHorizontal.offset += offset;

	if (this->scrollHorizontal.offset < 0)
		this->scrollHorizontal.offset = 0;

	return true;
}

bool LSG_ScrollBar::OnScrollVertical(int offset, bool forceScroll)
{
	if (!this->scrollVertical.show && !forceScroll)
		return false;

	this->scrollVertical.offset += offset;

	if (this->scrollVertical.offset < 0)
		this->scrollVertical.offset = 0;

	return true;
}

bool LSG_ScrollBar::onScrollSlideHorizontal(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!this->scrollHorizontal.show)
		return false;

	auto clipFactor = (1 / this->scrollHorizontal.clipFactor);
	auto mouseDiff  = (mousePosition.x - lastEventPosition.x);
	auto offset     = (int)((double)mouseDiff * clipFactor);

	this->scrollHorizontal.offset = (this->scrollHorizontal.offsetOnMouseDown + offset);

	if (this->scrollHorizontal.offset < 0)
		this->scrollHorizontal.offset = 0;

	return true;
}

bool LSG_ScrollBar::onScrollSlideVertical(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!this->scrollVertical.show)
		return false;

	auto clipFactor = (1 / this->scrollVertical.clipFactor);
	auto mouseDiff  = (mousePosition.y - lastEventPosition.y);
	auto offset     = (int)((double)mouseDiff * clipFactor);

	this->scrollVertical.offset = (this->scrollVertical.offsetOnMouseDown + offset);

	if (this->scrollVertical.offset < 0)
		this->scrollVertical.offset = 0;

	return true;
}

void LSG_ScrollBar::renderScrollableTexture(
	SDL_Renderer*        renderer,
	const SDL_Rect&      fillArea,
	int                  borderWidth,
	const LSG_Alignment& alignment,
	SDL_Texture*         texture,
	const SDL_Size&      size
) {
	auto backgroundArea = this->getScrollableBackground(fillArea, borderWidth, size);
	auto clip           = this->getScrollableClip(backgroundArea, size);
	auto destination    = LSG_Graphics::GetDestinationAligned(backgroundArea, size, alignment);

	LSG_Graphics::RenderTexture(renderer, texture, &clip, &destination);
}

void LSG_ScrollBar::renderScrollBarHorizontal(
	SDL_Renderer*    renderer,
	const SDL_Rect&  background,
	int              maxWidth,
	const SDL_Color& backgroundColor,
	bool             highlighted,
	LSG_Component*   component
) {
	auto thumbColor = (highlighted ? LSG_Graphics::GetThumbColor(backgroundColor) : LSG_ScrollBar::DefaultThumbColor);

	this->scrollHorizontal.bar = this->getScrollBarHorizontal(background);

	this->renderScrollBar(renderer, this->scrollHorizontal.bar, backgroundColor);

	this->renderScrollArrowsHorizontal(renderer, thumbColor);

	this->renderScrollThumbHorizontal(renderer, thumbColor, maxWidth, component);
}

void LSG_ScrollBar::renderScrollBarVertical(
	SDL_Renderer*    renderer,
	const SDL_Rect&  background,
	int              maxHeight,
	const SDL_Color& backgroundColor,
	bool             highlighted,
	LSG_Component*   component
) {
	auto thumbColor = (highlighted ? LSG_Graphics::GetThumbColor(backgroundColor) : LSG_ScrollBar::DefaultThumbColor);

	this->scrollVertical.bar = this->getScrollBarVertical(background);

	this->renderScrollBar(renderer, this->scrollVertical.bar, backgroundColor);

	this->renderScrollArrowsVertical(renderer, thumbColor);

	this->renderScrollThumbVertical(renderer, thumbColor, maxHeight, component);
}

void LSG_ScrollBar::renderScrollArrowsHorizontal(SDL_Renderer* renderer, const SDL_Color& color)
{
	auto scrollBarSize = LSG_ScrollBar::GetSize();

	this->scrollHorizontal.arrowBack   = SDL_Rect(this->scrollHorizontal.bar);
	this->scrollHorizontal.arrowBack.w = scrollBarSize;

	auto triangleLeft = LSG_Graphics::GetGeometryTriangleHorizontal(this->scrollHorizontal.arrowBack, color, LSG_TRIANGLE_ORIENTATION_LEFT);

	SDL_RenderGeometry(renderer, nullptr, triangleLeft.data(), 3, nullptr, 0);

	this->scrollHorizontal.arrowForward    = SDL_Rect(this->scrollHorizontal.arrowBack);
	this->scrollHorizontal.arrowForward.x += (this->scrollHorizontal.bar.w - scrollBarSize);

	auto triangleRight = LSG_Graphics::GetGeometryTriangleHorizontal(this->scrollHorizontal.arrowForward, color, LSG_TRIANGLE_ORIENTATION_RIGHT);

	SDL_RenderGeometry(renderer, nullptr, triangleRight.data(), 3, nullptr, 0);
}

void LSG_ScrollBar::renderScrollArrowsVertical(SDL_Renderer* renderer, const SDL_Color& color)
{
	auto scrollBarSize = LSG_ScrollBar::GetSize();

	this->scrollVertical.arrowBack   = SDL_Rect(this->scrollVertical.bar);
	this->scrollVertical.arrowBack.h = scrollBarSize;

	auto triangleUp = LSG_Graphics::GetGeometryTriangleVertical(this->scrollVertical.arrowBack, color, LSG_TRIANGLE_ORIENTATION_UP);

	SDL_RenderGeometry(renderer, nullptr, triangleUp.data(), 3, nullptr, 0);

	this->scrollVertical.arrowForward    = SDL_Rect(this->scrollVertical.arrowBack);
	this->scrollVertical.arrowForward.y += (this->scrollVertical.bar.h - scrollBarSize);

	auto triangleDown = LSG_Graphics::GetGeometryTriangleVertical(this->scrollVertical.arrowForward, color, LSG_TRIANGLE_ORIENTATION_DOWN);

	SDL_RenderGeometry(renderer, nullptr, triangleDown.data(), 3, nullptr, 0);
}

void LSG_ScrollBar::renderScrollBar(SDL_Renderer* renderer, const SDL_Rect& bar, const SDL_Color& backgroundColor) const
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);

	LSG_Graphics::RenderFill(renderer, &bar);
}

void LSG_ScrollBar::renderScrollThumbHorizontal(SDL_Renderer* renderer, const SDL_Color& color, int maxWidth, LSG_Component* component)
{
	auto scrollBarSize   = LSG_ScrollBar::GetSize();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();

	this->scrollHorizontal.thumb = SDL_Rect(this->scrollHorizontal.bar);

	this->scrollHorizontal.thumb.x += scrollBarSize;
	this->scrollHorizontal.thumb.w -= scrollBarSize2x;
	this->scrollHorizontal.thumb.y += LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding);
	this->scrollHorizontal.thumb.h -= LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding2x);

	this->scrollHorizontal.clipFactor = (double)((double)this->scrollHorizontal.thumb.w / (double)maxWidth);

	auto sizeArrows = (scrollBarSize2x + (component->IsCards() && this->scrollVertical.show ? scrollBarSize : 0));

	this->scrollHorizontal.thumb.w  = (int)std::ceil((double)(this->scrollHorizontal.thumb.w + sizeArrows) * this->scrollHorizontal.clipFactor);
	this->scrollHorizontal.thumb.x += (int)std::ceil((double)this->scrollHorizontal.offset * this->scrollHorizontal.clipFactor);

	if (this->scrollHorizontal.thumb.w >= LSG_Window::GetDPIScaled(LSG_ScrollBar::ThumbRadius2x))
	{
		LSG_Graphics::RenderFillRounded(
			renderer,
			LSG_Window::GetDPIScaled(LSG_ScrollBar::ThumbRadius),
			color,
			this->scrollHorizontal.thumb,
			std::format("{}_scroll_thumb_horizontal", component->GetID())
		);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		LSG_Graphics::RenderFill(renderer, &this->scrollHorizontal.thumb);
	}
}

void LSG_ScrollBar::renderScrollThumbVertical(SDL_Renderer* renderer, const SDL_Color& color, int maxHeight, LSG_Component* component)
{
	auto scrollBarSize   = LSG_ScrollBar::GetSize();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();

	this->scrollVertical.thumb = SDL_Rect(this->scrollVertical.bar);

	this->scrollVertical.thumb.x += LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding);
	this->scrollVertical.thumb.w -= LSG_Window::GetDPIScaled(LSG_ScrollBar::Padding2x);
	this->scrollVertical.thumb.y += scrollBarSize;
	this->scrollVertical.thumb.h -= scrollBarSize2x;

	this->scrollVertical.clipFactor = (double)((double)this->scrollVertical.thumb.h / (double)maxHeight);

	auto sizeArrows = (scrollBarSize2x + (component->IsCards() && this->scrollHorizontal.show ? scrollBarSize : 0));

	this->scrollVertical.thumb.h  = (int)std::ceil((double)(this->scrollVertical.thumb.h + sizeArrows) * this->scrollVertical.clipFactor);
	this->scrollVertical.thumb.y += (int)std::ceil((double)this->scrollVertical.offset * this->scrollVertical.clipFactor);

	if (this->scrollVertical.thumb.h >= LSG_Window::GetDPIScaled(LSG_ScrollBar::ThumbRadius2x))
	{
		LSG_Graphics::RenderFillRounded(
			renderer,
			LSG_Window::GetDPIScaled(LSG_ScrollBar::ThumbRadius),
			color,
			this->scrollVertical.thumb,
			std::format("{}_scroll_thumb_vertical", component->GetID())
		);
	}
	else
	{
		SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
		LSG_Graphics::RenderFill(renderer, &this->scrollVertical.thumb);
	}
}

void LSG_ScrollBar::resetScroll()
{
	this->scrollHorizontal.offset = 0;
	this->scrollVertical.offset   = 0;
}

void LSG_ScrollBar::ScrollToHorizontal(int position)
{
	this->scrollHorizontal.offset = std::max(0, position);
}

void LSG_ScrollBar::ScrollToVertical(int position)
{
	this->scrollVertical.offset = std::max(0, position);
}
