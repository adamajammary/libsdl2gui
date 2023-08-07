#include "main.h"

#ifndef LSG_PAGINATION_H
#define LSG_PAGINATION_H

class LSG_Pagination
{
public:
	LSG_Pagination();
	~LSG_Pagination();

protected:
	int             page;
	LSG_Strings     pageItems;
	LSG_Strings     pageHeader;
	LSG_TableGroups pageGroups;
	LSG_TableRows   pageRows;

private:
	SDL_Rect     arrowEnd;
	SDL_Rect     arrowHome;
	SDL_Rect     arrowNext;
	SDL_Rect     arrowPrev;
	SDL_Rect     pagination;
	SDL_Texture* textureArrowEnd;
	SDL_Texture* textureArrowHome;
	SDL_Texture* textureArrowNext;
	SDL_Texture* textureArrowPrev;
	SDL_Texture* textureLabel;

protected:
	void initPagination(const SDL_Rect& backgroundArea, const SDL_Color& backgroundColor);
	bool isPageArrowClicked(const SDL_Point& mousePosition);
	bool isPaginationClicked(const SDL_Point& mousePosition);
	void renderPagination(SDL_Renderer* renderer, const SDL_Rect& backgroundArea);
	bool showPagination();

private:
	void         destroyPageTextures();
	SDL_Rect     getDestinationCenterAligned(const SDL_Rect& backgroundArea, const SDL_Size& size);
	std::string  getLabel();
	int          getMaxPage();
	SDL_Texture* getPaginationTexture(const std::string& text, const SDL_Color& color, bool bold = false);
	int          getRowCount();
	bool         navigate(int page);
	void         updatePagination(const SDL_Rect& backgroundArea);

};

#endif
