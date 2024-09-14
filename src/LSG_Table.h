#include "main.h"

#ifndef LSG_TABLE_H
#define LSG_TABLE_H

class LSG_Table : public LSG_List
{
public:
	LSG_Table(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Table() {}

public:
	static const int ColumnSpacing = 20;
	static const int LayerOffset   = 100000000;

private:
	std::vector<int>          columnWidths;
	std::vector<SDL_Texture*> headerTextures;
	int                       resizeColumn;
	int                       resizeColumnWidth;

public:
	void         AddGroup(const LSG_TableGroup& group);
	void         AddGroup(LibXml::xmlNode* node);
	void         AddRow(const LSG_Strings& row);
	void         AddRow(LibXml::xmlNode* node);
	int          GetColumnWidth(int column);
	int          GetSortColumn();
	SDL_Size     GetSize();
	bool         IsMouseOverColumnBorder(const SDL_Point& mousePosition);
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
	bool         OnMouseMove(const SDL_Point& mousePosition, const SDL_Point& lastEventPosition);
	void         OnMouseUp(bool resetWidth);
	void         RemoveGroup(const std::string& group);
	void         RemoveHeader();
	void         RemoveRow(int row);
	void         RemovePageRow(int row);
	void         Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetColumnWidth(int column, int width);
	void         SetGroup(const LSG_TableGroup& group);
	void         SetGroups(const LSG_TableGroups& groups);
	void         SetHeader(const LSG_Strings& header);
	void         SetHeader(LibXml::xmlNode* node);
	void         SetPageRow(int row, const LSG_Strings& columns);
	void         SetRow(int row, const LSG_Strings& columns);
	void         SetRows(const LSG_TableRows& rows);
	void         SetRows();
	void         Sort(LSG_SortOrder sortOrder, int sortColumn);
	void         Update();

private:
	virtual void destroyTextures() override;
	int          getColumnCount();
	int          getMaxColumnWidth(size_t column);
	int          getMinColumnWidth();
	SDL_Size     getTextureSize();
	void         removeRow(int row, int start, int end);
	void         removeRow();
	void         render(SDL_Renderer* renderer);
	void         renderColumn(SDL_Renderer* renderer, size_t column, SDL_Rect& clip, SDL_Rect& destination, int spacing, int& offsetX, int& width, bool header);
	void         renderHeader(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, const SDL_Size& size, int spacing, int rowHeight);
	void         renderRows(SDL_Renderer* renderer, const SDL_Rect& fillArea, const LSG_Alignment& alignment, const SDL_Size& size, int spacing);
	virtual void reset() override;
	void         setRow(int row, int start, int end, const LSG_Strings& columns);
	void         setRows(bool sort = true);
	void         sort();
};

#endif
