#include "LSG_Table.h"

LSG_Table::LSG_Table(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_List(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_Table::addColumn(const std::string& column, LSG_Component* parent)
{
	if (!parent)
		return;

	auto columnNode      = LSG_XML::AddChildNode(parent->GetXmlNode(), "table-column");
	auto columnComponent = LSG_UI::AddXmlNode(columnNode, parent);

	columnComponent->text = column;
}

void LSG_Table::addHeader(const LSG_Strings& header)
{
	auto headerNode = LSG_XML::AddChildNode(this->xmlNode, "table-header");

	if (!headerNode) {
		auto error = LibXml::xmlGetLastError();
		throw std::runtime_error(LSG_Text::Format("Failed to add table header: %s", (error ? error->message : "")));
	}

	auto headerComponent = LSG_UI::AddXmlNode(headerNode, this);

	for (const auto& column : header)
		this->addColumn(column, headerComponent);

	this->pageHeader = headerComponent;
}

LSG_Component* LSG_Table::AddGroup(const LSG_TableGroupRows& group)
{
	auto groupComponent = this->addGroup(group);

	this->reset();

	return groupComponent;
}

LSG_Component* LSG_Table::addGroup(const LSG_TableGroupRows& group)
{
	if (group.group.empty() || group.rows.empty())
		return nullptr;

	auto groupNode = LSG_XML::AddChildNode(this->xmlNode, "table-group");

	if (!groupNode) {
		auto error = LibXml::xmlGetLastError();
		throw std::runtime_error(LSG_Text::Format("Failed to add table group: %s", (error ? error->message : "")));
	}

	LSG_XML::SetAttribute(groupNode, "group", group.group);

	auto groupCompoment = LSG_UI::AddXmlNode(groupNode, this);

	for (const auto& row : group.rows)
	{
		auto rowNode      = LSG_XML::AddChildNode(groupNode, "table-row");
		auto rowComponent = LSG_UI::AddXmlNode(rowNode, groupCompoment);

		for (const auto& column : row)
			this->addColumn(column, rowComponent);
	}

	return groupCompoment;
}

/**
 * @throws runtime_error
 */
LSG_Component* LSG_Table::AddRow(const LSG_Strings& columns)
{
	auto rowComponent = this->addRow(columns, this);

	this->reset();

	return rowComponent;
}

/**
 * @throws runtime_error
 */
LSG_Component* LSG_Table::addRow(const LSG_Strings& columns, LSG_Component* parent)
{
	if (columns.empty() || !parent)
		return nullptr;

	auto rowNode = LSG_XML::AddChildNode(parent->GetXmlNode(), "table-row");

	if (!rowNode) {
		auto error = LibXml::xmlGetLastError();
		throw std::runtime_error(LSG_Text::Format("Failed to add table row: %s", (error ? error->message : "")));
	}

	auto rowComponent = LSG_UI::AddXmlNode(rowNode, parent);

	for (const auto& column : columns)
		this->addColumn(column, rowComponent);

	return rowComponent;
}

int LSG_Table::getColumnCount()
{
	auto columns = (int)(this->pageHeader ? this->pageHeader->GetChildCount() : 0);

	for (const auto& group : this->pageGroups) {
		for (const auto& row : group->GetChildren())
			columns = std::max((int)row->GetChildCount(), columns);
	}

	for (const auto& row : this->pageRows)
		columns = std::max((int)row->GetChildCount(), columns);

	return columns;
}

int LSG_Table::GetSortColumn()
{
	auto sortColumn = this->GetXmlAttribute("sort-column");

	return (!sortColumn.empty() ? std::atoi(sortColumn.c_str()) : -1);
}

bool LSG_Table::OnMouseClick(const SDL_Point& mousePosition)
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
	auto positionY  = (mousePosition.y - background.y);
	auto rowHeight  = this->getRowHeight();

	if (rowHeight < 1)
		return false;

	if (this->pageHeader && ((positionY / rowHeight) == 0))
	{
		int  column      = 0;
		auto positionX   = (mousePosition.x - background.x + this->scrollOffsetX);
		auto spacingHalf = (LSG_Graphics::GetDPIScaled(LSG_Table::ColumnSpacing) / 2);
		auto startX      = 0;

		for (const auto& texture : this->textures)
		{
			auto size = LSG_Graphics::GetTextureSize(texture);
			auto endX = (startX + size.width + spacingHalf);

			if ((positionX >= startX) && (positionX <= endX))
				break;

			startX = (endX + spacingHalf);

			if (column < (int)(this->textures.size() - 1))
				column++;
		}

		if ((this->GetXmlAttribute("sort") == LSG_ConstSortOrder::Ascending) && (this->GetSortColumn() == column))
			this->Sort(LSG_SORT_ORDER_DESCENDING, column);
		else
			this->Sort(LSG_SORT_ORDER_ASCENDING, column);

		return true;
	}

	auto header = (this->pageHeader ? 1 : 0);
	auto row    = (((positionY + this->scrollOffsetY) / rowHeight) - header);

	this->Select(row);

	return true;
}

void LSG_Table::RemoveHeader()
{
	this->removeHeader();
	this->Update();
}

void LSG_Table::removeHeader()
{
	if (!this->pageHeader)
		return;

	LSG_UI::RemoveXmlChildNodes(this->pageHeader);
	LSG_UI::RemoveXmlNode(this->pageHeader);

	this->pageHeader = nullptr;
}

void LSG_Table::RemoveGroup(const std::string& group)
{
	for (auto groupIter = this->pageGroups.begin(); groupIter != this->pageGroups.end(); groupIter++)
	{
		if ((*groupIter)->GetXmlAttribute("group") != group)
			continue;

		this->removeGroup(*groupIter);
		this->pageGroups.erase(groupIter);

		this->removeRow();

		break;
	}
}

void LSG_Table::removeGroup(LSG_Component* group)
{
	auto rows = group->GetChildren();

	for (auto rowIter = rows.begin(); rowIter != rows.end(); rowIter++)
	{
		auto columns = (*rowIter)->GetChildren();

		for (auto colIter = columns.begin(); colIter != columns.end(); colIter++)
			LSG_UI::RemoveXmlNode(*colIter);

		LSG_UI::RemoveXmlNode(*rowIter);
	}

	LSG_UI::RemoveXmlNode(group);
}

void LSG_Table::RemovePageRow(int row)
{
	auto rowIndex = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);
	auto start    = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end      = (start + LSG_MAX_ROWS_PER_PAGE);

	this->removeRow(rowIndex, start, end);
}

void LSG_Table::RemoveRow(int row)
{
	this->removeRow(row, 0, this->getRowCount());
}

void LSG_Table::removeRow(int row, int start, int end)
{
	int i = -1;

	for (const auto& pageGroup : this->pageGroups)
	{
		if ((++i) >= end)
			return;

		for (auto groupRow : pageGroup->GetChildren()) {
			if ((++i) >= end)
				return;
		}
	}

	for (auto rowIter = this->pageRows.begin(); (rowIter != this->pageRows.end()) && (i < end); rowIter++)
	{
		if ((++i) >= end)
			return;

		if ((i < start) || (i != row))
			continue;

		this->removeRow(*rowIter);
		this->pageRows.erase(rowIter);
		this->removeRow();

		return;
	}
}

void LSG_Table::removeRow(LSG_Component* row)
{
	LSG_UI::RemoveXmlChildNodes(row);
	LSG_UI::RemoveXmlNode(row);
}

void LSG_Table::removeRow()
{
	this->Update();

	auto lastRow = this->getLastRow();

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

void LSG_Table::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (this->textures.empty())
		return;

	auto alignment       = this->getAlignment();
	auto background      = this->getFillArea(this->background, this->border);
	auto columnSpacing   = LSG_Graphics::GetDPIScaled(LSG_Table::ColumnSpacing);
	auto rowHeight       = this->getRowHeight();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();
	bool showPagination  = this->showPagination();
	bool showRowBorder   = (this->GetXmlAttribute("row-border") == "true");
	auto textureSize     = LSG_Graphics::GetTextureSize(this->textures, LSG_ORIENTATION_HORIZONTAL);

	textureSize.width += (columnSpacing * (int)(this->textures.size() - 1));

	if (background.h < scrollBarSize2x)
		return;

	if (showPagination)
		background.h -= LSG_ScrollBar::GetSize();

	this->renderScrollableTextures(renderer, background, alignment, this->textures, textureSize, columnSpacing);

	if (showRowBorder)
		this->renderRowBorder(renderer, background, rowHeight);

	this->renderHighlightSelection(renderer, background, rowHeight);

	if (this->pageHeader)
		this->renderHeader(renderer, background, alignment, textureSize, columnSpacing, rowHeight);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, background, textureSize.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, background, textureSize.height, this->backgroundColor, this->highlighted);

	if (showPagination)
		this->renderPagination(renderer, background, this->backgroundColor);
}

void LSG_Table::renderHeader(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, const SDL_Size& size, int spacing, int rowHeight)
{
	auto     color  = LSG_Graphics::GetOffsetColor(this->backgroundColor, 30);
	SDL_Rect clip   = { this->scrollOffsetX, 0, std::min(size.width, background.w), rowHeight };
	auto     dest   = LSG_Graphics::GetDestinationAligned(background, size, alignment);
	SDL_Rect header = { dest.x, dest.y, background.w, rowHeight };
		
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(renderer, &header);

	this->renderTextures(renderer, this->textures, background.w, spacing, clip, dest);
}

void LSG_Table::reset()
{
	this->destroyTextures();
	this->SetRows();
}

void LSG_Table::SetGroup(const LSG_TableGroupRows& group)
{
	for (auto pageGroup : this->pageGroups)
	{
		if (pageGroup->GetXmlAttribute("group") != group.group)
			continue;

		LSG_UI::RemoveXmlChildNodes(pageGroup);

		for (const auto& row : group.rows)
			this->addRow(row, pageGroup);

		this->reset();

		break;
	}
}

void LSG_Table::SetGroups(const LSG_TableGroups& groups)
{
	for (auto pageGroup : this->pageGroups)
		this->removeGroup(pageGroup);

	this->pageGroups.clear();

	for (const auto& group : groups)
		this->addGroup(group);

	this->reset();
}

void LSG_Table::SetHeader(const LSG_Strings& header)
{
	this->removeHeader();
	this->addHeader(header);

	this->Update();
}

void LSG_Table::SetPageRow(int row, const LSG_Strings& columns)
{
	auto rowIndex = ((this->page * LSG_MAX_ROWS_PER_PAGE) + row);
	auto start    = (this->page * LSG_MAX_ROWS_PER_PAGE);
	auto end      = (start + LSG_MAX_ROWS_PER_PAGE);

	this->setRow(rowIndex, start, end, columns);
}

void LSG_Table::SetRow(int row, const LSG_Strings& columns)
{
	this->setRow(row, 0, this->getRowCount(), columns);
}

void LSG_Table::setRow(int row, int start, int end, const LSG_Strings& columns)
{
	int i = -1;

	for (const auto& pageGroup : this->pageGroups)
	{
		if ((++i) >= end)
			return;

		for (auto groupRow : pageGroup->GetChildren()) {
			if ((++i) >= end)
				return;
		}
	}

	for (auto pageRow : this->pageRows)
	{
		if ((++i) >= end)
			return;

		if ((i < start) || (i != row))
			continue;

		LSG_UI::RemoveXmlChildNodes(pageRow);
		
		for (const auto& column : columns)
			this->addColumn(column, pageRow);

		this->reset();

		return;
	}
}

void LSG_Table::SetRows(const LSG_TableRows& rows)
{
	for (auto pageRow : this->pageRows)
		this->removeRow(pageRow);

	this->pageRows.clear();

	for (const auto& row : rows)
		this->addRow(row, this);

	this->reset();
}

void LSG_Table::SetRows()
{
	if (!this->textures.empty() && !this->hasChanged())
		return;

	this->pageHeader = nullptr;

	this->pageGroups.clear();
	this->pageRows.clear();

	this->destroyTextures();

	for (auto child : this->children)
	{
		if (child->IsTableGroup())
			this->pageGroups.push_back(child);
		else if (child->IsTableRow())
			this->pageRows.push_back(child);
		else if (child->IsTableHeader())
			this->pageHeader = child;
	}

	this->setRows();
}

void LSG_Table::Update()
{
	this->destroyTextures();
	this->setRows(false);
}

void LSG_Table::setRows(bool sort)
{
	if (this->showPagination())
		this->initPagination(this->getFillArea(this->background, this->border), this->backgroundColor);

	auto sortOrder = this->GetXmlAttribute("sort");

	if (sort && !sortOrder.empty())
		this->sort();

	auto header      = this->GetHeader();
	auto pageGroups  = this->GetPageGroups();
	auto pageRows    = this->GetPageRows();
	auto sortColumn  = this->GetSortColumn();
	auto columnCount = this->getColumnCount();

	if (columnCount < 1)
		return;

	auto columns = LSG_Strings(columnCount, "");

	if (this->pageHeader)
	{
		for (size_t i = 0; i < columns.size(); i++)
		{
			if (i < header.size())
			{
				if ((i == sortColumn) && (sortOrder == LSG_ConstSortOrder::Ascending))
					columns[i].append(LSG_ConstUnicodeCharacter::ArrowUp);
				else if ((i == sortColumn) && (sortOrder == LSG_ConstSortOrder::Descending))
					columns[i].append(LSG_ConstUnicodeCharacter::ArrowDown);

				columns[i].append(!header[i].empty() ? header[i] : " ");
			}

			columns[i].append("\n");
		}
	}

	for (const auto& group : pageGroups)
	{
		if (!columns.empty())
			columns[0].append(LSG_Text::Format("%s\n", group.group.c_str()));

		for (size_t i = 1; i < columns.size(); i++)
			columns[i].append("\n");

		for (const auto& row : group.rows)
		{
			for (size_t i = 0; i < columns.size(); i++)
			{
				if (i < row.size())
					columns[i].append(i > 0 ? row[i] : LSG_Text::Format("   %s", (!row[0].empty() ? row[0].c_str() : " ")));

				columns[i].append("\n");
			}
		}
	}

	for (const auto& row : pageRows)
	{
		for (size_t i = 0; i < columns.size(); i++)
		{
			if (i < row.size())
				columns[i].append(!row[i].empty() ? row[i].c_str() : " ");

			columns[i].append("\n");
		}
	}

	if (columns.empty())
		return;

	for (const auto& column : columns)
		this->textures.push_back(this->getTexture(column));
}

void LSG_Table::Sort(LSG_SortOrder sortOrder, int sortColumn)
{
	if ((sortOrder == this->GetSortOrder()) && (sortColumn == this->GetSortColumn()))
		return;

	LSG_XML::SetAttribute(this->xmlNode, "sort",        LSG_ConstSortOrder::ToString(sortOrder));
	LSG_XML::SetAttribute(this->xmlNode, "sort-column", std::to_string(sortColumn));

	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;

	this->Select(-1);
	this->reset();
}

void LSG_Table::sort()
{
	auto sortColumn = std::max(0, this->GetSortColumn());
	auto compare    = LSG_Text::GetRowCompare(sortColumn);

	if (this->GetXmlAttribute("sort") == LSG_ConstSortOrder::Descending)
	{
		for (auto group : this->pageGroups)
			static_cast<LSG_TableGroup*>(group)->Sort(compare, LSG_SORT_ORDER_DESCENDING);

		std::sort(this->pageRows.rbegin(), this->pageRows.rend(), compare);
	}
	else
	{
		for (auto group : this->pageGroups)
			static_cast<LSG_TableGroup*>(group)->Sort(compare, LSG_SORT_ORDER_ASCENDING);

		std::sort(this->pageRows.begin(), this->pageRows.end(), compare);
	}
}
