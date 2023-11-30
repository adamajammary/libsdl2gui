#include "main.h"

#ifndef LSG_TABLE_H
#define LSG_TABLE_H

class LSG_Table : public LSG_List
{
public:
	LSG_Table(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Table() {}

public:
	static const int ColumnSpacing = 10;
	static const int LayerOffset   = 100000000;

public:
	LSG_Component* AddGroup(const LSG_TableGroupRows& group);
	LSG_Component* AddRow(const LSG_Strings& columns);
	int            GetSortColumn();
	virtual bool   OnMouseClick(const SDL_Point& mousePosition) override;
	void           RemoveGroup(const std::string& group);
	void           RemoveHeader();
	void           RemoveRow(int row);
	void           RemovePageRow(int row);
	virtual void   Render(SDL_Renderer* renderer) override;
	void           SetGroup(const LSG_TableGroupRows& group);
	void           SetGroups(const LSG_TableGroups& groups);
	void           SetHeader(const LSG_Strings& header);
	void           SetPageRow(int row, const LSG_Strings& columns);
	void           SetRow(int row, const LSG_Strings& columns);
	void           SetRows(const LSG_TableRows& rows);
	void           SetRows();
	void           Sort(LSG_SortOrder sortOrder, int sortColumn);
	void           Update();

private:
	void           addColumn(const std::string& column, LSG_Component* parent);
	void           addHeader(const LSG_Strings& header);
	LSG_Component* addGroup(const LSG_TableGroupRows& group);
	LSG_Component* addRow(const LSG_Strings& columns, LSG_Component* parent);
	int            getColumnCount();
	void           removeHeader();
	void           removeGroup(LSG_Component* group);
	void           removeRow(int row, int start, int end);
	void           removeRow(LSG_Component* row);
	void           removeRow();
	void           renderHeader(SDL_Renderer* renderer, const SDL_Rect& background, const LSG_Alignment& alignment, const SDL_Size& size, int spacing, int rowHeight);
	virtual void   reset() override;
	void           setRow(int row, int start, int end, const LSG_Strings& columns);
	void           setRows(bool sort = true);
	void           sort();

};

#endif
