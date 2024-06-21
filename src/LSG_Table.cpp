#include "LSG_Table.h"

LSG_Table::LSG_Table(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_List(id, layer, xmlNode, xmlNodeName, parent)
{
}

void LSG_Table::AddGroup(const LSG_TableGroup& group)
{
	this->groups.push_back(group);

	this->reset();
}

void LSG_Table::AddGroup(LibXml::xmlNode* node)
{
	LSG_TableGroup group = { .group = LSG_XML::GetAttribute(node, "group") };

	auto rowNodes = LSG_XML::GetChildNodes(node);

	for (auto rowNode : rowNodes)
	{
		if (std::string(reinterpret_cast<const char*>(rowNode->name)) != "table-row")
			continue;
			
		LSG_Strings row;

		auto columnNodes = LSG_XML::GetChildNodes(rowNode);

		for (auto columnNode : columnNodes)
		{
			if (std::string(reinterpret_cast<const char*>(columnNode->name)) == "table-column")
				row.push_back(LSG_XML::GetValue(columnNode));
		}

		group.rows.push_back(row);
	}

	this->AddGroup(group);
}

void LSG_Table::AddRow(const LSG_Strings& row)
{
	this->rows.push_back(row);

	this->reset();
}

void LSG_Table::AddRow(LibXml::xmlNode* node)
{
	LSG_Strings row;

	auto children = LSG_XML::GetChildNodes(node);

	for (auto child : children) {
		if (std::string(reinterpret_cast<const char*>(child->name)) == "table-column")
			row.push_back(LSG_XML::GetValue(child));
	}

	this->AddRow(row);
}

int LSG_Table::getColumnCount()
{
	auto columns = (int)this->header.size();

	for (const auto& group : this->groups) {
		for (const auto& row : group.rows)
			columns = std::max((int)row.size(), columns);
	}

	for (const auto& row : this->rows)
		columns = std::max((int)row.size(), columns);

	return columns;
}

int LSG_Table::GetSortColumn()
{
	auto sortColumn = this->GetXmlAttribute("sort-column");

	return (!sortColumn.empty() ? std::atoi(sortColumn.c_str()) : -1);
}

SDL_Size LSG_Table::GetSize()
{
	auto attributes    = this->GetXmlAttributes();
	auto columnSpacing = LSG_Graphics::GetDPIScaled(LSG_Table::ColumnSpacing);
	auto textureSize   = LSG_Graphics::GetTextureSize(this->textures, LSG_ORIENTATION_HORIZONTAL);

	textureSize.width += (columnSpacing * (int)(this->textures.size() - 1));

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

bool LSG_Table::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || (this->rows.empty() && this->groups.empty()))
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

	if (!this->header.empty() && ((positionY / rowHeight) == 0))
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

	auto header = (!this->header.empty() ? 1 : 0);
	auto row    = (((positionY + this->scrollOffsetY) / rowHeight) - header);

	this->Select(row);

	return true;
}

void LSG_Table::RemoveHeader()
{
	this->header.clear();

	this->Update();
}

void LSG_Table::RemoveGroup(const std::string& group)
{
	for (auto groupIter = this->groups.begin(); groupIter != this->groups.end(); groupIter++)
	{
		if ((*groupIter).group != group)
			continue;

		this->groups.erase(groupIter);

		this->removeRow();

		break;
	}
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

	for (const auto& pageGroup : this->groups)
	{
		if ((++i) >= end)
			return;

		for (const auto& groupRow : pageGroup.rows) {
			if ((++i) >= end)
				return;
		}
	}

	for (auto rowIter = this->rows.begin(); (rowIter != this->rows.end()) && (i < end); rowIter++)
	{
		if ((++i) >= end)
			return;

		if ((i < start) || (i != row))
			continue;

		this->rows.erase(rowIter);

		this->removeRow();

		return;
	}
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

void LSG_Table::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto attributes    = this->GetXmlAttributes();
	auto columnSpacing = LSG_Graphics::GetDPIScaled(LSG_Table::ColumnSpacing);
	auto textureSize   = LSG_Graphics::GetTextureSize(this->textures, LSG_ORIENTATION_HORIZONTAL);

	textureSize.width += (columnSpacing * (int)(this->textures.size() - 1));

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

void LSG_Table::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Table::render(SDL_Renderer* renderer)
{
	LSG_Component::Render(renderer);

	if (this->textures.empty())
		return;

	auto alignment       = this->getAlignment();
	auto columnSpacing   = LSG_Graphics::GetDPIScaled(LSG_Table::ColumnSpacing);
	auto fillArea        = this->getFillArea(this->background, this->border);
	auto rowHeight       = this->getRowHeight();
	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();
	bool showPagination  = this->showPagination();
	bool showRowBorder   = (this->GetXmlAttribute("row-border") == "true");
	auto textureSize     = LSG_Graphics::GetTextureSize(this->textures, LSG_ORIENTATION_HORIZONTAL);

	textureSize.width += (columnSpacing * (int)(this->textures.size() - 1));

	if (fillArea.h < scrollBarSize2x)
		return;

	if (showPagination)
		fillArea.h -= LSG_ScrollBar::GetSize();

	this->renderScrollableTextures(renderer, fillArea, this->border, alignment, this->textures, textureSize, columnSpacing);

	if (showRowBorder)
		this->renderRowBorder(renderer, fillArea, rowHeight);

	this->renderHighlightSelection(renderer, fillArea, rowHeight);

	if (!this->header.empty())
		this->renderHeader(renderer, fillArea, alignment, textureSize, columnSpacing, rowHeight);

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, fillArea, textureSize.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, fillArea, textureSize.height, this->backgroundColor, this->highlighted);

	if (showPagination)
		this->renderPagination(renderer, fillArea, this->backgroundColor);
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

void LSG_Table::SetGroup(const LSG_TableGroup& group)
{
	for (auto& pageGroup : this->groups)
	{
		if (pageGroup.group != group.group)
			continue;

		pageGroup.rows = group.rows;

		this->reset();

		break;
	}
}

void LSG_Table::SetGroups(const LSG_TableGroups& groups)
{
	this->groups = groups;

	this->reset();
}

void LSG_Table::SetHeader(const LSG_Strings& header)
{
	this->header = header;

	this->Update();
}

void LSG_Table::SetHeader(LibXml::xmlNode* node)
{
	LSG_Strings header;

	auto children = LSG_XML::GetChildNodes(node);

	for (auto child : children) {
		if (std::string(reinterpret_cast<const char*>(child->name)) == "table-column")
			header.push_back(LSG_XML::GetValue(child));
	}

	this->SetHeader(header);
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

	for (const auto& pageGroup : this->groups)
	{
		if ((++i) >= end)
			return;

		for (const auto& groupRow : pageGroup.rows) {
			if ((++i) >= end)
				return;
		}
	}

	for (auto& pageRow : this->rows)
	{
		if ((++i) >= end)
			return;

		if ((i < start) || (i != row))
			continue;

		pageRow = columns;

		this->reset();

		return;
	}
}

void LSG_Table::SetRows(const LSG_TableRows& rows)
{
	this->rows = rows;

	this->reset();
}

void LSG_Table::SetRows()
{
	if (!this->textures.empty() && !this->hasChanged())
		return;

	this->destroyTextures();

	this->setRows();
}

void LSG_Table::setRows(bool sort)
{
	if (this->showPagination())
		this->initPagination(this->getFillArea(this->background, this->border), this->backgroundColor);

	auto sortOrder = this->GetXmlAttribute("sort");

	if (sort && !sortOrder.empty())
		this->sort();

	auto pageGroups  = this->GetPageGroups();
	auto pageRows    = this->GetPageRows();
	auto sortColumn  = this->GetSortColumn();
	auto columnCount = this->getColumnCount();

	if (columnCount < 1)
		return;

	auto columns = LSG_Strings(columnCount, "");

	if (!this->header.empty())
	{
		for (size_t i = 0; i < columns.size(); i++)
		{
			if (i < this->header.size())
			{
				if ((i == sortColumn) && (sortOrder == LSG_ConstSortOrder::Ascending))
					columns[i].append(LSG_ConstUnicodeCharacter::ArrowUp);
				else if ((i == sortColumn) && (sortOrder == LSG_ConstSortOrder::Descending))
					columns[i].append(LSG_ConstUnicodeCharacter::ArrowDown);

				columns[i].append(!this->header[i].empty() ? this->header[i] : " ");
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
	auto compare    = LSG_Text::GetTableRowCompare(sortColumn);

	if (this->GetXmlAttribute("sort") == LSG_ConstSortOrder::Descending)
	{
		for (auto& group : this->groups)
			std::sort(group.rows.rbegin(), group.rows.rend(), compare);

		std::sort(this->rows.rbegin(), this->rows.rend(), compare);
	}
	else
	{
		for (auto& group : this->groups)
			std::sort(group.rows.begin(), group.rows.end(), compare);

		std::sort(this->rows.begin(), this->rows.end(), compare);
	}
}

void LSG_Table::Update()
{
	this->destroyTextures();
	this->setRows(false);
}
