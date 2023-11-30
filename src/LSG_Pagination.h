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
	int            page;
	LSG_Components pageItems;
	LSG_Component* pageHeader;
	LSG_Components pageGroups;
	LSG_Components pageRows;

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
	LSG_TableGroupRows GetGroup(const std::string& group);
	LSG_TableGroups    GetGroups();
	LSG_Strings        GetHeader();
	std::string        GetItem(int row);
	LSG_Strings        GetItems();
	int                GetLastPage();
	int                GetPage();
	LSG_TableGroups    GetPageGroups();
	std::string        GetPageItem(int row);
	LSG_Strings        GetPageItems();
	LSG_Strings        GetPageRow(int row);
	LSG_TableRows      GetPageRows();
	LSG_Strings        GetRow(int row);
	LSG_TableRows      GetRows();

protected:
	int  getLastRow();
	int  getRowCount();
	void initPagination(const SDL_Rect& background, const SDL_Color& backgroundColor);
	bool isPageArrowClicked(const SDL_Point& mousePosition);
	bool isPaginationClicked(const SDL_Point& mousePosition);
	bool navigate(int page);
	void renderPagination(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Color& backgroundColor);
	bool showPagination();

private:
	void               destroyPageTextures();
	SDL_Size           getArrowSize(const SDL_Rect& arrow);
	SDL_Rect           getDestinationCenterAligned(const SDL_Rect& background, const SDL_Size& size);
	LSG_TableGroupRows getGroup(LSG_Component* component);
	std::string        getLabel();
	SDL_Texture*       getPaginationTexture(const std::string& text, const SDL_Color& color);
	LSG_Strings        getRow(LSG_Component* component);
	LSG_Strings        getRow(int row, int start, int end);
	void               updatePagination(const SDL_Rect& background);

};

#endif
