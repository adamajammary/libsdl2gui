#include "LSG_List.h"

LSG_List::LSG_List(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->orientation = LSG_ConstOrientation::Vertical;
	this->row         = -1;
	this->wrap        = true;
}

void LSG_List::Activate()
{
	this->sendEvent(LSG_EVENT_ROW_ACTIVATED);
}

/**
 * @throws runtime_error
 */
LSG_Component* LSG_List::AddItem(const std::string& item, bool reset)
{
	if (item.empty())
		return nullptr;

	auto itemNode = LSG_XML::AddChildNode(this->xmlNode, "list-item");

	if (!itemNode) {
		auto error = LibXml::xmlGetLastError();
		throw std::runtime_error(LSG_Text::Format("Failed to add list item: %s", (error ? error->message : "")));
	}

	auto itemComponent = LSG_UI::AddXmlNode(itemNode, this);

	itemComponent->text = item;

	if (reset)
		this->reset();

	return itemComponent;
}

int LSG_List::getRowHeight()
{
	auto header = (this->pageHeader ? 1 : 0);
	auto rows   = (this->getLastRow() + header + 1);

	if (rows > 0)
		return (this->getTextureHeight() / rows);

	return 0;
}

int LSG_List::GetSelectedRow() const
{
	return this->row;
}

LSG_SortOrder LSG_List::GetSortOrder()
{
	return LSG_ConstSortOrder::ToEnum(this->GetXmlAttribute("sort"));
}

bool LSG_List::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->children.empty())
		return false;

	if (this->isPaginationClicked(mousePosition))
	{
		if (this->isPageArrowClicked(mousePosition)) {
			this->Update();
			this->SelectFirstRow();
		}

		return true;
	}

	auto background = this->getFillArea(this->background, this->border);
	auto positionY  = (mousePosition.y - background.y + this->scrollOffsetY);
	auto rowHeight  = this->getRowHeight();

	if (rowHeight < 1)
		return false;

	this->Select(positionY / rowHeight);

	return true;
}

void LSG_List::removeItem(int row, int lastRow)
{
	if ((row < 0) || (row > lastRow))
		return;

	auto itemIter = (this->pageItems.begin() + (size_t)row);

	if ((row >= (int)this->pageItems.size()) || (itemIter == this->pageItems.end()))
		return;

	LSG_UI::RemoveXmlNode(*itemIter);

	this->pageItems.erase(itemIter);

	this->Update();

	lastRow = this->getLastRow();

	if (lastRow < 0)
	{
		this->page          = 0;
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;

		this->Select(-1);
	} else if (this->row > lastRow) {
		this->SelectLastRow();
	}
}

void LSG_List::RemoveItem(int row)
{
	auto lastRow = (int)(this->pageItems.size() - 1);

	this->removeItem(row, lastRow);
}

void LSG_List::RemovePageItem(int row)
{
	auto rowIndex = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);

	this->removeItem(rowIndex, this->getLastRow());
}

void LSG_List::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto background      = this->getFillArea(this->background, this->border);
	auto rowHeight       = this->getRowHeight();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();
	bool showPagination  = this->showPagination();
	bool showRowBorder   = (this->GetXmlAttribute("row-border") == "true");
	auto textureSize     = this->getTextureSize();

	if (background.h < scrollBarSize2x)
		return;

	if (showPagination)
		background.h -= LSG_ScrollBar::GetSize();

	this->renderScrollableTexture(renderer, background, this->getAlignment(), this->texture, textureSize);

	if (showRowBorder)
		this->renderRowBorder(renderer, background, rowHeight);

	this->renderHighlightSelection(renderer, background, rowHeight);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, background, textureSize.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, background, textureSize.height, this->backgroundColor, this->highlighted);

	if (showPagination)
		this->renderPagination(renderer, background, this->backgroundColor);
}

void LSG_List::renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight)
{
	if (this->row < 0)
		return;

	auto header = (this->pageHeader ? rowHeight : 0);

	auto scrollSize  = LSG_ScrollBar::GetSize();
	auto scrollWidth = (this->showScrollY ? scrollSize : 0);

	SDL_Rect row = background;

	row.y += (header + (this->row * rowHeight) - this->scrollOffsetY);
	row.w -= scrollWidth;
	row.h  = rowHeight;

	auto topOffset    = ((row.y + rowHeight) - background.y);
	auto bottomOffset = ((background.y + background.h) - row.y);

	if ((topOffset <= 0) || (bottomOffset <= 0))
		return;

	if (topOffset < rowHeight) {
		row.y += (row.h - topOffset);
		row.h  = topOffset;
	} else if (bottomOffset < rowHeight) {
		row.h = bottomOffset;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, (255 - this->backgroundColor.r), (255 - this->backgroundColor.g), (255 - this->backgroundColor.b), 64);

	SDL_RenderFillRect(renderer, &row);
}

void LSG_List::renderRowBorder(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight)
{
	if (rowHeight < 1)
		return;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, (255 - this->backgroundColor.r), (255 - this->backgroundColor.g), (255 - this->backgroundColor.b), 64);

	auto scrollOffsetY = (this->showScrollY ? (this->scrollOffsetY % rowHeight) : 0);
	auto scrollSize    = LSG_ScrollBar::GetSize();
	auto scrollWidth   = (this->showScrollY ? scrollSize : 0);
	auto scrollHeight  = (this->showScrollX ? scrollSize : 0);

	auto header = (this->pageHeader ? rowHeight : 0);
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

	if (row == this->row)
		return true;

	this->row = row;

	this->sendEvent(row < 0 ? LSG_EVENT_ROW_UNSELECTED : LSG_EVENT_ROW_SELECTED);

	return true;
}

void LSG_List::SelectFirstRow()
{
	if (!this->enabled || (this->pageItems.empty() && this->pageGroups.empty() && this->pageRows.empty()))
		return;

	this->Select(0);
	this->OnScrollHome();
}

void LSG_List::SelectLastRow()
{
	if (!this->enabled || (this->pageItems.empty() && this->pageGroups.empty() && this->pageRows.empty()))
		return;

	this->Select(this->getLastRow());
	this->OnScrollEnd();
}

void LSG_List::SelectRow(int offset)
{
	if (!this->enabled || (this->pageItems.empty() && this->pageGroups.empty() && this->pageRows.empty()))
		return;

	auto nextRow = (this->row + offset);

	this->Select(std::max(0, std::min(this->getLastRow(), nextRow)));

	if (!this->showScrollY)
		return;

	auto background    = this->getFillArea(this->background, this->border);
	auto rowHeight     = this->getRowHeight();
	auto rowY          = (background.y + (this->row * rowHeight));
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
	listEvent.user.data2 = (void*)&this->row;

	SDL_PushEvent(&listEvent);
}

void LSG_List::SetItem(int row, const std::string& item)
{
	auto lastRow = (int)(this->pageItems.size() - 1);

	this->setItem(row, lastRow, item);
}

void LSG_List::setItem(int row, int lastRow, const std::string& item)
{
	if ((row < 0) || (row > lastRow) || (row >= (int)this->pageItems.size()))
		return;

	this->pageItems[row]->text = item;

	this->destroyTextures();
	this->setItems();
}

void LSG_List::SetItems(const LSG_Strings& items)
{
	this->page          = 0;
	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;

	this->Select(-1);

	this->pageItems.clear();

	this->destroyTextures();

	LSG_UI::RemoveXmlChildNodes(this);

	for (const auto& item : items)
	{
		auto itemComponent = this->AddItem(item, false);

		this->pageItems.push_back(itemComponent);
	}

	this->setItems();
}

void LSG_List::SetItems()
{
	if (this->texture && !this->hasChanged())
		return;

	this->pageItems.clear();

	this->destroyTextures();

	for (auto child : this->children)
		this->pageItems.push_back(child);

	this->setItems();
}

void LSG_List::Update()
{
	this->destroyTextures();
	this->setItems(false);
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

	this->Select(-1);
	this->reset();
}

void LSG_List::sort()
{
	if (this->GetXmlAttribute("sort") == LSG_ConstSortOrder::Descending)
		std::sort(this->pageItems.rbegin(), this->pageItems.rend(), LSG_Text::GetXmlValueCompare);
	else
		std::sort(this->pageItems.begin(), this->pageItems.end(), LSG_Text::GetXmlValueCompare);
}
