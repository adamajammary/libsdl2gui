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

void LSG_ScrollBar::renderScrollBarHorizontal(SDL_Renderer* renderer, const SDL_Rect& background, int maxWidth, const SDL_Color& backgroundColor, bool highlighted)
{
	this->scrollBarX = background;

	this->scrollBarX.y += (background.h - LSG_SCROLL_WIDTH);
	this->scrollBarX.h  = LSG_SCROLL_WIDTH;
	
	if (this->showScrollY)
		this->scrollBarX.w -= LSG_SCROLL_WIDTH;

	auto fillColor = LSG_Graphics::GetFillColor(backgroundColor);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 255);

	SDL_RenderFillRect(renderer, &this->scrollBarX);

	auto thumbColor = (highlighted ? LSG_Graphics::GetThumbColor(backgroundColor) : LSG_SCROLL_COLOR_THUMB);

	this->scrollArrowLeft   = SDL_Rect(this->scrollBarX);
	this->scrollArrowLeft.w = LSG_SCROLL_WIDTH;

	auto triangleLeft = LSG_Graphics::GetGeometryTriangle(this->scrollArrowLeft, (LSG_SCROLL_PADDING + 2), LSG_SCROLL_PADDING, thumbColor, LSG_TRIANGLE_ORIENTATION_LEFT);

	SDL_RenderGeometry(renderer, nullptr, triangleLeft.data(), 3, nullptr, 0);

	this->scrollArrowRight    = SDL_Rect(this->scrollArrowLeft);
	this->scrollArrowRight.x += (this->scrollBarX.w - LSG_SCROLL_WIDTH);

	auto triangleRight = LSG_Graphics::GetGeometryTriangle(this->scrollArrowRight, (LSG_SCROLL_PADDING + 2), LSG_SCROLL_PADDING, thumbColor, LSG_TRIANGLE_ORIENTATION_RIGHT);

	SDL_RenderGeometry(renderer, nullptr, triangleRight.data(), 3, nullptr, 0);

	if (this->scrollBarX.w < LSG_SCROLL_MIN_SIZE) {
		this->scrollThumbX = {};
		return;
	}

	this->scrollThumbX = SDL_Rect(this->scrollBarX);

	this->scrollThumbX.x += LSG_SCROLL_WIDTH;
	this->scrollThumbX.w -= LSG_SCROLL_WIDTH_2X;
	this->scrollThumbX.y += LSG_SCROLL_PADDING;
	this->scrollThumbX.h -= LSG_SCROLL_PADDING_2X;

	this->clipFactorX = (double)((double)this->scrollThumbX.w / (double)maxWidth);

	this->scrollThumbX.w  = (int)std::ceil((double)((double)this->scrollThumbX.w + (double)LSG_SCROLL_WIDTH_2X) * this->clipFactorX);
	this->scrollThumbX.x += (int)((double)this->scrollOffsetX * this->clipFactorX);

	SDL_SetRenderDrawColor(renderer, thumbColor.r, thumbColor.g, thumbColor.b, thumbColor.a);
	SDL_RenderFillRect(renderer, &this->scrollThumbX);
}

void LSG_ScrollBar::renderScrollBarVertical(SDL_Renderer* renderer, const SDL_Rect& background, int maxHeight, const SDL_Color& backgroundColor, bool highlighted)
{
	this->scrollBarY = background;

	this->scrollBarY.x += (background.w - LSG_SCROLL_WIDTH);
	this->scrollBarY.w  = LSG_SCROLL_WIDTH;

	if (this->showScrollX)
		this->scrollBarY.h -= LSG_SCROLL_WIDTH;

	auto fillColor = LSG_Graphics::GetFillColor(backgroundColor);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 255);

	SDL_RenderFillRect(renderer, &this->scrollBarY);

	auto thumbColor = (highlighted ? LSG_Graphics::GetThumbColor(backgroundColor) : LSG_SCROLL_COLOR_THUMB);

	this->scrollArrowUp   = SDL_Rect(this->scrollBarY);
	this->scrollArrowUp.h = LSG_SCROLL_WIDTH;

	auto triangleUp = LSG_Graphics::GetGeometryTriangle(this->scrollArrowUp, LSG_SCROLL_PADDING, (LSG_SCROLL_PADDING + 2), thumbColor);

	SDL_RenderGeometry(renderer, nullptr, triangleUp.data(), 3, nullptr, 0);

	this->scrollArrowDown    = SDL_Rect(this->scrollArrowUp);
	this->scrollArrowDown.y += (this->scrollBarY.h - LSG_SCROLL_WIDTH);

	auto triangleDown = LSG_Graphics::GetGeometryTriangle(this->scrollArrowDown, LSG_SCROLL_PADDING, (LSG_SCROLL_PADDING + 2), thumbColor, LSG_TRIANGLE_ORIENTATION_DOWN);

	SDL_RenderGeometry(renderer, nullptr, triangleDown.data(), 3, nullptr, 0);

	if (this->scrollBarY.h < LSG_SCROLL_MIN_SIZE) {
		this->scrollThumbY = {};
		return;
	}

	this->scrollThumbY = SDL_Rect(this->scrollBarY);

	this->scrollThumbY.x += LSG_SCROLL_PADDING;
	this->scrollThumbY.w -= LSG_SCROLL_PADDING_2X;
	this->scrollThumbY.y += LSG_SCROLL_WIDTH;
	this->scrollThumbY.h -= LSG_SCROLL_WIDTH_2X;

	this->clipFactorY = (double)((double)this->scrollThumbY.h / (double)maxHeight);

	this->scrollThumbY.h  = (int)std::ceil((double)((double)this->scrollThumbY.h + (double)LSG_SCROLL_WIDTH_2X) * this->clipFactorY);
	this->scrollThumbY.y += (int)((double)this->scrollOffsetY * this->clipFactorY);

	SDL_SetRenderDrawColor(renderer, thumbColor.r, thumbColor.g, thumbColor.b, thumbColor.a);
	SDL_RenderFillRect(renderer, &this->scrollThumbY);
}

void LSG_ScrollBar::ScrollHome()
{
	this->scrollOffsetY = 0;
}

void LSG_ScrollBar::ScrollEnd()
{
	this->scrollOffsetY = LSG_MAX_TEXTURE_SIZE;
}

bool LSG_ScrollBar::ScrollMouseClick(const SDL_Point& mousePosition)
{
	if (LSG_Events::IsMouseDown())
		return false;

	if (!this->isSlideActiveY && this->showScrollY && SDL_PointInRect(&mousePosition, &this->scrollBarY))
	{
		this->scrollOffsetOnMouseDownY = this->scrollOffsetY;
		this->isSlideActiveY           = false;

		if (SDL_PointInRect(&mousePosition, &this->scrollArrowUp))
			this->ScrollVertical(-LSG_SCROLL_UNIT);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowDown))
			this->ScrollVertical(LSG_SCROLL_UNIT);

		if (SDL_RectEmpty(&this->scrollThumbY))
			return true;

		if (mousePosition.y < this->scrollThumbY.y)
			this->ScrollVertical(-LSG_SCROLL_UNIT_PAGE);
		else if (mousePosition.y > (this->scrollThumbY.y + this->scrollThumbY.h))
			this->ScrollVertical(LSG_SCROLL_UNIT_PAGE);
		else
			this->isSlideActiveY = true;

		return true;
	}
	else if (!this->isSlideActiveX && this->showScrollX && SDL_PointInRect(&mousePosition, &this->scrollBarX))
	{
		this->scrollOffsetOnMouseDownX = this->scrollOffsetX;
		this->isSlideActiveX           = false;

		if (SDL_PointInRect(&mousePosition, &this->scrollArrowLeft))
			this->ScrollHorizontal(-LSG_SCROLL_UNIT);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowRight))
			this->ScrollHorizontal(LSG_SCROLL_UNIT);

		if (SDL_RectEmpty(&this->scrollThumbX))
			return true;

		if (mousePosition.x < this->scrollThumbX.x)
			this->ScrollHorizontal(-LSG_SCROLL_UNIT_PAGE);
		else if (mousePosition.x > (this->scrollThumbX.x + this->scrollThumbX.w))
			this->ScrollHorizontal(LSG_SCROLL_UNIT_PAGE);
		else
			this->isSlideActiveX = true;

		return true;
	}

	return false;
}

void LSG_ScrollBar::ScrollMouseDown(const SDL_Point& mousePosition)
{
	if (!LSG_Events::IsMouseDown())
		return;

	if (!this->isSlideActiveY && this->showScrollY && SDL_PointInRect(&mousePosition, &this->scrollBarY))
	{
		if (SDL_PointInRect(&mousePosition, &this->scrollArrowUp))
			this->ScrollVertical(-LSG_SCROLL_UNIT);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowDown))
			this->ScrollVertical(LSG_SCROLL_UNIT);

		if (SDL_RectEmpty(&this->scrollThumbY))
			return;

		if (mousePosition.y < this->scrollThumbY.y)
			this->ScrollVertical(-LSG_SCROLL_UNIT_PAGE);
		else if (mousePosition.y > (this->scrollThumbY.y + this->scrollThumbY.h))
			this->ScrollVertical(LSG_SCROLL_UNIT_PAGE);
	}
	else if (!this->isSlideActiveX && this->showScrollX && SDL_PointInRect(&mousePosition, &this->scrollBarX))
	{
		if (SDL_PointInRect(&mousePosition, &this->scrollArrowLeft))
			this->ScrollHorizontal(-LSG_SCROLL_UNIT);
		else if (SDL_PointInRect(&mousePosition, &this->scrollArrowRight))
			this->ScrollHorizontal(LSG_SCROLL_UNIT);

		if (SDL_RectEmpty(&this->scrollThumbX))
			return;

		if (mousePosition.x < this->scrollThumbX.x)
			this->ScrollHorizontal(-LSG_SCROLL_UNIT_PAGE);
		else if (mousePosition.x > (this->scrollThumbX.x + this->scrollThumbX.w))
			this->ScrollHorizontal(LSG_SCROLL_UNIT_PAGE);
	}
}

void LSG_ScrollBar::ScrollMouseMove(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!LSG_Events::IsMouseDown())
		return;

	if (this->isSlideActiveY && this->showScrollY)
		this->scrollSlideVertical(mousePosition, lastEventPosition);
	else if (this->isSlideActiveX && this->showScrollX)
		this->scrollSlideHorizontal(mousePosition, lastEventPosition);
}

void LSG_ScrollBar::ScrollMouseUp()
{
	this->isSlideActiveX = false;
	this->isSlideActiveY = false;
}

void LSG_ScrollBar::ScrollHorizontal(int offset)
{
	this->scrollOffsetX += offset;

	if (this->scrollOffsetX < 0)
		this->scrollOffsetX = 0;
}

void LSG_ScrollBar::ScrollVertical(int offset)
{
	this->scrollOffsetY += offset;

	if (this->scrollOffsetY < 0)
		this->scrollOffsetY = 0;
}

void LSG_ScrollBar::scrollSlideHorizontal(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!this->showScrollX)
		return;

	auto clipFactor = (1 / this->clipFactorX);
	auto mouseDiff  = (mousePosition.x - lastEventPosition.x);
	auto offset     = (int)((double)mouseDiff * clipFactor);

	this->scrollOffsetX = (this->scrollOffsetOnMouseDownX + offset);

	if (this->scrollOffsetX < 0)
		this->scrollOffsetX = 0;
}

void LSG_ScrollBar::scrollSlideVertical(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition)
{
	if (!this->showScrollY)
		return;

	auto clipFactor = (1 / this->clipFactorY);
	auto mouseDiff  = (mousePosition.y - lastEventPosition.y);
	auto offset     = (int)((double)mouseDiff * clipFactor);

	this->scrollOffsetY = (this->scrollOffsetOnMouseDownY + offset);

	if (this->scrollOffsetY < 0)
		this->scrollOffsetY = 0;
}
