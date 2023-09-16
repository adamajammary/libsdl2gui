#include "main.h"

#ifndef LSG_SCROLL_BAR_H
#define LSG_SCROLL_BAR_H

class LSG_ScrollBar
{
public:
	LSG_ScrollBar();
	~LSG_ScrollBar() {}

protected:
	double   clipFactorX;
	double   clipFactorY;
	bool     isSlideActiveX;
	bool     isSlideActiveY;
	int      scrollOffsetX;
	int      scrollOffsetY;
	int      scrollOffsetOnMouseDownX;
	int      scrollOffsetOnMouseDownY;
	SDL_Rect scrollBarX;
	SDL_Rect scrollBarY;
	SDL_Rect scrollArrowDown;
	SDL_Rect scrollArrowLeft;
	SDL_Rect scrollArrowRight;
	SDL_Rect scrollArrowUp;
	SDL_Rect scrollThumbX;
	SDL_Rect scrollThumbY;
	bool     showScrollX;
	bool     showScrollY;

public:
	void ScrollEnd();
	void ScrollHome();
	void ScrollHorizontal(int offset);
	bool ScrollMouseClick(const SDL_Point& mousePosition);
	void ScrollMouseDown(const SDL_Point& mousePosition);
	void ScrollMouseMove(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition);
	void ScrollMouseUp();
	void ScrollVertical(int offset);

protected:
	void renderScrollBarHorizontal(SDL_Renderer* renderer, const SDL_Rect& background, int maxWidth,  const SDL_Color& backgroundColor, bool highlighted);
	void renderScrollBarVertical(SDL_Renderer* renderer,   const SDL_Rect& background, int maxHeight, const SDL_Color& backgroundColor, bool highlighted);
	void scrollSlideHorizontal(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition);
	void scrollSlideVertical(const SDL_Point& mousePosition,   const SDL_Point& lastEventPosition);

};

#endif
