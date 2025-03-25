#include "main.h"

#ifndef LSG_PAGINATION_H
#define LSG_PAGINATION_H

class LSG_Pagination
{
public:
	LSG_Pagination();
	~LSG_Pagination();

private:
	static const int ArrowPadding = 10;
	static const int FontSize     = 13;

protected:
	LSG_Strings     items;
	LSG_Strings     header;
	LSG_TableGroups groups;
	int             page;
	LSG_TableRows   rows;

private:
	SDL_Rect     pagination;
	SDL_Rect     arrowEnd;
	SDL_Rect     arrowHome;
	SDL_Rect     arrowNext;
	SDL_Rect     arrowPrev;
	SDL_Rect     label;
	SDL_Texture* textureArrowEnd;
	SDL_Texture* textureArrowHome;
	SDL_Texture* textureArrowNext;
	SDL_Texture* textureArrowPrev;
	SDL_Texture* textureLabel;

public:
	LSG_TableGroup  GetGroup(const std::string& group) const;
	LSG_TableGroups GetGroups() const;
	LSG_Strings     GetHeader() const;
	std::string     GetItem(int row) const;
	LSG_Strings     GetItems() const;
	int             GetLastPage() const;
	int             GetPage() const;
	LSG_TableGroups GetPageGroups() const;
	std::string     GetPageItem(int row) const;
	LSG_Strings     GetPageItems() const;
	LSG_Strings     GetPageRow(int row) const;
	LSG_TableRows   GetPageRows() const;
	LSG_Strings     GetRow(int row) const;
	size_t          GetRowCount() const;
	LSG_TableRows   GetRows() const;

protected:
	int  getLastRow() const;
	void initPagination(const SDL_Rect& background, const SDL_Color& backgroundColor);
	bool isEmpty() const;
	bool isPageArrowClicked(const SDL_Point& mousePosition, const std::string& id);
	bool isPaginationClicked(const SDL_Point& mousePosition) const;
	bool navigate(int page, const std::string& id);
	void renderPagination(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Color& backgroundColor);
	bool showPagination() const;

private:
	void         destroyPageTextures();
	SDL_Size     getArrowSize(const SDL_Rect& arrow) const;
	SDL_Rect     getDestinationCenterAligned(const SDL_Rect& background, const SDL_Size& size) const;
	std::string  getLabel() const;
	SDL_Texture* getPaginationTexture(const std::string& text, const SDL_Color& color) const;
	LSG_Strings  getRow(int row, int start, int end) const;
	void         updatePagination(const SDL_Rect& background);
};

#endif
