#include "main.h"

#ifndef LSG_SCROLL_BAR_H
#define LSG_SCROLL_BAR_H

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
	static const int Size   = 20;
	static const int Size2x = 40;

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
	static int GetSize();
	static int GetSize2x();

public:
	int  GetScrollX() const;
	int  GetScrollY() const;
	void OnScrollEnd();
	void OnScrollHome();
	void OnScrollHorizontal(int offset);
	bool OnScrollMouseClick(const SDL_Point& mousePosition);
	void OnScrollMouseDown(const SDL_Point&  mousePosition);
	void OnScrollMouseMove(const SDL_Point&  mousePosition, const SDL_Point& lastEventPosition);
	void OnScrollMouseUp();
	void OnScrollVertical(int offset);

protected:
	void onScrollSlideHorizontal(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition);
	void onScrollSlideVertical(const SDL_Point&   mousePosition, const SDL_Point& lastEventPosition);
	void renderScrollableTexture(SDL_Renderer*   renderer, const SDL_Rect& background, const LSG_Alignment& alignment, SDL_Texture* texture, const SDL_Size& size);
	void renderScrollableTextures(SDL_Renderer*  renderer,const SDL_Rect& background, const LSG_Alignment& alignment, const std::vector<SDL_Texture*>& textures, const SDL_Size& size, int spacing);
	void renderScrollBarHorizontal(SDL_Renderer* renderer, const SDL_Rect& background, int maxWidth,  const SDL_Color& backgroundColor, bool highlighted);
	void renderScrollBarVertical(SDL_Renderer*   renderer, const SDL_Rect& background, int maxHeight, const SDL_Color& backgroundColor, bool highlighted);

private:
	SDL_Rect getScrollableBackground(const SDL_Rect& background, const SDL_Size& textureSize, int scrollBarSize);
	SDL_Rect getScrollableClip(const SDL_Rect&       background, const SDL_Size& textureSize);

};

#endif
