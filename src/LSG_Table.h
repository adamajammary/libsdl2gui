#include "main.h"

#ifndef LSG_TABLE_H
#define LSG_TABLE_H

class LSG_Table : public LSG_List
{
public:
	LSG_Table(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Table() {}

private:
	int              sortColumn;
	LSG_TableColumns textColumns;

public:
	void             AddGroup(const std::string& group, const LSG_TableRows& rows);
	void             AddRow(const LSG_TableColumns& columns);
	LSG_TableColumns GetRow(int row);
	LSG_TableRows    GetRows();
	virtual bool     MouseClick(const SDL_MouseButtonEvent& event) override;
	void             RemoveHeader();
	void             RemoveGroup(const std::string& group);
	void             RemoveRow(int row);
	virtual void     Render(SDL_Renderer* renderer) override;
	void             SetGroups(const LSG_TableGroups& groups);
	void             SetHeader(const LSG_TableColumns& header);
	void             SetRow(int row, const LSG_TableColumns& columns);
	void             SetRows(const LSG_TableRows& rows);
	void             SetRows();
	void             Sort(LSG_SortOrder sortOrder, int sortColumn);

private:
	int  getClickedHeaderColumn(const SDL_Point& mousePosition, const std::vector<SDL_Size>& columnSizes, int maxWidth);
	int  getColumnCount();
	int  getLastRowIndex();
	int  getRowHeight(const SDL_Size& textureSize);
	void renderFillHeader(SDL_Renderer* renderer, const SDL_Rect& renderDestination, int rowHeight);
	void renderRowTextures(SDL_Renderer* renderer, int backgroundWidth, const std::vector<SDL_Size>& columnSizes, const SDL_Rect& renderClip, const SDL_Rect& renderDestination);
	void renderTextures(SDL_Renderer* renderer, SDL_Rect& backgroundArea);
	void setRows();
	void sort();

};

#endif
