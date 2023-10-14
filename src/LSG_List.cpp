#include "LSG_List.h"

LSG_List::LSG_List(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->rows        = {};
	this->orientation = LSG_ConstOrientation::Vertical;
	this->row         = -1;
	this->wrap        = true;

	this->showRowBorder = (this->GetXmlAttribute("row-border") == "true");
	this->sortOrder     = this->GetXmlAttribute("sort");
}

void LSG_List::Activate()
{
	this->sendEvent(LSG_EVENT_ROW_ACTIVATED);
}

void LSG_List::AddItem(const std::string& item)
{
	if (item.empty())
		return;

	this->pageItems.push_back(item);

	this->SetItems(this->pageItems);
}

std::string LSG_List::GetItem(int row)
{
	if (!this->rows.empty() && (row >= 0) && (row < (int)this->rows.size()))
		return this->rows[row].item;

	return "";
}

LSG_Strings LSG_List::GetItems()
{
	LSG_Strings items;

	for (const auto& row : this->rows)
		items.push_back(row.item);

	return items;
}

int LSG_List::getLastRow()
{
	return (int)(this->rows.size() - 1);
}

LSG_Strings LSG_List::getPageItems()
{
	if (this->pageItems.size() <= LSG_MAX_ROWS_PER_PAGE)
		return this->pageItems;

	auto offset = (size_t)(this->page * LSG_MAX_ROWS_PER_PAGE);
	auto max    = std::min(this->pageItems.size(), (offset + LSG_MAX_ROWS_PER_PAGE));
	auto start  = (this->pageItems.begin() + offset);
	auto end    = (this->pageItems.begin() + max);

	return LSG_Strings(start, end);
}

int LSG_List::getRowHeight()
{
	if (this->rows.empty())
		return 0;

	auto textureSize = this->GetTextureSize();
	auto rowHeight   = (textureSize.height / (int)this->rows.size());

	return rowHeight;
}

int LSG_List::GetSelectedRow()
{
	return this->row;
}

LSG_SortOrder LSG_List::GetSortOrder()
{
	 return LSG_ConstSortOrder::ToEnum(this->sortOrder);
}

bool LSG_List::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->rows.empty())
		return false;

	SDL_Point mousePosition = { event.x, event.y };

	if (this->isPaginationClicked(mousePosition))
	{
		if (this->isPageArrowClicked(mousePosition))
			this->updatePage();

		return true;
	}

	auto positionY = (mousePosition.y - this->background.y + this->scrollOffsetY);
	auto rowHeight = this->getRowHeight();

	if (rowHeight < 1)
		return false;

	bool selected = this->Select(positionY / rowHeight);

	if (selected && LSG_Events::IsDoubleClick(event))
		this->sendEvent(LSG_EVENT_ROW_ACTIVATED);

	return true;
}

void LSG_List::RemoveItem(int row)
{
	auto rowOffset = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);

	if ((rowOffset < 0) || (rowOffset >= (int)this->pageItems.size()))
		return;

	this->pageItems.erase(this->pageItems.begin() + (size_t)rowOffset);
	this->SetItems(this->pageItems);
}

void LSG_List::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (!this->texture || this->rows.empty())
		return;

	auto listBackground = this->getFillArea(this->background, this->border);
	auto rowHeight      = this->getRowHeight();
	auto size           = this->GetTextureSize();
	bool showPagination = this->showPagination();

	if (showPagination)
		listBackground.h -= LSG_ConstScrollBar::Width;

	this->setRowHeights(rowHeight, listBackground);
	this->renderTexture(renderer, listBackground);

	if (this->showRowBorder)
		this->renderRowBorder(renderer, rowHeight, listBackground);

	this->renderHighlightSelection(renderer, listBackground, this->rows[0].background);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, listBackground, size.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, listBackground, size.height, this->backgroundColor, this->highlighted);

	if (showPagination)
		this->renderPagination(renderer, listBackground);
}

void LSG_List::renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& backgroundArea, const SDL_Rect& rowBackground)
{
	SDL_Rect row = rowBackground;

	row.y = (backgroundArea.y + (this->row * rowBackground.h));

	if (this->showScrollY)
		row.w -= LSG_ConstScrollBar::Width;

	row.y -= this->scrollOffsetY;

	auto listBottomY = (backgroundArea.y + backgroundArea.h);
	auto bottomY     = (row.y + row.h);
	auto topDiffY    = (bottomY - backgroundArea.y);
	auto bottomDiffY = (listBottomY - row.y);

	if (topDiffY < row.h) {
		row.y += (row.h - topDiffY);
		row.h  = topDiffY;
	} else if (bottomDiffY < row.h) {
		row.h = bottomDiffY;
	}

	bool isVisible = ((bottomY >= backgroundArea.y) && (row.y <= listBottomY));
	
	if (!isVisible)
		return;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 64);

	SDL_RenderFillRect(renderer, &row);
}

void LSG_List::renderRowBorder(SDL_Renderer* renderer, int rowHeight, const SDL_Rect& backgroundArea)
{
	if (rowHeight < 1)
		return;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 64);

	if (!this->showScrollY)
	{
		auto rows = ((backgroundArea.h - (this->showScrollX ? LSG_ConstScrollBar::Width : 0)) / rowHeight);
		auto y    = (backgroundArea.y + rowHeight - 1);
		auto x2   = (backgroundArea.x + backgroundArea.w);

		for (int i = 0; i < rows; i++) {
			SDL_RenderDrawLine(renderer, backgroundArea.x, y, x2, y);
			y += rowHeight;
		}

		return;
	}

	auto bottomY = (backgroundArea.y + backgroundArea.h - (this->showScrollX ? LSG_ConstScrollBar::Width : 0));

	for (const auto& row : this->rows)
	{
		auto y  = (row.background.y + row.background.h - 1 - this->scrollOffsetY);
		auto x2 = (row.background.x + row.background.w - (this->showScrollY ? LSG_ConstScrollBar::Width : 0));

		if ((y >= backgroundArea.y) && (y <= bottomY))
			SDL_RenderDrawLine(renderer, row.background.x, y, x2, y);
	}
}

bool LSG_List::Select(int row)
{
	if (!this->enabled || this->rows.empty())
		return false;

	auto lastRow = this->getLastRow();

	if ((row < 0) || (row > lastRow))
		return false;

	if (row == this->row)
		return true;

	this->row = row;

	this->sendEvent(LSG_EVENT_ROW_SELECTED);

	return true;
}

void LSG_List::SelectFirstRow()
{
	if (!this->enabled || this->rows.empty())
		return;

	this->Select(0);
	this->ScrollHome();
}

void LSG_List::SelectLastRow()
{
	if (!this->enabled || this->rows.empty())
		return;

	this->Select(this->getLastRow());
	this->ScrollEnd();
}

void LSG_List::SelectRow(int offset)
{
	if (!this->enabled || this->rows.empty())
		return;

	auto lastRow     = this->getLastRow();
	auto selectedRow = std::max(0, std::min(lastRow, (this->row + offset)));

	this->Select(selectedRow);

	if (!this->showScrollY)
		return;

	auto list       = this->getFillArea(this->background, this->border);
	auto row        = this->rows[0].background;
	auto rowY       = (list.y + (this->row * row.h));
	auto pagination = (this->showPagination() ? LSG_ConstScrollBar::Width : 0);
	auto scrollX    = (this->showScrollX ? LSG_ConstScrollBar::Width : 0);
	auto listTop    = (list.y + this->scrollOffsetY);
	auto listBottom = (listTop + list.h - scrollX - pagination);

	if ((rowY + row.h) >= listBottom)
		this->ScrollVertical(std::abs(offset) * row.h);
	else if (rowY < listTop)
		this->ScrollVertical(std::abs(offset) * -row.h);
}

void LSG_List::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event listEvent = {};

	listEvent.type       = SDL_RegisterEvents(1);
	listEvent.user.code  = (int)type;
	listEvent.user.data1 = (void*)strdup(this->GetID().c_str());
	listEvent.user.data2 = (void*)&this->row;

	SDL_PushEvent(&listEvent);
}

void LSG_List::SetItem(int row, const std::string& item)
{
	if (item.empty())
		return;

	auto rowOffset = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);

	if ((rowOffset < 0) || (rowOffset >= (int)this->pageItems.size()))
		return;

	this->pageItems[rowOffset] = item;
	this->SetItems(this->pageItems);
}

void LSG_List::SetItems(const LSG_Strings& items)
{
	this->page      = 0;
	this->pageItems = items;

	if (!this->sortOrder.empty())
		this->sort();

	this->updatePage();
}

void LSG_List::SetItems()
{
	if (!this->rows.empty() && !this->text.empty() && this->texture && !this->hasChanged())
	{
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;

		for (auto& row : this->rows)
			row.background = {};

		return;
	}

	if (this->rows.empty() || this->text.empty())
	{
		LSG_Strings items;

		for (auto child : this->children)
		{
			auto item = child->GetXmlValue();

			if (!item.empty())
				items.push_back(item);
		}

		this->SetItems(items);
	} else {
		this->updatePage(false);
	}
}

void LSG_List::SetPage(int page)
{
	if (this->navigate(page))
		this->updatePage();
}

void LSG_List::setRowHeights(int rowHeight, const SDL_Rect& backgroundArea)
{
	if ((rowHeight < 1) || SDL_RectEmpty(&backgroundArea) || this->rows.empty() || (this->rows[0].background.h > 0))
		return;

	for (int i = 0; i < (int)this->rows.size(); i++)
	{
		this->rows[i].background    = SDL_Rect(backgroundArea);
		this->rows[i].background.y += (i * rowHeight);
		this->rows[i].background.h  = rowHeight;
	}
}

void LSG_List::Sort(LSG_SortOrder sortOrder)
{
	if (sortOrder == this->GetSortOrder())
		return;

	this->sortOrder = LSG_ConstSortOrder::ToString(sortOrder);

	this->sort();
	this->updatePage();
}

void LSG_List::sort()
{
	if (this->sortOrder == LSG_ConstSortOrder::Descending)
		std::sort(this->pageItems.rbegin(), this->pageItems.rend(), LSG_Text::GetStringCompare);
	else
		std::sort(this->pageItems.begin(), this->pageItems.end(), LSG_Text::GetStringCompare);
}

void LSG_List::updatePage(bool reset)
{
	if (reset)
	{
		this->row           = -1;
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;
	}

	this->rows.clear();
	this->text = "";

	this->destroyTextures();

	if (this->showPagination())
	{
		auto listBackground = this->getFillArea(this->background, this->border);
		listBackground.h   -= LSG_ConstScrollBar::Width;

		this->initPagination(listBackground, this->backgroundColor);
	}

	auto pageItems = this->getPageItems();

	for (const auto& item : pageItems)
	{
		if (item.empty())
			continue;

		this->text.append(item).append("\n");
		this->rows.push_back({ .item = item });
	}

	if (this->rows.empty() || this->text.empty())
		return;

	this->SetText(this->text);

	if (this->row > 0)
		this->Select(this->row);
	else
		this->SelectFirstRow();
}
