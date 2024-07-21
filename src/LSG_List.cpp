#include "LSG_List.h"

LSG_List::LSG_List(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->orientation  = LSG_ConstOrientation::Vertical;
	this->selectedRows = {};
	this->wrap         = true;
}

void LSG_List::Activate()
{
	this->sendEvent(LSG_EVENT_ROW_ACTIVATED);
}

void LSG_List::Activate(const SDL_Point& mousePosition)
{
	if (!this->isPaginationClicked(mousePosition))
		this->sendEvent(LSG_EVENT_ROW_ACTIVATED);
}

void LSG_List::AddItem(const std::string& item, bool reset)
{
	if (item.empty())
		return;

	this->items.push_back(item);

	if (reset)
		this->reset();
}

int LSG_List::getRowHeight()
{
	auto header = (!this->header.empty() ? 1 : 0);
	auto rows   = (this->getLastRow() + header + 1);

	if (rows > 0)
		return (this->getTextureHeight() / rows);

	return 0;
}

std::vector<int> LSG_List::GetSelectedRows() const
{
	return this->selectedRows;
}

LSG_SortOrder LSG_List::GetSortOrder()
{
	return LSG_ConstSortOrder::ToEnum(this->GetXmlAttribute("sort"));
}

SDL_Size LSG_List::GetSize()
{
	auto attributes  = this->GetXmlAttributes();
	auto textureSize = this->getTextureSize();

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	textureSize.width  += (padding2x + border2x);
	textureSize.height += (padding2x + border2x);

	if (attributes.contains("width") && (this->background.w > 0))
		textureSize.width = this->background.w;

	if (attributes.contains("height") && (this->background.h > 0))
		textureSize.height = this->background.h;

	return textureSize;
}

bool LSG_List::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->items.empty())
		return false;

	if (this->isPaginationClicked(mousePosition))
	{
		if (this->isPageArrowClicked(mousePosition)) {
			this->Update();
			this->SelectFirstRow();
		}

		return true;
	}

	auto rowHeight  = this->getRowHeight();

	if (rowHeight < 1)
		return false;

	auto background = this->getFillArea(this->background, this->border);
	auto positionY  = (mousePosition.y - background.y + this->scrollOffsetY);
	auto clickedRow = (positionY / rowHeight);
	auto keyState   = SDL_GetKeyboardState(nullptr);

	if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL])
	{
		auto rowIter = std::find(this->selectedRows.begin(), this->selectedRows.end(), clickedRow);

		if (rowIter == this->selectedRows.end()) {
			this->selectedRows.push_back(clickedRow);
			this->sendEvent(LSG_EVENT_ROW_SELECTED);
		} else {
			this->selectedRows.erase(rowIter);
			this->sendEvent(LSG_EVENT_ROW_UNSELECTED);
		}
	}
	else if (keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT])
	{
		if (!this->selectedRows.empty())
			this->Select(this->selectedRows[0], clickedRow);
	} else {
		this->Select(clickedRow);
	}

	return true;
}

void LSG_List::removeItem(int row, int lastRow)
{
	if ((row < 0) || (row > lastRow))
		return;

	auto itemIter = (this->items.begin() + (size_t)row);

	if ((row >= (int)this->items.size()) || (itemIter == this->items.end()))
		return;

	this->items.erase(itemIter);

	this->Update();

	lastRow = this->getLastRow();

	if (lastRow < 0)
	{
		this->page          = 0;
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;

		this->Select(-1);
	} else if (!this->selectedRows.empty() && this->selectedRows[0] > lastRow) {
		this->SelectLastRow();
	}
}

void LSG_List::RemoveItem(int row)
{
	auto lastRow = (int)(this->items.size() - 1);

	this->removeItem(row, lastRow);
}

void LSG_List::RemovePageItem(int row)
{
	auto rowIndex = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);

	this->removeItem(rowIndex, this->getLastRow());
}

void LSG_List::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto attributes  = this->GetXmlAttributes();
	auto textureSize = this->getTextureSize();

	SDL_Size size = {};

	if (attributes.contains("width") && (this->background.w > 0))
		size.width = this->background.w;

	if (attributes.contains("height") && (this->background.h > 0))
		size.height = this->background.h;

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	this->background.x = position.x;
	this->background.y = position.y;
	this->background.w = (size.width  > 0 ? size.width  : (textureSize.width  + padding2x + border2x));
	this->background.h = (size.height > 0 ? size.height : (textureSize.height + padding2x + border2x));

	this->render(renderer);
}

void LSG_List::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_List::render(SDL_Renderer* renderer)
{
	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto fillArea        = this->getFillArea(this->background, this->border);
	auto rowHeight       = this->getRowHeight();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();
	bool showPagination  = this->showPagination();
	bool showRowBorder   = (this->GetXmlAttribute("show-row-border") == "true");
	auto textureSize     = this->getTextureSize();

	if (fillArea.h < scrollBarSize2x)
		return;

	if (showPagination)
		fillArea.h -= LSG_ScrollBar::GetSize();

	this->renderScrollableTexture(renderer, fillArea, this->border, this->getAlignment(), this->texture, textureSize);

	if (showRowBorder)
		this->renderRowBorder(renderer, fillArea, rowHeight);

	this->renderHighlightSelection(renderer, fillArea, rowHeight);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, fillArea, textureSize.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, fillArea, textureSize.height, this->backgroundColor, this->highlighted);

	if (showPagination)
		this->renderPagination(renderer, fillArea, this->backgroundColor);
}

void LSG_List::renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight)
{
	if (this->selectedRows.empty())
		return;

	auto fillColor = LSG_Graphics::GetInverseColor(this->backgroundColor);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 64);

	auto header = (!this->header.empty() ? rowHeight : 0);
	auto rowY   = (background.y + header - this->scrollOffsetY);

	SDL_Rect row = background;

	row.w -= (this->showScrollY ? LSG_ScrollBar::GetSize() : 0);

	for (auto selectedRow : this->selectedRows)
	{
		row.y = (rowY + (selectedRow * rowHeight));
		row.h = rowHeight;

		auto offsetTop    = ((row.y + rowHeight) - background.y);
		auto offsetBottom = ((background.y + background.h) - row.y);

		if ((offsetTop <= 0) || (offsetBottom <= 0))
			continue;

		if (offsetTop < rowHeight) {
			row.y += (row.h - offsetTop);
			row.h  = offsetTop;
		} else if (offsetBottom < rowHeight) {
			row.h = offsetBottom;
		}

		SDL_RenderFillRect(renderer, &row);
	}
}

void LSG_List::renderRowBorder(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight)
{
	if (rowHeight < 1)
		return;

	auto fillColor = LSG_Graphics::GetInverseColor(this->backgroundColor);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 64);

	auto scrollOffsetY = (this->showScrollY ? (this->scrollOffsetY % rowHeight) : 0);
	auto scrollSize    = LSG_ScrollBar::GetSize();
	auto scrollWidth   = (this->showScrollY ? scrollSize : 0);
	auto scrollHeight  = (this->showScrollX ? scrollSize : 0);

	auto header = (!this->header.empty() ? rowHeight : 0);
	auto rows   = (int)((float)(background.h - header - scrollHeight) / (float)rowHeight);

	auto y  = (background.y + header + rowHeight - scrollOffsetY);
	auto x2 = (background.x + background.w - 1 - scrollWidth);

	for (int i = 0; i < rows; i++) {
		SDL_RenderDrawLine(renderer, background.x, y, x2, y);
		y += rowHeight;
	}
}

void LSG_List::reset()
{
	this->destroyTextures();
	this->SetItems();
}

bool LSG_List::Select(int row)
{
	if (!this->enabled || (row > this->getLastRow()))
		return false;

	this->selectedRows = { row };

	this->sendEvent(row < 0 ? LSG_EVENT_ROW_UNSELECTED : LSG_EVENT_ROW_SELECTED);

	return true;
}

bool LSG_List::Select(int start, int end)
{
	if (!this->enabled)
		return false;

	this->selectedRows.clear();

	if ((start < 0) || (end < 0))
		return false;

	if (end < start) {
		for (int i = start; i >= end; i--)
			this->selectedRows.push_back(i);
	} else {
		for (int i = start; i <= end; i++)
			this->selectedRows.push_back(i);
	}

	this->sendEvent(this->selectedRows.empty() ? LSG_EVENT_ROW_UNSELECTED : LSG_EVENT_ROW_SELECTED);

	return true;
}

bool LSG_List::Select(const std::vector<int>& rows)
{
	if (!this->enabled)
		return false;

	this->selectedRows.clear();

	auto lastRow = this->getLastRow();

	for (auto row : rows)
	{
		auto rowIter = std::find(this->selectedRows.begin(), this->selectedRows.end(), row);

		if ((row >= 0) && (row <= lastRow) && (rowIter == this->selectedRows.end()))
			this->selectedRows.push_back(row);
	}

	this->sendEvent(this->selectedRows.empty() ? LSG_EVENT_ROW_UNSELECTED : LSG_EVENT_ROW_SELECTED);

	return true;
}

void LSG_List::SelectAll()
{
	if (!this->enabled || this->isEmpty())
		return;

	this->Select(0, this->getLastRow());
	this->OnScrollHome();
}

void LSG_List::SelectFirstRow()
{
	if (!this->enabled || this->isEmpty())
		return;

	this->Select(0);
	this->OnScrollHome();
}

void LSG_List::SelectFirstRowShift()
{
	if (!this->enabled || this->selectedRows.empty() || this->isEmpty())
		return;

	this->Select(this->selectedRows[0], 0);
	this->OnScrollHome();
}

void LSG_List::SelectLastRow()
{
	if (!this->enabled || this->isEmpty())
		return;

	this->Select(this->getLastRow());
	this->OnScrollEnd();
}

void LSG_List::SelectLastRowShift()
{
	if (!this->enabled || this->selectedRows.empty() || this->isEmpty())
		return;

	this->Select(this->selectedRows[0], this->getLastRow());
	this->OnScrollEnd();
}

void LSG_List::SelectRow(int offset, bool multiSelect)
{
	if (!this->enabled || this->selectedRows.empty() || this->isEmpty())
		return;

	auto selectedRow = (!multiSelect ? this->selectedRows[0] : this->selectedRows[this->selectedRows.size() - 1]);
	auto nextRow     = std::max(0, std::min(this->getLastRow(), (selectedRow + offset)));

	if (!multiSelect)
		this->Select(nextRow);
	else
		this->Select(this->selectedRows[0], nextRow);

	if (!this->showScrollY)
		return;

	auto background    = this->getFillArea(this->background, this->border);
	auto rowHeight     = this->getRowHeight();
	auto rowY          = (background.y + (selectedRow * rowHeight));
	auto scrollBarSize = LSG_ScrollBar::GetSize();
	auto pagination    = (this->showPagination() ? scrollBarSize : 0);
	auto scrollX       = (this->showScrollX ? scrollBarSize : 0);
	auto topY          = (background.y + this->scrollOffsetY);
	auto bottomY       = (topY + background.h - scrollX - pagination);

	if ((rowY + rowHeight) >= bottomY)
		this->OnScrollVertical(std::abs(offset) * rowHeight);
	else if (rowY < topY)
		this->OnScrollVertical(std::abs(offset) * (-rowHeight));
}

void LSG_List::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event listEvent = {};

	listEvent.type       = SDL_RegisterEvents(1);
	listEvent.user.code  = (int)type;
	listEvent.user.data1 = (void*)strdup(this->id.c_str());
	listEvent.user.data2 = new std::vector(this->selectedRows);

	SDL_PushEvent(&listEvent);
}

void LSG_List::SetItem(int row, const std::string& item)
{
	auto lastRow = (int)(this->items.size() - 1);

	this->setItem(row, lastRow, item);
}

void LSG_List::setItem(int row, int lastRow, const std::string& item)
{
	if ((row < 0) || (row > lastRow) || (row >= (int)this->items.size()))
		return;

	this->items[row] = item;

	this->destroyTextures();
	this->setItems();
}

void LSG_List::SetItems(const LSG_Strings& items)
{
	this->page          = 0;
	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;

	this->destroyTextures();

	this->items = items;

	this->setItems();

	this->Select(!this->items.empty() ? 0 : -1);
}

void LSG_List::SetItems()
{
	if (this->texture && !this->hasChanged())
		return;

	this->destroyTextures();

	this->setItems();
}

void LSG_List::setItems(bool sort)
{
	if (this->showPagination())
		this->initPagination(this->getFillArea(this->background, this->border), this->backgroundColor);

	auto sortOrder = this->GetXmlAttribute("sort");

	if (sort && !sortOrder.empty())
		this->sort();

	auto        items = this->GetPageItems();
	std::string text  = "";

	for (const auto& item : items)
		text.append(LSG_Text::Format("%s\n", (!item.empty() ? item.c_str() : " ")));

	if (!text.empty())
		this->texture = this->getTexture(text);
}

void LSG_List::SetPage(int page)
{
	if (!this->navigate(page))
		return;

	this->reset();
	this->SelectFirstRow();
}

void LSG_List::SetPageItem(int row, const std::string& item)
{
	auto rowIndex = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);

	this->setItem(rowIndex, this->getLastRow(), item);
}

void LSG_List::Sort(LSG_SortOrder sortOrder)
{
	if (sortOrder == this->GetSortOrder())
		return;

	LSG_XML::SetAttribute(this->xmlNode, "sort", LSG_ConstSortOrder::ToString(sortOrder));

	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;

	this->reset();

	this->Select(!this->items.empty() ? 0 : -1);
}

void LSG_List::sort()
{
	if (this->GetXmlAttribute("sort") == LSG_ConstSortOrder::Descending)
		std::sort(this->items.rbegin(), this->items.rend());
	else
		std::sort(this->items.begin(), this->items.end());
}

void LSG_List::Update()
{
	this->destroyTextures();
	this->setItems(false);
}
