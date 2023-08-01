#include "LSG_List.h"

LSG_List::LSG_List(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->groups      = {};
	this->header      = {};
	this->orientation = LSG_VERTICAL;
	this->row         = -1;
	this->rows        = {};
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

	this->row = -1;

	LSG_Strings items;

	for (const auto& row : this->rows)
		items.push_back(row.text);

	items.push_back(item);

	this->SetItems(items);
}

int LSG_List::getFirstRow()
{
	if (!this->header.columns.empty())
		return 1;

	return 0;
}

std::string LSG_List::GetItem(int row)
{
	if (!this->rows.empty() && (row >= 0) && (row < (int)this->rows.size()))
		return this->rows[row].text;

	return "";
}

LSG_Strings LSG_List::GetItems()
{
	LSG_Strings items;

	for (const auto& row : this->rows)
		items.push_back(row.text);

	return items;
}

int LSG_List::getLastRow()
{
	auto firstRow   = this->getFirstRow();
	auto lastRow    = (int)(this->rows.size() + firstRow - 1);
	auto groupCount = (int)this->groups.size();

	if (groupCount > 0)
		lastRow += groupCount;

	return lastRow;
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
	 return (this->sortOrder == LSG_DESCENDING ? LSG_SORT_ORDER_DESCENDING : LSG_SORT_ORDER_ASCENDING);
}

bool LSG_List::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->rows.empty())
		return false;

	auto positionY = (event.y - this->background.y + this->scrollOffsetY);
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
	if (this->rows.empty() || (row < 0) || (row >= (int)this->rows.size()))
		return;

	this->row = -1;

	LSG_Strings items;

	for (int i = 0; i < (int)this->rows.size(); i++) {
		if (i != row)
			items.push_back(this->rows[i].text);
	}

	this->SetItems(items);
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

	this->setRowHeights(rowHeight, listBackground);
	this->renderTexture(renderer, listBackground);

	if (this->showRowBorder)
		this->renderRowBorder(renderer, rowHeight, listBackground);

	this->renderHighlightSelection(renderer, listBackground);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, listBackground, size.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, listBackground, size.height, this->backgroundColor, this->highlighted);

	if (!this->enabled)
		this->renderDisabledOverlay(renderer);
}

void LSG_List::renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& backgroundArea)
{
	if (this->rows.empty() && this->groups.empty())
		return;

	auto row = SDL_Rect(!this->rows.empty() ? this->rows[0].background : this->groups[0].background);

	row.y = (backgroundArea.y + (this->row * row.h));

	if (this->showScrollY)
		row.w -= LSG_SCROLL_WIDTH;

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

	auto header = (this->getFirstRow() * rowHeight);

	if (!this->showScrollY)
	{
		auto rows = ((backgroundArea.h - header - (this->showScrollX ? LSG_SCROLL_WIDTH : 0)) / rowHeight);
		auto y    = (backgroundArea.y + header + rowHeight - 1);
		auto x2   = (backgroundArea.x + backgroundArea.w);

		for (int i = 0; i < rows; i++) {
			SDL_RenderDrawLine(renderer, backgroundArea.x, y, x2, y);
			y += rowHeight;
		}

		return;
	}

	auto topY    = (backgroundArea.y + header);
	auto bottomY = (backgroundArea.y + backgroundArea.h - (this->showScrollX ? LSG_SCROLL_WIDTH : 0));

	for (const auto& row : this->rows)
	{
		auto y  = (row.background.y + row.background.h - 1 - this->scrollOffsetY);
		auto x2 = (row.background.x + row.background.w - (this->showScrollY ? LSG_SCROLL_WIDTH : 0));

		if ((y >= topY) && (y <= bottomY))
			SDL_RenderDrawLine(renderer, row.background.x, y, x2, y);
	}
}

bool LSG_List::Select(int row)
{
	if (!this->enabled || (this->rows.empty() && this->groups.empty()))
		return false;

	auto firstRow = this->getFirstRow();
	auto lastRow  = this->getLastRow();

	if ((row < firstRow) || (row > lastRow))
		return false;

	if (row == this->row)
		return true;

	this->row = row;

	this->sendEvent(LSG_EVENT_ROW_SELECTED);

	return true;
}

void LSG_List::SelectFirstRow()
{
	if (!this->enabled || (this->rows.empty() && this->groups.empty()))
		return;

	auto firstRow = this->getFirstRow();

	this->Select(firstRow);
	this->ScrollHome();
}

void LSG_List::SelectLastRow()
{
	if (!this->enabled || (this->rows.empty() && this->groups.empty()))
		return;

	auto lastRow = this->getLastRow();

	this->Select(lastRow);
	this->ScrollEnd();
}

void LSG_List::SelectRow(int offset)
{
	if (!this->enabled || (this->rows.empty() && this->groups.empty()))
		return;

	auto firstRow    = this->getFirstRow();
	auto lastRow     = this->getLastRow();
	auto selectedRow = max(firstRow, min(lastRow, (this->row + offset)));

	this->Select(selectedRow);

	if (!this->showScrollY)
		return;

	auto list           = this->getFillArea(this->background, this->border);
	auto row            = this->rows[0].background;
	auto rowY           = (list.y + (this->row * row.h));
	auto firstRowHeight = (firstRow > 0 ? row.h : 0);
	auto scrollWidth    = (this->showScrollX ? LSG_SCROLL_WIDTH : 0);
	auto listTop        = (list.y + this->scrollOffsetY + firstRowHeight);
	auto listBottom     = (listTop + list.h - scrollWidth - firstRowHeight);

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

void LSG_List::setRowHeights(int rowHeight, const SDL_Rect& backgroundArea)
{
	if ((rowHeight < 1) || SDL_RectEmpty(&backgroundArea))
		return;

	auto firstRow = this->getFirstRow();

	if ((firstRow > 0) && (this->header.background.h < 1)) {
		auto headers = std::vector<LSG_ListRow>{ this->header };
		this->setRowHeights(headers, rowHeight, backgroundArea);
	}

	if (!this->groups.empty() && (this->groups[0].background.h < 1))
		this->setRowHeights(this->groups, rowHeight, backgroundArea);

	if (!this->rows.empty() && (this->rows[0].background.h < 1))
		this->setRowHeights(this->rows, rowHeight, backgroundArea);
}

void LSG_List::setRowHeights(std::vector<LSG_ListRow>& rows, int rowHeight, const SDL_Rect& backgroundArea)
{
	for (auto& row : rows)
	{
		row.background    = SDL_Rect(backgroundArea);
		row.background.y += (row.index * rowHeight);
		row.background.h  = rowHeight;
	}
}

void LSG_List::SetItem(int row, const std::string& item)
{
	if (item.empty() || this->rows.empty() || (row < 0) || (row >= (int)this->rows.size()))
		return;

	LSG_Strings items;

	this->rows[row].text = item;

	for (const auto& row : this->rows)
		items.push_back(row.text);

	this->SetItems(items);
}

void LSG_List::SetItems(const LSG_Strings& items)
{
	this->row           = -1;
	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;

	this->rows.clear();
	this->text = "";

	this->destroyTextures();

	if (items.empty())
		return;

	auto listItems = (!this->sortOrder.empty() ? this->sort(items) : items);

	for (int i = 0; i < (int)listItems.size(); i++)
	{
		if (listItems[i].empty())
			continue;

		this->text.append(listItems[i]).append("\n");

		this->rows.push_back({ .index = i, .text = listItems[i] });
	}

	if (this->rows.empty() || this->text.empty())
		return;

	this->SetText(this->text);

	auto firstRow = this->getFirstRow();

	if (this->row < firstRow)
		this->Select(firstRow);
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
		this->rows.clear();
		this->text = "";

		LSG_Strings items;

		for (auto child : this->children)
		{
			auto item = child->GetXmlValue();

			if (!item.empty())
				items.push_back(item);
		}

		auto listItems = (!this->sortOrder.empty() ? this->sort(items) : items);

		for (int i = 0; i < (int)listItems.size(); i++)
		{
			this->text.append(listItems[i]).append("\n");
			this->rows.push_back({ .index = i, .text = listItems[i]});
		}
	}

	if (this->rows.empty() || this->text.empty())
		return;

	this->SetText();

	if (this->row < 0)
		this->Select(this->getFirstRow());
}

void LSG_List::Sort(LSG_SortOrder sortOrder)
{
	this->row       = -1;
	this->sortOrder = (sortOrder == LSG_SORT_ORDER_DESCENDING ? LSG_DESCENDING : LSG_ASCENDING);

	LSG_Strings items;

	for (const auto& row : this->rows)
		items.push_back(row.text);

	this->SetItems(items);
}

LSG_Strings LSG_List::sort(const LSG_Strings& items)
{
	auto compare = [](const std::string& s1, const std::string& s2)
	{
		return std::lexicographical_compare(
			s1.begin(), s1.end(),
			s2.begin(), s2.end(),
			[](const char& c1, const char& c2) {
				return std::tolower(c1) < std::tolower(c2);
			}
		);
	};

	LSG_Strings listItems = items;

	if (this->sortOrder == LSG_DESCENDING)
		std::sort(listItems.rbegin(), listItems.rend(), compare);
	else
		std::sort(listItems.begin(), listItems.end(), compare);

	return listItems;
}
