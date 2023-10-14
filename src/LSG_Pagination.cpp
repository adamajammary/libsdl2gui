#include "LSG_Pagination.h"

LSG_Pagination::LSG_Pagination()
{
	this->arrowEnd         = {};
	this->arrowHome        = {};
	this->arrowNext        = {};
	this->arrowPrev        = {};
	this->page             = 0;
	this->pageHeader       = {};
	this->pageItems        = {};
	this->pageGroups       = {};
	this->pageRows         = {};
	this->pagination       = {};
	this->textureArrowEnd  = nullptr;
	this->textureArrowHome = nullptr;
	this->textureArrowNext = nullptr;
	this->textureArrowPrev = nullptr;
	this->textureLabel     = nullptr;
}

LSG_Pagination::~LSG_Pagination()
{
	this->destroyPageTextures();
}

void LSG_Pagination::destroyPageTextures()
{
	if (this->textureArrowHome) {
		SDL_DestroyTexture(this->textureArrowHome);
		this->textureArrowHome = nullptr;
	}

	if (this->textureArrowPrev) {
		SDL_DestroyTexture(this->textureArrowPrev);
		this->textureArrowPrev = nullptr;
	}

	if (this->textureArrowNext) {
		SDL_DestroyTexture(this->textureArrowNext);
		this->textureArrowNext = nullptr;
	}

	if (this->textureArrowEnd) {
		SDL_DestroyTexture(this->textureArrowEnd);
		this->textureArrowEnd = nullptr;
	}

	if (this->textureLabel) {
		SDL_DestroyTexture(this->textureLabel);
		this->textureLabel = nullptr;
	}
}

SDL_Rect LSG_Pagination::getDestinationCenterAligned(const SDL_Rect& backgroundArea, const SDL_Size& size)
{
	SDL_Rect destination = { backgroundArea.x, backgroundArea.y, size.width, size.height };

	destination.x += ((backgroundArea.w - destination.w) / 2);
	destination.y += ((backgroundArea.h - destination.h) / 2);

	return destination;
}

std::string LSG_Pagination::getLabel()
{
	auto rows  = this->getRowCount();
	auto start = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end   = std::min(rows, (start + LSG_MAX_ROWS_PER_PAGE));
	auto page  = (this->page + 1);
	auto last  = (this->GetLastPage() + 1);
	auto label = LSG_Text::Format("%d - %d / %d ( %d / %d )", (start + 1), end, rows, page, last);

	return label;
}

int LSG_Pagination::GetLastPage()
{
	auto rows      = this->getRowCount();
	auto lastPage  = (rows / LSG_MAX_ROWS_PER_PAGE);
	auto remainder = (rows % LSG_MAX_ROWS_PER_PAGE);

	if (remainder == 0)
		return (lastPage - 1);

	return lastPage;
}

int LSG_Pagination::GetPage()
{
	return this->page;
}

SDL_Texture* LSG_Pagination::getPaginationTexture(const std::string& text, const SDL_Color& color, bool bold)
{
	if (text.empty())
		return nullptr;

	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;

	auto font = LSG_Text::GetFontArial(LSG_Const::PaginationFontSize);

	if (bold && font)
		TTF_SetFontStyle(font, TTF_STYLE_BOLD);

	if (font)
		surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);

	if (surface)
		texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);
	TTF_CloseFont(font);

	return texture;
}

int LSG_Pagination::getRowCount()
{
	if (!this->pageItems.empty())
		return (int)this->pageItems.size();

	int rows = 0;

	for (const auto& group : this->pageGroups)
		rows += (int)(1 + group.rows.size());

	rows += (int)this->pageRows.size();

	return rows;
}

void LSG_Pagination::initPagination(const SDL_Rect& backgroundArea, const SDL_Color& backgroundColor)
{
	this->updatePagination(backgroundArea);

	auto color     = LSG_Graphics::GetThumbColor(backgroundColor);
	auto colorPrev = (this->page == 0 ? LSG_ConstDefaultColor::ScrollThumb : color);
	auto colorNext = (this->page == this->GetLastPage() ? LSG_ConstDefaultColor::ScrollThumb : color);

	this->destroyPageTextures();

	this->textureArrowHome = this->getPaginationTexture("<<", colorPrev, true);
	this->textureArrowPrev = this->getPaginationTexture("<",  colorPrev, true);
	this->textureArrowNext = this->getPaginationTexture(">",  colorNext, true);
	this->textureArrowEnd  = this->getPaginationTexture(">>", colorNext, true);
	this->textureLabel     = this->getPaginationTexture(this->getLabel(), color);
}

bool LSG_Pagination::isPageArrowClicked(const SDL_Point& mousePosition)
{
	if (SDL_PointInRect(&mousePosition, &this->arrowHome))
		return this->navigate(0);
	else if (SDL_PointInRect(&mousePosition, &this->arrowPrev))
		return this->navigate(std::max(0, this->page - 1));
	else if (SDL_PointInRect(&mousePosition, &this->arrowNext))
		return this->navigate(std::min(this->GetLastPage(), this->page + 1));
	else if (SDL_PointInRect(&mousePosition, &this->arrowEnd))
		return this->navigate(this->GetLastPage());

	return false;
}

bool LSG_Pagination::isPaginationClicked(const SDL_Point& mousePosition)
{
	return (this->showPagination() && SDL_PointInRect(&mousePosition, &this->pagination));
}

bool LSG_Pagination::navigate(int page)
{
	if (page == this->page)
		return false;

	this->page = page;

	return true;
}

void LSG_Pagination::renderPagination(SDL_Renderer* renderer, const SDL_Rect& backgroundArea)
{
	if (!this->textureArrowHome || !this->textureArrowPrev || !this->textureArrowNext || !this->textureArrowEnd || !this->textureLabel)
		return;

	this->updatePagination(backgroundArea);

	auto arrowHomeTextureSize = LSG_Graphics::GetTextureSize(this->textureArrowHome);
	auto arrowHomeDestination = this->getDestinationCenterAligned(this->arrowHome, arrowHomeTextureSize);

	SDL_RenderCopy(renderer, this->textureArrowHome, nullptr, &arrowHomeDestination);

	auto arrowPrevTextureSize = LSG_Graphics::GetTextureSize(this->textureArrowPrev);
	auto arrowPrevDestination = this->getDestinationCenterAligned(this->arrowPrev, arrowPrevTextureSize);

	SDL_RenderCopy(renderer, this->textureArrowPrev, nullptr, &arrowPrevDestination);

	auto arrowNextTextureSize = LSG_Graphics::GetTextureSize(this->textureArrowNext);
	auto arrowNextDestination = this->getDestinationCenterAligned(this->arrowNext, arrowNextTextureSize);

	SDL_RenderCopy(renderer, this->textureArrowNext, nullptr, &arrowNextDestination);

	auto arrowEndTextureSize = LSG_Graphics::GetTextureSize(this->textureArrowEnd);
	auto arrowEndDestination = this->getDestinationCenterAligned(this->arrowEnd, arrowEndTextureSize);

	SDL_RenderCopy(renderer, this->textureArrowEnd, nullptr, &arrowEndDestination);

	auto labelTextureSize = LSG_Graphics::GetTextureSize(this->textureLabel);
	auto labelDestination = SDL_Rect(this->pagination);

	labelDestination.x += LSG_ConstScrollBar::Width;
	labelDestination.w -= LSG_ConstScrollBar::Width2x;

	SDL_Rect clip = { 0, 0, std::min(labelTextureSize.width, labelDestination.w), std::min(labelTextureSize.height, labelDestination.h) };
	auto     dest = this->getDestinationCenterAligned(labelDestination, { clip.w, clip.h });

	SDL_RenderCopy(renderer, this->textureLabel, &clip, &dest);
}

bool LSG_Pagination::showPagination()
{
	return (this->getRowCount() >= LSG_MAX_ROWS_PER_PAGE);
}

void LSG_Pagination::updatePagination(const SDL_Rect& backgroundArea)
{
	this->pagination    = backgroundArea;
	this->pagination.y += backgroundArea.h;
	this->pagination.h  = LSG_ConstScrollBar::Width;

	this->arrowHome   = SDL_Rect(this->pagination);
	this->arrowHome.w = LSG_ConstScrollBar::Width;

	this->arrowPrev    = SDL_Rect(this->arrowHome);
	this->arrowPrev.x += LSG_ConstScrollBar::Width;

	this->arrowNext   = SDL_Rect(this->arrowPrev);
	this->arrowNext.x = (this->pagination.x + this->pagination.w - LSG_ConstScrollBar::Width2x);

	this->arrowEnd    = SDL_Rect(this->arrowNext);
	this->arrowEnd.x += LSG_ConstScrollBar::Width;
}
