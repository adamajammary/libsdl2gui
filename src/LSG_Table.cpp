#include "LSG_Table.h"

LSG_Table::LSG_Table(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_List(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->groups      = {};
	this->header      = {};
	this->rows        = {};
	this->orientation = LSG_ConstOrientation::Vertical;
	this->row         = -1;
	this->sortColumn  = -1;
	this->sortOrder   = "";
	this->textColumns = {};
	this->wrap        = true;

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("sort"))
		this->sortOrder = attributes["sort"];

	if (attributes.contains("sort-column"))
		this->sortColumn = std::atoi(attributes["sort-column"].c_str());
}

void LSG_Table::AddGroup(const std::string& group, const LSG_TableRows& rows)
{
	if (group.empty() || rows.empty())
		return;

	this->pageGroups.push_back({ .group = group, .rows = rows });

	this->SetGroups(this->pageGroups);
}

void LSG_Table::AddRow(const LSG_Strings& columns)
{
	if (columns.empty())
		return;

	this->pageRows.push_back(columns);

	this->SetRows(this->pageRows);
}

int LSG_Table::getClickedHeaderColumn(const SDL_Point& mousePosition, const std::vector<SDL_Size>& columnSizes, int maxWidth)
{
	auto column           = SDL_Rect();
	auto remainingX       = maxWidth;
	auto remainingOffsetX = this->scrollOffsetX;

	for (int i = 0; i < (int)this->textures.size(); i++)
	{
		auto textureWidth = (columnSizes[i].width + LSG_Const::TableColumnSpacing);

		column.w = std::max(0, std::min((textureWidth - std::max(0, remainingOffsetX)), remainingX));
		column.h = columnSizes[i].height;

		if (SDL_PointInRect(&mousePosition, &column))
			return i;

		column.x         += column.w;
		remainingX       -= column.w;
		remainingOffsetX -= textureWidth;
	}

	return -1;
}

int LSG_Table::getColumnCount()
{
	auto columns = (int)this->pageHeader.size();

	if (!this->groups.empty() && !this->groups[0].rows.empty() && (int)this->groups[0].rows[0].columns.size() > columns)
		columns = (int)this->groups[0].rows[0].columns.size();

	if (!this->rows.empty() && (int)this->rows[0].columns.size() > columns)
		columns = (int)this->rows[0].columns.size();

	return columns;
}

int LSG_Table::getFirstRow()
{
	return (!this->pageHeader.empty() ? 1 : 0);
}

int LSG_Table::getLastRow()
{
	auto firstRow = this->getFirstRow();

	int rows = firstRow;

	for (const auto& group : this->groups)
		rows += (int)((!group.group.empty() ? 1 : 0) + group.rows.size());

	rows += (int)this->rows.size();

	return (int)(rows - 1);
}

LSG_Strings LSG_Table::GetRow(int row)
{
	if (!this->header.columns.empty() && (row == 0))
		return this->header.columns;

	int i = (this->getFirstRow() - 1);

	for (const auto& group : this->groups)
	{
		if (!group.group.empty() && ((++i) == row))
			return { group.group };

		for (const auto& r : group.rows) {
			if ((++i) == row)
				return r.columns;
		}
	}

	for (const auto& r : this->rows) {
		if ((++i) == row)
			return r.columns;
	}

	return {};
}

LSG_TableRows LSG_Table::GetRows()
{
	LSG_TableRows rows;

	if (!this->header.columns.empty())
		rows.push_back(this->header.columns);

	for (const auto& group : this->groups)
	{
		if (!group.group.empty())
			rows.push_back({ group.group });

		for (const auto& row : group.rows)
			rows.push_back(row.columns);
	}

	for (const auto& row : this->rows)
		rows.push_back(row.columns);

	return rows;
}

int LSG_Table::getRowHeight()
{
	int rowHeight = (!this->rows.empty() ? this->rows[0].background.h : 0);

	if ((rowHeight < 1) && !this->groups.empty() && !this->groups[0].rows.empty())
		rowHeight = this->groups[0].rows[0].background.h;

	if (rowHeight > 0)
		return rowHeight;

	auto textureSize = this->getTextureSizes(LSG_Const::TableColumnSpacing);

	return this->getRowHeight(textureSize.totalSize);
}

int LSG_Table::getRowHeight(const SDL_Size& textureSize)
{
	auto lastRow = this->getLastRow();

	return (lastRow >= 0 ? (textureSize.height / (lastRow + 1)) : 0);
}

int LSG_Table::GetSortColumn()
{
	return this->sortColumn;
}

bool LSG_Table::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled || LSG_Events::IsMouseDown())
		return false;

	SDL_Point mousePosition = { event.x, event.y };

	if (this->isPaginationClicked(mousePosition))
	{
		if (this->isPageArrowClicked(mousePosition))
			this->updatePage();

		return true;
	}

	auto background  = this->getFillArea(this->background, this->border);
	auto positionY   = (mousePosition.y - background.y);
	auto textureSize = this->getTextureSizes(LSG_Const::TableColumnSpacing);
	auto rowHeight   = this->getRowHeight();

	if (rowHeight < 1)
		return false;

	bool isDoubleClick = LSG_Events::IsDoubleClick(event);
	auto noScrollRow   = (positionY / rowHeight);

	if (!this->header.columns.empty() && (noScrollRow == 0))
	{
		if (isDoubleClick)
			return false;

		auto      positionX  = (mousePosition.x - background.x);
		SDL_Point position   = { positionX, positionY };
		auto      sortColumn = this->getClickedHeaderColumn(position, textureSize.sizes, background.w);

		if (sortColumn < 0)
			return false;

		if ((this->sortOrder == LSG_ConstSortOrder::Ascending) && (this->sortColumn == sortColumn))
			this->Sort(LSG_SORT_ORDER_DESCENDING, sortColumn);
		else
			this->Sort(LSG_SORT_ORDER_ASCENDING, sortColumn);

		return true;
	}

	auto scrolledRow = ((positionY + this->scrollOffsetY) / rowHeight);

	bool selected = this->Select(scrolledRow);

	if (selected && isDoubleClick)
		this->sendEvent(LSG_EVENT_ROW_ACTIVATED);

	return true;
}

void LSG_Table::RemoveHeader()
{
	auto firstRow = this->getFirstRow();

	if (firstRow < 1)
		return;

	this->pageHeader = {};

	this->updatePage();
}

void LSG_Table::RemoveGroup(const std::string& group)
{
	for (auto iter = this->pageGroups.begin(); iter != this->pageGroups.end(); iter++)
	{
		if ((*iter).group == group) {
			this->pageGroups.erase(iter);
			break;
		}
	}

	this->SetGroups(this->pageGroups);
}

void LSG_Table::RemoveRow(int row)
{
	if (!this->header.columns.empty() && (row == 0)) {
		this->RemoveHeader();
		return;
	}

	auto rowOffset = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);
	int  i         = (this->getFirstRow() - 1);

	for (auto g = this->pageGroups.begin(); g != this->pageGroups.end(); g++)
	{
		if (++i == rowOffset) {
			this->pageGroups.erase(g);
			this->SetGroups(this->pageGroups);
			return;
		}

		for (auto r = (*g).rows.begin(); r != (*g).rows.end(); r++)
		{
			if (++i == rowOffset) {
				(*g).rows.erase(r);
				this->SetGroups(this->pageGroups);
				return;
			}
		}
	}

	for (auto r = this->pageRows.begin(); r != this->pageRows.end(); r++)
	{
		if (++i == rowOffset) {
			this->pageRows.erase(r);
			this->SetRows(this->pageRows);
			return;
		}
	}
}

void LSG_Table::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (this->textures.empty())
		return;

	auto tableBackground = this->getFillArea(this->background, this->border);
	bool showPagination  = this->showPagination();

	if (showPagination)
		tableBackground.h -= LSG_ConstScrollBar::Width;

	auto rowHeight = this->getRowHeight();

	this->setRowHeights(tableBackground, rowHeight);

	this->renderTextures(renderer, tableBackground, rowHeight);

	if (this->showRowBorder)
		this->renderRowBorder(renderer, tableBackground, rowHeight);

	if (showPagination)
		this->renderPagination(renderer, tableBackground);
}

void LSG_Table::renderFillHeader(SDL_Renderer* renderer, const SDL_Rect& renderDestination, int rowHeight)
{
	SDL_Rect headerFillDest = renderDestination;

	headerFillDest.h = rowHeight;

	auto fillColor = LSG_Graphics::GetOffsetColor(this->backgroundColor, 30);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 255);
	SDL_RenderFillRect(renderer, &headerFillDest);
}

void LSG_Table::renderRowBorder(SDL_Renderer* renderer, const SDL_Rect& backgroundArea, int rowHeight)
{
	if (rowHeight < 1)
		return;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 64);

	auto header = (this->getFirstRow() * rowHeight);

	if (!this->showScrollY)
	{
		auto rows = ((backgroundArea.h - header - (this->showScrollX ? LSG_ConstScrollBar::Width : 0)) / rowHeight);
		auto y    = (backgroundArea.y + header + rowHeight - 1);
		auto x2   = (backgroundArea.x + backgroundArea.w);

		for (int i = 0; i < rows; i++) {
			SDL_RenderDrawLine(renderer, backgroundArea.x, y, x2, y);
			y += rowHeight;
		}

		return;
	}

	auto topY    = (backgroundArea.y + header);
	auto bottomY = (backgroundArea.y + backgroundArea.h - (this->showScrollX ? LSG_ConstScrollBar::Width : 0));

	for (const auto& row : this->rows)
	{
		auto y  = (row.background.y + row.background.h - 1 - this->scrollOffsetY);
		auto x2 = (row.background.x + row.background.w - (this->showScrollY ? LSG_ConstScrollBar::Width : 0));

		if ((y >= topY) && (y <= bottomY))
			SDL_RenderDrawLine(renderer, row.background.x, y, x2, y);
	}
}

void LSG_Table::renderRowTextures(SDL_Renderer* renderer, int backgroundWidth, const std::vector<SDL_Size>& columnSizes, const SDL_Rect& renderClip, const SDL_Rect& renderDestination)
{
	SDL_Rect clip             = renderClip;
	SDL_Rect destination      = renderDestination;
	auto     remainingX       = backgroundWidth;
	auto     remainingOffsetX = this->scrollOffsetX;

	for (size_t i = 0; i < this->textures.size(); i++)
	{
		clip.x = std::max(0, remainingOffsetX);
		clip.w = std::max(0, std::min((columnSizes[i].width - clip.x), remainingX));

		destination.w = clip.w;
		destination.h = clip.h;

		SDL_RenderCopy(renderer, this->textures[i], &clip, &destination);

		auto columnSpacing = (clip.w > 0 ? LSG_Const::TableColumnSpacing : 0);

		destination.x    += (clip.w + columnSpacing);
		remainingX       -= (clip.w + columnSpacing);
		remainingOffsetX -= columnSizes[i].width;
	}
}

void LSG_Table::renderTextures(SDL_Renderer* renderer, SDL_Rect& backgroundArea, int rowHeight)
{
	auto textureSize = this->getTextureSizes(LSG_Const::TableColumnSpacing);
	auto size        = textureSize.totalSize;

	SDL_Rect background = backgroundArea;

	if (this->enableScroll)
	{
		bool showScrollY = (size.height > backgroundArea.h);
		bool showScrollX = (size.width  > backgroundArea.w);

		if (showScrollY)
			background.w -= LSG_ConstScrollBar::Width;

		if (showScrollX)
			background.h -= LSG_ConstScrollBar::Width;

		this->showScrollY = (size.height > background.h);
		this->showScrollX = (size.width > background.w);

		if (this->showScrollY && !showScrollY)
			background.w -= LSG_ConstScrollBar::Width;

		if (this->showScrollX && !showScrollX)
			background.h -= LSG_ConstScrollBar::Width;
	}

	SDL_Rect clip = { 0, 0, std::min(size.width, background.w), std::min(size.height, background.h) };
	auto     dest = this->getRenderDestinationAligned(background, size);

	if (this->showScrollX)
	{
		auto maxScrollOffsetX = (size.width - clip.w);

		if (this->scrollOffsetX > maxScrollOffsetX)
			this->scrollOffsetX = maxScrollOffsetX;

		clip.x += this->scrollOffsetX;
	} else {
		this->scrollOffsetX = 0;
	}

	if (this->showScrollY)
	{
		auto maxScrollOffsetY = (size.height - clip.h);

		if (this->scrollOffsetY > maxScrollOffsetY)
			this->scrollOffsetY = maxScrollOffsetY;

		clip.y += this->scrollOffsetY;
	} else {
		this->scrollOffsetY = 0;
	}

	auto rowClip  = SDL_Rect(clip);
	auto rowDest  = SDL_Rect(dest);
	auto firstRow = this->getFirstRow();

	if (firstRow > 0) {
		rowClip.y += rowHeight;
		rowClip.h -= rowHeight;
		rowDest.y += rowHeight;
	}

	this->renderRowTextures(renderer, background.w, textureSize.sizes, rowClip, rowDest);

	if (!this->groups.empty() && !this->groups[0].rows.empty())
		this->renderHighlightSelection(renderer, backgroundArea, this->groups[0].rows[0].background);
	else if (!this->rows.empty())
		this->renderHighlightSelection(renderer, backgroundArea, this->rows[0].background);

	if (firstRow > 0)
	{
		this->renderFillHeader(renderer, background, rowHeight);

		auto headerClip = SDL_Rect(clip);

		headerClip.y = 0;
		headerClip.h = rowHeight;

		this->renderRowTextures(renderer, background.w, textureSize.sizes, headerClip, dest);
	}

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, backgroundArea, size.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, backgroundArea, size.height, this->backgroundColor, this->highlighted);
}

bool LSG_Table::Select(int row)
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

void LSG_Table::SelectFirstRow()
{
	if (!this->enabled || (this->rows.empty() && this->groups.empty()))
		return;

	this->Select(this->getFirstRow());
	this->ScrollHome();
}

void LSG_Table::SelectLastRow()
{
	if (!this->enabled || (this->rows.empty() && this->groups.empty()))
		return;

	this->Select(this->getLastRow());
	this->ScrollEnd();
}

void LSG_Table::SelectRow(int offset)
{
	if (!this->enabled || (this->rows.empty() && this->groups.empty()))
		return;

	auto firstRow    = this->getFirstRow();
	auto selectedRow = std::max(firstRow, std::min(this->getLastRow(), (this->row + offset)));

	this->Select(selectedRow);

	auto list           = this->getFillArea(this->background, this->border);
	auto rowHeight      = this->getRowHeight();
	auto rowY           = (list.y + (this->row * rowHeight));
	auto firstRowHeight = (firstRow > 0 ? rowHeight : 0);
	auto pagination     = (this->showPagination() ? LSG_ConstScrollBar::Width : 0);
	auto scrollX        = (this->showScrollX ? LSG_ConstScrollBar::Width : 0);
	auto listTop        = (list.y + firstRowHeight + this->scrollOffsetY);
	auto listBottom     = (listTop + list.h - firstRowHeight - scrollX - pagination);

	if ((rowY + rowHeight) >= listBottom)
		this->ScrollVertical(std::abs(offset) * rowHeight);
	else if (rowY < listTop)
		this->ScrollVertical(std::abs(offset) * -rowHeight);
}

void LSG_Table::SetGroups(const LSG_TableGroups& groups)
{
	this->page       = 0;
	this->pageGroups = groups;

	if (!this->sortOrder.empty())
		this->sort();

	this->updatePage();
}

void LSG_Table::SetHeader(const LSG_Strings& header)
{
	this->pageHeader = header;

	this->updatePage();
}

void LSG_Table::SetPage(int page)
{
	if (this->navigate(page))
		this->updatePage();
}

void LSG_Table::setPageItems()
{
	this->header = { .columns = this->pageHeader };

	auto start = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end   = (start + LSG_MAX_ROWS_PER_PAGE - 1);
	int  i     = -1;

	for (const auto& group : this->pageGroups)
	{
		LSG_TableItemGroup itemGroup;

		if ((++i) >= start)
		{
			itemGroup.group = group.group;

			if (i >= end) {
				this->groups.push_back(itemGroup);
				break;
			}
		}

		for (const auto& columns : group.rows)
		{
			if ((++i) >= start)
				itemGroup.rows.push_back({ .columns = columns });

			if (i >= end)
				break;
		}

		if (i >= start)
			this->groups.push_back(itemGroup);

		if (i >= end)
			break;
	}

	if (i >= end)
		return;

	for (const auto& columns : this->pageRows)
	{
		if ((++i) < start)
			continue;

		this->rows.push_back({ .columns = columns });

		if (i >= end)
			break;
	}
}

void LSG_Table::setRowHeights(const SDL_Rect& backgroundArea, int rowHeight)
{
	if ((rowHeight < 1) || SDL_RectEmpty(&backgroundArea))
		return;

	int i = 0;

	if ((this->getFirstRow() > 0) && (this->header.background.h < 1)) {
		LSG_TableItemRows headers = { this->header };
		this->setRowHeights(headers, backgroundArea, rowHeight, i);
	}

	if (!this->groups.empty() && !this->groups[0].rows.empty() && (this->groups[0].rows[0].background.h < 1)) {
		for (auto& group : this->groups)
			this->setRowHeights(group.rows, backgroundArea, rowHeight, ++i);
	}

	if (!this->rows.empty() && (this->rows[0].background.h < 1))
		this->setRowHeights(this->rows, backgroundArea, rowHeight, i);
}

void LSG_Table::setRowHeights(LSG_TableItemRows& rows, const SDL_Rect& backgroundArea, int rowHeight, int& i)
{
	for (auto& row : rows)
	{
		row.background    = SDL_Rect(backgroundArea);
		row.background.y += (i * rowHeight);
		row.background.h  = rowHeight;

		i++;
	}
}

void LSG_Table::SetRow(int row, const LSG_Strings& columns)
{
	if (columns.empty())
		return;

	if (!this->header.columns.empty() && (row == 0)) {
		this->SetHeader(columns);
		return;
	}

	auto rowOffset = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);
	int  i         = (this->getFirstRow() - 1);

	for (auto& group : this->pageGroups)
	{
		if (++i == rowOffset) {
			group.group = columns[0];
			this->SetGroups(this->pageGroups);
			return;
		}

		for (auto& r : group.rows)
		{
			if (++i == rowOffset) {
				r = columns;
				this->SetGroups(this->pageGroups);
				return;
			}
		}
	}

	for (auto& r : this->pageRows)
	{
		if (++i == rowOffset) {
			r = columns;
			this->SetRows(this->pageRows);
			return;
		}
	}
}

void LSG_Table::SetRows(const LSG_TableRows& rows)
{
	this->page     = 0;
	this->pageRows = rows;

	if (!this->sortOrder.empty())
		this->sort();

	this->updatePage();
}

void LSG_Table::SetRows()
{
	if ((!this->groups.empty() || !this->rows.empty()) && !this->textColumns.empty() && !this->textures.empty() && !this->hasChanged())
	{
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;

		this->header.background = {};

		for (auto& group : this->groups) {
			for (auto& row : group.rows)
				row.background = {};
		}

		for (auto& row : this->rows)
			row.background = {};

		return;
	}

	if ((this->groups.empty() && this->rows.empty()) || this->textColumns.empty())
	{
		for (auto child : this->children)
		{
			auto children = child->GetChildren();

			if (children.empty())
				break;

			if (!child->IsTableGroup())
			{
				LSG_Strings columns;

				for (auto column : children)
					columns.push_back(column->GetXmlValue());

				if (child->IsTableHeader())
					this->pageHeader = columns;
				else if (child->IsTableRow())
					this->pageRows.push_back(columns);

				continue;
			}

			LSG_TableGroupRows group;

			group.group = child->GetXmlAttribute("group");

			for (auto row : children)
			{
				LSG_Strings columns;

				for (auto column : row->GetChildren())
					columns.push_back(column->GetXmlValue());

				group.rows.push_back(columns);
			}

			this->pageGroups.push_back(group);
		}

		if (!this->sortOrder.empty())
			this->sort();

		this->updatePage();
	} else {
		this->updatePage(false);
	}
}

void LSG_Table::Sort(LSG_SortOrder sortOrder, int sortColumn)
{
	if ((sortOrder == this->GetSortOrder()) && (sortColumn == this->sortColumn))
		return;

	this->sortColumn = sortColumn;
	this->sortOrder  = LSG_ConstSortOrder::ToString(sortOrder);

	this->sort();
	this->updatePage();
}

void LSG_Table::sort()
{
	auto sortColumn     = std::max(0, this->sortColumn);
	auto stringsCompare = LSG_Text::GetStringsCompare(sortColumn);

	if (this->sortOrder == LSG_ConstSortOrder::Descending)
	{
		for (auto& group : this->pageGroups)
			std::sort(group.rows.rbegin(), group.rows.rend(), stringsCompare);

		std::sort(this->pageRows.rbegin(), this->pageRows.rend(), stringsCompare);
	}
	else
	{
		for (auto& group : this->pageGroups)
			std::sort(group.rows.begin(), group.rows.end(), stringsCompare);

		std::sort(this->pageRows.begin(), this->pageRows.end(), stringsCompare);
	}
}

void LSG_Table::updatePage(bool reset)
{
	if (reset)
	{
		this->row           = -1;
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;
	}

	this->groups.clear();
	this->rows.clear();
	this->header = {};
	this->text   = "";

	this->destroyTextures();

	if (this->showPagination())
	{
		auto tableBackground = this->getFillArea(this->background, this->border);
		tableBackground.h   -= LSG_ConstScrollBar::Width;

		this->initPagination(tableBackground, this->backgroundColor);
	}

	this->setPageItems();

	auto columnCount  = this->getColumnCount();
	this->textColumns = LSG_Strings(columnCount, "");

	for (int i = 0; i < (int)this->header.columns.size(); i++)
	{
		if ((i == sortColumn) && (this->sortOrder == LSG_ConstSortOrder::Ascending))
			this->textColumns[i] = LSG_Text::Format("%s%s\n", LSG_ConstUnicodeCharacter::ArrowUp, this->header.columns[i].c_str());
		else if ((i == sortColumn) && (this->sortOrder == LSG_ConstSortOrder::Descending))
			this->textColumns[i] = LSG_Text::Format("%s%s\n", LSG_ConstUnicodeCharacter::ArrowDown, this->header.columns[i].c_str());
		else
			this->textColumns[i] = LSG_Text::Format("%s\n", this->header.columns[i].c_str());
	}

	for (const auto& group : this->groups)
	{
		if (!group.group.empty())
		{
			this->textColumns[0].append(LSG_Text::Format("%s\n", group.group.c_str()));

			for (int i = 1; i < columnCount; i++)
				this->textColumns[i].append("\n");
		}

		for (const auto& row : group.rows)
		{
			this->textColumns[0].append(LSG_Text::Format("   %s\n", row.columns[0].c_str()));

			for (int i = 1; i < (int)row.columns.size(); i++)
				this->textColumns[i].append(LSG_Text::Format("%s\n", row.columns[i].c_str()));
		}
	}

	for (const auto& row : this->rows) {
		for (int j = 0; j < (int)row.columns.size(); j++)
			this->textColumns[j].append(LSG_Text::Format("%s\n", row.columns[j].c_str()));
	}

	if (this->textColumns.empty())
		return;

	for (const auto& textColumn : this->textColumns)
		this->textures.push_back(this->getTexture(textColumn));

	if (this->row > 0)
		this->Select(this->row);
	else
		this->SelectFirstRow();
}
