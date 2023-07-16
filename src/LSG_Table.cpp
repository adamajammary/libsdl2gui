#include "LSG_Table.h"

LSG_Table::LSG_Table(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_List(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->orientation = LSG_VERTICAL;
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

	auto lastIndex = (this->getLastRowIndex() + 1);

	auto groupRow = LSG_Strings(rows[0].size(), "");
	groupRow[0]   = group;

	this->groups.push_back({ .columns = groupRow, .index = lastIndex });
	lastIndex++;

	for (const auto& columns : rows) {
		this->rows.push_back({ .columns = columns, .index = lastIndex });
		lastIndex++;
	}

	this->setRows();
}

void LSG_Table::AddRow(const LSG_Strings& columns)
{
	if (columns.empty())
		return;

	auto lastIndex = this->getLastRowIndex();

	this->rows.push_back({ .columns = columns, .index = (lastIndex + 1) });

	this->setRows();
}

int LSG_Table::getClickedHeaderColumn(const SDL_Point& mousePosition, const std::vector<SDL_Size>& columnSizes, int maxWidth)
{
	auto column           = SDL_Rect();
	auto remainingX       = maxWidth;
	auto remainingOffsetX = this->scrollOffsetX;

	for (int i = 0; i < (int)this->textures.size(); i++)
	{
		auto textureWidth = (columnSizes[i].width + LSG_TABLE_COLUMN_SPACING);

		column.w = max(0, min((textureWidth - max(0, remainingOffsetX)), remainingX));
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
	int columnCount = 0;

	if (this->getFirstRow() > 0)
		columnCount = (int)this->header.columns.size();
	else if (!this->rows.empty())
		columnCount = (int)this->rows[0].columns.size();
	else if (!this->groups.empty())
		columnCount = (int)this->groups[0].columns.size();

	return columnCount;
}

int LSG_Table::getLastRowIndex()
{
	int index = -1;

	for (const auto& row : this->rows)
		index = max(row.index, index);

	for (const auto& group : this->groups)
		index = max(group.index, index);

	return index;
}

SDL_Color LSG_Table::getOffsetColor(const SDL_Color& color, int offset)
{
	const int DEFAULT = (255 - offset);
	const int MAX     = (DEFAULT - offset);

	auto r = (color.r < MAX ? (color.r + offset) : DEFAULT);
	auto g = (color.g < MAX ? (color.g + offset) : DEFAULT);
	auto b = (color.b < MAX ? (color.b + offset) : DEFAULT);

	return SDL_Color(r, g, b, 255);
}

LSG_Strings LSG_Table::GetRow(int row)
{
	for (const auto& r : this->rows) {
		if (r.index == row)
			return r.columns;
	}

	for (const auto& group : this->groups) {
		if (group.index == row)
			return group.columns;
	}

	if (this->header.index == row)
		return this->header.columns;

	return {};
}

LSG_TableRows LSG_Table::GetRows()
{
	auto rowCount = (this->rows.size() + this->groups.size());
	auto rows     = LSG_TableRows(rowCount);

	for (const auto& row : this->rows)
		rows[row.index] = row.columns;

	for (const auto& group : this->groups)
		rows[group.index] = group.columns;

	return rows;
}

int LSG_Table::getRowHeight(const SDL_Size& textureSize)
{
	if (this->rows.empty())
		return 0;

	auto firstRow   = this->getFirstRow();
	auto rowCount   = ((int)this->rows.size() + firstRow);
	auto groupCount = (int)this->groups.size();

	if (groupCount > 0)
		rowCount += groupCount;

	auto rowHeight = (textureSize.height / rowCount);

	return rowHeight;
}

bool LSG_Table::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->rows.empty())
		return false;

	auto background  = this->getFillArea(this->background, this->border);
	auto positionY   = (event.y - background.y);
	auto textureSize = this->getTextureSizes(LSG_TABLE_COLUMN_SPACING);
	auto rowHeight   = this->getRowHeight(textureSize.totalSize);

	if (rowHeight < 1)
		return false;

	bool isDoubleClick = LSG_Events::IsDoubleClick(event);
	auto noScrollRow   = (positionY / rowHeight);

	if (!this->header.columns.empty() && (noScrollRow == 0))
	{
		if (isDoubleClick)
			return false;

		auto positionX  = (event.x - background.x);
		auto position   = SDL_Point(positionX, positionY);
		auto sortColumn = this->getClickedHeaderColumn(position, textureSize.sizes, background.w);

		if (sortColumn < 0)
			return false;

		if ((this->sortOrder == LSG_ASCENDING) && (this->sortColumn == sortColumn))
			this->sortOrder = LSG_DESCENDING;
		else
			this->sortOrder = LSG_ASCENDING;

		this->sortColumn = sortColumn;

		this->setRows();

		return true;
	}

	auto scrolledRow = ((positionY + this->scrollOffsetY) / rowHeight);

	bool selected = this->select(scrolledRow);

	if (selected && isDoubleClick)
		this->sendEvent(LSG_EVENT_ROW_ACTIVATED);

	return true;
}

void LSG_Table::RemoveHeader()
{
	auto firstRow = this->getFirstRow();

	if (firstRow < 1)
		return;

	this->header = {};

	for (auto& row : this->rows)
		row.index--;

	for (auto& group : this->groups)
		group.index--;

	this->setRows();
}

void LSG_Table::RemoveGroup(const std::string& group)
{
	int groupIndex     = -1;
	int nextGroupIndex = -1;

	for (auto iter = this->groups.begin(); iter != this->groups.end(); iter++)
	{
		if ((*iter).columns[0] != group)
			continue;

		groupIndex     = (*iter).index;
		auto nextIter  = this->groups.erase(iter);
		nextGroupIndex = (nextIter != this->groups.end() ? (*nextIter).index : (this->getLastRowIndex() + 1));

		break;
	}

	if (groupIndex < 0)
		return;

	for (auto iter = this->rows.begin(); iter != this->rows.end();)
	{
		if (((*iter).index > groupIndex) && ((*iter).index < nextGroupIndex))
			iter = this->rows.erase(iter);
		else
			iter++;
	}

	auto indexDiff = (nextGroupIndex - groupIndex);

	for (auto& row : this->rows) {
		if (row.index > groupIndex)
			row.index -= indexDiff;
	}

	for (auto& group : this->groups) {
		if (group.index > groupIndex)
			group.index -= indexDiff;
	}

	this->setRows();
}

void LSG_Table::RemoveRow(int row)
{
	bool isRemoved = false;

	for (auto iter = this->rows.begin(); iter != this->rows.end(); iter++)
	{
		if ((*iter).index != row)
			continue;

		this->rows.erase(iter);
		isRemoved = true;

		break;
	}

	if (!isRemoved)
	{
		for (auto iter = this->groups.begin(); iter != this->groups.end(); iter++)
		{
			if ((*iter).index != row)
				continue;

			this->groups.erase(iter);
			isRemoved = true;

			break;
		}
	}

	if (!isRemoved)
		return;

	for (auto& r : this->rows) {
		if (r.index > row)
			r.index--;
	}

	for (auto& group : this->groups) {
		if (group.index > row)
			group.index--;
	}

	this->setRows();
}

void LSG_Table::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (this->textures.empty())
		return;

	auto tableBackground = this->getFillArea(this->background, this->border);
	auto textureSize     = this->getTextureSizes(LSG_TABLE_COLUMN_SPACING);
	auto rowHeight       = this->getRowHeight(textureSize.totalSize);

	this->setRowHeights(rowHeight, tableBackground);
	this->renderTextures(renderer, tableBackground);

	if (this->showRowBorder)
		this->renderRowBorder(renderer, rowHeight, tableBackground);

	if (!this->enabled)
		this->renderDisabledOverlay(renderer);
}

void LSG_Table::renderFillHeader(SDL_Renderer* renderer, const SDL_Rect& renderDestination, int rowHeight)
{
	SDL_Rect headerFillDest = renderDestination;

	headerFillDest.h = rowHeight;

	auto fillColor = this->getOffsetColor(this->backgroundColor, 30);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, fillColor.r, fillColor.g, fillColor.b, 255);
	SDL_RenderFillRect(renderer, &headerFillDest);
}

void LSG_Table::renderRowTextures(SDL_Renderer* renderer, int backgroundWidth, const std::vector<SDL_Size>& columnSizes, const SDL_Rect& renderClip, const SDL_Rect& renderDestination)
{
	SDL_Rect clip             = renderClip;
	SDL_Rect destination      = renderDestination;
	auto     remainingX       = backgroundWidth;
	auto     remainingOffsetX = this->scrollOffsetX;

	for (size_t i = 0; i < this->textures.size(); i++)
	{
		clip.x = max(0, remainingOffsetX);
		clip.w = max(0, min((columnSizes[i].width - clip.x), remainingX));

		destination.w = clip.w;
		destination.h = clip.h;

		SDL_RenderCopy(renderer, this->textures[i], &clip, &destination);

		auto columnSpacing = (clip.w > 0 ? LSG_TABLE_COLUMN_SPACING : 0);

		destination.x    += (clip.w + columnSpacing);
		remainingX       -= (clip.w + columnSpacing);
		remainingOffsetX -= columnSizes[i].width;
	}
}

void LSG_Table::renderTextures(SDL_Renderer* renderer, SDL_Rect& backgroundArea)
{
	auto textureSize = this->getTextureSizes(LSG_TABLE_COLUMN_SPACING);
	auto size        = textureSize.totalSize;

	SDL_Rect background = backgroundArea;

	if (this->enableScroll)
	{
		bool showScrollY = (size.height > backgroundArea.h);
		bool showScrollX = (size.width  > backgroundArea.w);

		if (showScrollY)
			background.w -= LSG_SCROLL_WIDTH;

		if (showScrollX)
			background.h -= LSG_SCROLL_WIDTH;

		this->showScrollY = (size.height > background.h);
		this->showScrollX = (size.width > background.w);

		if (this->showScrollY && !showScrollY)
			background.w -= LSG_SCROLL_WIDTH;

		if (this->showScrollX && !showScrollX)
			background.h -= LSG_SCROLL_WIDTH;
	}

	SDL_Rect clip = { 0, 0, min(size.width, background.w), min(size.height, background.h) };
	auto     dest = this->getRenderDestinationAligned(background, size);

	if (this->showScrollX)
	{
		auto maxScrollOffsetX = (size.width - clip.w);

		if (this->scrollOffsetX > maxScrollOffsetX)
			this->scrollOffsetX = maxScrollOffsetX;

		clip.x += this->scrollOffsetX;
	}

	if (this->showScrollY)
	{
		auto maxScrollOffsetY = (size.height - clip.h);

		if (this->scrollOffsetY > maxScrollOffsetY)
			this->scrollOffsetY = maxScrollOffsetY;

		clip.y += this->scrollOffsetY;
	}

	auto rowClip   = SDL_Rect(clip);
	auto rowDest   = SDL_Rect(dest);
	auto rowHeight = this->getRowHeight(textureSize.totalSize);

	auto firstRow = this->getFirstRow();

	if (firstRow > 0) {
		rowClip.y += rowHeight;
		rowClip.h -= rowHeight;
		rowDest.y += rowHeight;
	}

	this->renderRowTextures(renderer, background.w, textureSize.sizes, rowClip, rowDest);

	this->renderHighlightSelection(renderer, backgroundArea);

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

void LSG_Table::SetGroups(const LSG_TableGroups& groups)
{
	if (groups.empty())
		return;

	this->groups.clear();
	this->rows.clear();

	auto firstRow = this->getFirstRow();
	int  index    = 0;

	for (const auto& group : groups)
	{
		if (group.second.empty())
			continue;

		auto groupRow = LSG_Strings(group.second[0].size(), "");
		groupRow[0]   = group.first;

		this->groups.push_back({ .columns = groupRow, .index = (firstRow + index) });
		index++;

		for (const auto& row : group.second) {
			this->rows.push_back({ .columns = row, .index = (firstRow + index) });
			index++;
		}
	}

	this->setRows();
}

void LSG_Table::SetHeader(const LSG_Strings& header)
{
	if (header.empty())
		return;

	auto firstRow = this->getFirstRow();

	this->header = { .columns = header, .index = 0 };
	
	if (firstRow < 1)
	{
		for (auto& row : this->rows)
			row.index++;

		for (auto& group : this->groups)
			group.index++;
	}

	this->setRows();
}

void LSG_Table::SetRow(int row, const LSG_Strings& columns)
{
	if (columns.empty())
		return;

	for (auto& r : this->rows)
	{
		if (r.index != row)
			continue;

		r.columns = columns;
		this->setRows();

		return;
	}

	for (auto& group : this->groups)
	{
		if (group.index != row)
			continue;

		group.columns = columns;
		this->setRows();

		return;
	}
}

void LSG_Table::SetRows(const LSG_TableRows& rows)
{
	if (rows.empty())
		return;

	this->rows.clear();

	auto firstRow = this->getFirstRow();

	for (int i = 0; i < (int)rows.size(); i++) {
		if (!rows[i].empty())
			this->rows.push_back({ .columns = rows[i], .index = (firstRow + i) });
	}
	
	this->setRows();
}

void LSG_Table::setRows()
{
	if (this->header.columns.empty() && this->rows.empty() && this->groups.empty())
		return;

	auto firstRow    = this->getFirstRow();
	auto columnCount = this->getColumnCount();

	if (columnCount < 1)
		return;

	auto groups   = std::set<int>();
	auto rowCount = (firstRow + this->groups.size() + this->rows.size());
	auto rows     = LSG_TableRows(rowCount);

	this->textColumns = LSG_Strings(columnCount, "");

	if (firstRow > 0)
	{
		auto columns    = LSG_Strings(this->header.columns);
		auto sortColumn = ((this->sortColumn >= 0) && (this->sortColumn < (int)columns.size()) ? this->sortColumn : 0);

		if (this->sortOrder == LSG_ASCENDING)
			columns[sortColumn] = std::format("{}{}", LSG_ARROW_UP, columns[sortColumn]);
		else if (this->sortOrder == LSG_DESCENDING)
			columns[sortColumn] = std::format("{}{}", LSG_ARROW_DOWN, columns[sortColumn]);

		rows[0] = columns;
	}

	for (const auto& group : this->groups) {
		rows[group.index] = group.columns;
		groups.insert(group.index);
	}

	if (!this->sortOrder.empty())
		this->sort();

	for (const auto& row : this->rows)
		rows[row.index] = row.columns;

	bool isGrouped = false;

	for (int i = 0; i < (int)rows.size(); i++)
	{
		if (rows[i].empty())
			break;

		bool isGroup = groups.contains(i);

		if (isGrouped && !isGroup)
			this->textColumns[0].append("   ");

		this->textColumns[0].append(rows[i][0]).append("\n");

		for (size_t j = 1; j < rows[i].size(); j++)
			this->textColumns[j].append(rows[i][j]).append("\n");

		if (!isGrouped)
			isGrouped = isGroup;
	}

	if (this->textColumns.empty())
		return;

	this->destroyTextures();

	for (const auto& textColumn : this->textColumns)
		this->textures.push_back(this->getTexture(textColumn));

	if (this->row < firstRow)
		this->select(firstRow);
}

void LSG_Table::SetRows()
{
	if (!this->rows.empty() && !this->textColumns.empty() && !this->textures.empty() && SDL_ColorEquals(this->textColor, this->lastTextColor))
	{
		this->scrollOffsetX = 0;
		this->scrollOffsetY = 0;

		this->header.background = {};

		for (auto& row : this->rows)
			row.background = {};

		for (auto& group : this->groups)
			group.background = {};

		return;
	}

	if (this->rows.empty() || this->textColumns.empty())
	{
		this->header = {};
		this->groups.clear();
		this->rows.clear();
		this->textColumns.clear();

		bool isGrouped = false;

		for (int i = 0; i < (int)this->children.size(); i++)
		{
			auto row      = this->children[i];
			auto children = row->GetChildren();
			bool isGroup  = row->IsTableGroup();
			bool isHeader = row->IsTableHeader();

			if (children.empty())
				break;

			LSG_Strings columns;

			columns.push_back(children[0]->GetXmlValue());

			for (size_t j = 1; j < children.size(); j++)
				columns.push_back(children[j]->GetXmlValue());

			if (isHeader)
				this->header = { .columns = columns, .index = i };
			else if (isGroup)
				this->groups.push_back({ .columns = columns, .index = i });
			else
				this->rows.push_back({ .columns = columns, .index = i });

			if (!isGrouped)
				isGrouped = isGroup;
		}
	}

	this->setRows();
}

void LSG_Table::Sort(LSG_SortOrder sortOrder, int sortColumn)
{
	this->sortColumn = sortColumn;
	this->sortOrder  = (sortOrder == LSG_SORT_ORDER_DESCENDING ? LSG_DESCENDING : LSG_ASCENDING);

	this->setRows();
}

void LSG_Table::sort()
{
	auto columnCount = this->getColumnCount();

	if (this->rows.empty() || (columnCount < 1))
		return;

	auto sortColumn = ((this->sortColumn >= 0) && (this->sortColumn < columnCount) ? this->sortColumn : 0);

	auto compare = [sortColumn](const LSG_ListRow& r1, const LSG_ListRow& r2)
	{
		return std::lexicographical_compare(
			r1.columns[sortColumn].begin(), r1.columns[sortColumn].end(),
			r2.columns[sortColumn].begin(), r2.columns[sortColumn].end(),
			[](const char& c1, const char& c2) {
				return std::tolower(c1) < std::tolower(c2);
			}
		);
	};

	if (this->groups.empty())
	{
		if (this->sortOrder == LSG_DESCENDING)
			std::sort(this->rows.rbegin(), this->rows.rend(), compare);
		else
			std::sort(this->rows.begin(), this->rows.end(), compare);

		auto firstRow = this->getFirstRow();

		for (int i = 0; i < (int)this->rows.size(); i++)
			this->rows[i].index = (i + firstRow);

		return;
	}

	LSG_ListRows rows;

	for (int i = 0; i < (int)this->groups.size(); i++)
	{
		int start = this->groups[i].index;
		int next  = (i + 1);
		int last  = (int)(this->rows.size() - 1);
		int end   = (next < (int)this->groups.size() ? this->groups[next].index : (this->rows[last].index + 1));

		LSG_ListRows groupedRows;

		for (const auto& row : this->rows) {
			if ((row.index > start) && (row.index < end))
				groupedRows.push_back(row);
		}

		if (this->sortOrder == LSG_DESCENDING)
			std::sort(groupedRows.rbegin(), groupedRows.rend(), compare);
		else
			std::sort(groupedRows.begin(), groupedRows.end(), compare);

		for (int i = 0; i < (int)groupedRows.size(); i++)
		{
			groupedRows[i].index = (i + start + 1);

			rows.push_back(groupedRows[i]);
		}
	}

	this->rows = rows;
}
