#include "main.h"

#ifndef LSG_TABLE_H
#define LSG_TABLE_H

struct LSG_TableItemRow
{
	SDL_Rect    background = {};
	LSG_Strings columns    = {};
};

using LSG_TableItemRows = std::vector<LSG_TableItemRow>;

struct LSG_TableItemGroup
{
	std::string       group = "";
	LSG_TableItemRows rows  = {};
};

using LSG_TableItemGroups = std::vector<LSG_TableItemGroup>;

class LSG_Table : public LSG_List
{
public:
	LSG_Table(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Table() {}

private:
	LSG_TableItemRow    header;
	LSG_TableItemGroups groups;
	LSG_TableItemRows   rows;
	int                 sortColumn;
	LSG_Strings         textColumns;

public:
	void          AddGroup(const std::string& group, const LSG_TableRows& rows);
	void          AddRow(const LSG_Strings& columns);
	LSG_Strings   GetRow(int row);
	LSG_TableRows GetRows();
	int           GetSortColumn();
	virtual bool  MouseClick(const SDL_MouseButtonEvent& event) override;
	void          RemoveHeader();
	void          RemoveGroup(const std::string& group);
	void          RemoveRow(int row);
	virtual void  Render(SDL_Renderer* renderer) override;
	bool          Select(int row);
	void          SelectFirstRow();
	void          SelectLastRow();
	void          SelectRow(int offset);
	void          SetGroups(const LSG_TableGroups& groups);
	void          SetHeader(const LSG_Strings& header);
	void          SetPage(int page);
	void          SetRow(int row, const LSG_Strings& columns);
	void          SetRows(const LSG_TableRows& rows);
	void          SetRows();
	void          Sort(LSG_SortOrder sortOrder, int sortColumn);

private:
	int  getClickedHeaderColumn(const SDL_Point& mousePosition, const std::vector<SDL_Size>& columnSizes, int maxWidth);
	int  getColumnCount();
	int  getFirstRow();
	int  getLastRow();
	int  getRowHeight();
	int  getRowHeight(const SDL_Size& textureSize);
	void renderFillHeader(SDL_Renderer* renderer, const SDL_Rect& renderDestination, int rowHeight);
	void renderRowBorder(SDL_Renderer* renderer, const SDL_Rect& backgroundArea, int rowHeight);
	void renderRowTextures(SDL_Renderer* renderer, int backgroundWidth, const std::vector<SDL_Size>& columnSizes, const SDL_Rect& renderClip, const SDL_Rect& renderDestination);
	void renderTextures(SDL_Renderer* renderer, SDL_Rect& backgroundArea, int rowHeight);
	void setPageItems();
	void setRowHeights(const SDL_Rect& backgroundArea, int rowHeight);
	void setRowHeights(LSG_TableItemRows& rows, const SDL_Rect& backgroundArea, int rowHeight, int& i);
	void sort();
	void updatePage(bool reset = true);

};

#endif
