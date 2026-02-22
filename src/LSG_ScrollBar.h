#include "main.h"

#ifndef LSG_SCROLL_BAR_H
#define LSG_SCROLL_BAR_H

struct LSG_ScrollBarState
{
	SDL_Rect arrowBack         = {};
	SDL_Rect arrowForward      = {};
	SDL_Rect bar               = {};
	double   clipFactor        = 0.0;
	bool     isSlideActive     = false;
	int      offset            = 0;
	int      offsetOnMouseDown = 0;
	bool     show              = false;
	SDL_Rect thumb             = {};
};

class LSG_ScrollBar
{
public:
	LSG_ScrollBar();
	~LSG_ScrollBar() {}

public:
	static inline const SDL_Color DefaultThumbColor = { 128, 128, 128, 255 };

public:
	static const int Padding   = 5;
	static const int Padding2x = 10;
	static const int Unit      = 40;
	static const int UnitPage  = 140;
	static const int UnitWheel = 20;

private:
	static const int Size          = 20;
	static const int Size2x        = 40;
	static const int ThumbRadius   = 5;
	static const int ThumbRadius2x = 10;

protected:
	LSG_ScrollBarState scrollHorizontal;
	LSG_ScrollBarState scrollVertical;

public:
	static int GetSize();
	static int GetSize2x();

public:
	int  GetScrollHorizontal() const;
	int  GetScrollVertical() const;
	bool IsMouseOverScrollbar(const SDL_Point& mousePosition) const;
	bool OnScrollEnd();
	bool OnScrollHome();
	bool OnScrollMouseClick(const SDL_Point& mousePosition);
	bool OnScrollMouseDown(const SDL_Point&  mousePosition);
	bool OnScrollMouseMove(const SDL_Point&  mousePosition, const SDL_Point& lastEventPosition);
	void OnScrollMouseUp();
	bool OnScrollHorizontal(int offset, bool forceScroll = false);
	bool OnScrollVertical(int   offset, bool forceScroll = false);

protected:
	SDL_Rect getClipWithOffset(const SDL_Rect& clip, const SDL_Size& textureSize);
	SDL_Rect getScrollableBackground(const SDL_Rect& fillArea, int borderWidth, const SDL_Size& textureSize);
	SDL_Rect getScrollableClip(const SDL_Rect& background, const SDL_Size& textureSize);
	void     renderScrollableTexture(SDL_Renderer* renderer, const SDL_Rect& fillArea, int borderWidth, const LSG_Alignment& alignment, SDL_Texture* texture, const SDL_Size& size);
	void     renderScrollBarHorizontal(SDL_Renderer* renderer, const SDL_Rect& background, int maxWidth,  const SDL_Color& backgroundColor, bool highlighted, LSG_Component* component);
	void     renderScrollBarVertical(SDL_Renderer*   renderer, const SDL_Rect& background, int maxHeight, const SDL_Color& backgroundColor, bool highlighted, LSG_Component* component);
	void     resetScroll();

private:
	SDL_Rect getScrollBarHorizontal(const SDL_Rect& background) const;
	SDL_Rect getScrollBarVertical(const   SDL_Rect& background) const;
	bool     onScrollSlideHorizontal(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition);
	bool     onScrollSlideVertical(const SDL_Point&   mousePosition, const SDL_Point& lastEventPosition);
	void     renderScrollArrowsHorizontal(SDL_Renderer* renderer, const SDL_Color& color);
	void     renderScrollArrowsVertical(SDL_Renderer*   renderer, const SDL_Color& color);
	void     renderScrollBar(SDL_Renderer* renderer, const SDL_Rect& bar, const SDL_Color& backgroundColor) const;
	void     renderScrollThumbHorizontal(SDL_Renderer* renderer, const SDL_Color& color, int maxWidth,  LSG_Component* component);
	void     renderScrollThumbVertical(SDL_Renderer*   renderer, const SDL_Color& color, int maxHeight, LSG_Component* component);
};

#endif
