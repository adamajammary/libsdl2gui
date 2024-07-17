#include "LSG_Pagination.h"

LSG_Pagination::LSG_Pagination()
{
	this->arrowEnd         = {};
	this->arrowHome        = {};
	this->arrowNext        = {};
	this->arrowPrev        = {};
	this->header           = {};
	this->items            = {};
	this->groups           = {};
	this->label            = {};
	this->page             = 0;
	this->rows             = {};
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

SDL_Size LSG_Pagination::getArrowSize(const SDL_Rect& arrow)
{
	auto padding = LSG_Graphics::GetDPIScaled(LSG_Pagination::ArrowPadding);

	return { arrow.w - padding, arrow.h - padding };
}

SDL_Rect LSG_Pagination::getDestinationCenterAligned(const SDL_Rect& background, const SDL_Size& size)
{
	SDL_Rect destination = { background.x, background.y, size.width, size.height };

	destination.x += ((background.w - destination.w) / 2);
	destination.y += ((background.h - destination.h) / 2);

	return destination;
}

LSG_TableGroup LSG_Pagination::GetGroup(const std::string& group)
{
	for (const auto& pageGroup : this->groups) {
		if (pageGroup.group == group)
			return pageGroup;
	}

	return {};
}

LSG_TableGroups LSG_Pagination::GetGroups()
{
	return this->groups;
}

LSG_Strings LSG_Pagination::GetHeader()
{
	return this->header;
}

std::string LSG_Pagination::GetItem(int row)
{
	if (!this->items.empty() && (row >= 0) && (row < (int)this->items.size()))
		return this->items[row];

	return "";
}

LSG_Strings LSG_Pagination::GetItems()
{
	return this->items;
}

std::string LSG_Pagination::getLabel()
{
	auto rows  = (int)this->GetRowCount();
	auto start = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end   = std::min(rows, (start + LSG_MAX_ROWS_PER_PAGE));
	auto page  = (this->page + 1);
	auto last  = (this->GetLastPage() + 1);

	return LSG_Text::Format("%d - %d / %d (%d / %d)", (start + 1), end, rows, page, last);
}

int LSG_Pagination::GetLastPage()
{
	auto rows      = (int)this->GetRowCount();
	auto lastPage  = (rows / LSG_MAX_ROWS_PER_PAGE);
	auto remainder = (rows % LSG_MAX_ROWS_PER_PAGE);

	if (remainder == 0)
		return (lastPage - 1);

	return lastPage;
}

int LSG_Pagination::getLastRow()
{
	auto rows  = (int)this->GetRowCount();
	auto start = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end   = std::min(rows, (start + LSG_MAX_ROWS_PER_PAGE));

	return ((end - start) - 1);
}

int LSG_Pagination::GetPage() const
{
	return this->page;
}

LSG_TableGroups LSG_Pagination::GetPageGroups()
{
	auto start = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end   = (start + LSG_MAX_ROWS_PER_PAGE);
	int  i     = -1;

	LSG_TableGroups groups;

	for (const auto& group : this->groups)
	{
		if ((++i) >= end)
			return groups;

		for (const auto& groupRow : group.rows)
			if ((++i) >= end)
				return groups;

		if (i >= start)
			groups.push_back(group);
	}

	return groups;
}

std::string LSG_Pagination::GetPageItem(int row)
{
	if (this->items.empty() || (row < 0) || (row > (int)this->getLastRow()))
		return "";

	auto rowOffset = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);

	return this->items[rowOffset];
}

LSG_Strings LSG_Pagination::GetPageItems()
{
	if (this->items.size() <= LSG_MAX_ROWS_PER_PAGE)
		return this->items;

	auto start = (size_t)(this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end   = std::min(this->items.size(), (start + LSG_MAX_ROWS_PER_PAGE));
	auto items = LSG_Strings(this->items.begin() + start, this->items.begin() + end);

	return items;
}

LSG_Strings LSG_Pagination::GetPageRow(int row)
{
	auto rowIndex = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);
	auto start    = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end      = (start + LSG_MAX_ROWS_PER_PAGE);

	return this->getRow(rowIndex, start, end);
}

LSG_TableRows LSG_Pagination::GetPageRows()
{
	auto start = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end   = (start + LSG_MAX_ROWS_PER_PAGE);
	int  i     = -1;

	for (const auto& group : this->groups)
	{
		if ((++i) >= end)
			return {};

		for (const auto& groupRow : group.rows) {
			if ((++i) >= end)
				return {};
		}
	}

	LSG_TableRows rows;

	for (const auto& row : this->rows)
	{
		if ((++i) >= end)
			return rows;

		if (i >= start)
			rows.push_back(row);
	}

	return rows;
}

SDL_Texture* LSG_Pagination::getPaginationTexture(const std::string& text, const SDL_Color& color)
{
	if (text.empty())
		return nullptr;

	SDL_Surface* surface = nullptr;
	SDL_Texture* texture = nullptr;

	auto font = LSG_Text::GetFontArial(LSG_Pagination::FontSize);

	if (font)
		surface = TTF_RenderUTF8_Blended(font, text.c_str(), color);

	if (surface)
		texture = LSG_Window::ToTexture(surface);

	SDL_FreeSurface(surface);
	TTF_CloseFont(font);

	return texture;
}

LSG_Strings LSG_Pagination::GetRow(int row)
{
	return this->getRow(row, 0, (int)this->GetRowCount());
}

LSG_Strings LSG_Pagination::getRow(int row, int start, int end)
{
	int i = -1;

	for (const auto& pageGroup : this->groups)
	{
		if ((++i) >= end)
			return {};

		for (const auto& groupRow : pageGroup.rows) {
			if ((++i) >= end)
				return {};
		}
	}

	for (const auto& pageRow : this->rows)
	{
		if ((++i) >= end)
			return {};

		if ((i >= start) && (i == row))
			return pageRow;
	}

	return {};
}

size_t LSG_Pagination::GetRowCount()
{
	if (!this->items.empty())
		return this->items.size();

	size_t rows = 0;

	for (const auto& group : this->groups)
		rows += (1 + group.rows.size());

	rows += this->rows.size();

	return rows;
}

LSG_TableRows LSG_Pagination::GetRows()
{
	return this->rows;
}

void LSG_Pagination::initPagination(const SDL_Rect& background, const SDL_Color& backgroundColor)
{
	this->updatePagination(background);

	auto color     = LSG_Graphics::GetThumbColor(backgroundColor);
	auto colorPrev = (this->page == 0 ? LSG_ScrollBar::DefaultThumbColor : color);
	auto colorNext = (this->page == this->GetLastPage() ? LSG_ScrollBar::DefaultThumbColor : color);

	this->destroyPageTextures();

	this->textureArrowHome = LSG_Graphics::GetVectorPageStart(colorPrev, this->getArrowSize(this->arrowHome));
	this->textureArrowPrev = LSG_Graphics::GetVectorPageBack(colorPrev,  this->getArrowSize(this->arrowPrev));
	this->textureArrowNext = LSG_Graphics::GetVectorPageNext(colorNext,  this->getArrowSize(this->arrowNext));
	this->textureArrowEnd  = LSG_Graphics::GetVectorPageEnd(colorNext,   this->getArrowSize(this->arrowEnd));

	this->textureLabel = this->getPaginationTexture(this->getLabel(), color);
}

bool LSG_Pagination::isEmpty()
{
	return (this->items.empty() && this->groups.empty() && this->rows.empty());
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

void LSG_Pagination::renderPagination(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Color& backgroundColor)
{
	if (!this->textureArrowHome || !this->textureArrowPrev || !this->textureArrowNext || !this->textureArrowEnd || !this->textureLabel)
		return;

	this->updatePagination(background);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, backgroundColor.r, backgroundColor.g, backgroundColor.b, 255);
	SDL_RenderFillRect(renderer, &this->pagination);

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

	auto     labelTextureSize = LSG_Graphics::GetTextureSize(this->textureLabel);
	SDL_Rect labelClip        = { 0, 0, std::min(labelTextureSize.width, this->label.w), std::min(labelTextureSize.height, this->label.h) };
	auto     labelDestination = this->getDestinationCenterAligned(this->label, { labelClip.w, labelClip.h });

	SDL_RenderCopy(renderer, this->textureLabel, &labelClip, &labelDestination);
}

bool LSG_Pagination::showPagination()
{
	return (this->GetRowCount() > LSG_MAX_ROWS_PER_PAGE);
}

void LSG_Pagination::updatePagination(const SDL_Rect& background)
{
	auto     scrollBarSize  = LSG_ScrollBar::GetSize();
	SDL_Rect backgroundArea = background;

	this->pagination    = backgroundArea;
	this->pagination.y += backgroundArea.h;
	this->pagination.h  = scrollBarSize;

	this->arrowHome   = SDL_Rect(this->pagination);
	this->arrowHome.w = scrollBarSize;

	this->arrowPrev    = SDL_Rect(this->arrowHome);
	this->arrowPrev.x += scrollBarSize;

	this->arrowNext   = SDL_Rect(this->arrowPrev);
	this->arrowNext.x = (this->pagination.x + this->pagination.w - LSG_ScrollBar::GetSize2x());

	this->arrowEnd    = SDL_Rect(this->arrowNext);
	this->arrowEnd.x += scrollBarSize;

	this->label    = SDL_Rect(this->pagination);
	this->label.x  = (this->arrowPrev.x + this->arrowPrev.w);
	this->label.w -= (this->arrowHome.w + this->arrowPrev.w + this->arrowNext.w + this->arrowEnd.w);
}
