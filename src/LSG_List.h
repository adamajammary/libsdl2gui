#include "main.h"

#ifndef LSG_LIST_H
#define LSG_LIST_H

struct LSG_ListItemRow
{
	SDL_Rect    background = {};
	std::string item       = "";
};

using LSG_ListItemRows = std::vector<LSG_ListItemRow>;

class LSG_List : public LSG_Pagination, public LSG_Text, public LSG_IEvent
{
public:
	LSG_List(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_List() {}

public:
	bool showRowBorder;

protected:
	int              row;
	LSG_ListItemRows rows;
	std::string      sortOrder;

public:
	void          AddItem(const std::string& item);
	void          Activate();
	std::string   GetItem(int row);
	LSG_Strings   GetItems();
	int           GetSelectedRow();
	LSG_SortOrder GetSortOrder();
	virtual bool  MouseClick(const SDL_MouseButtonEvent& event) override;
	void          RemoveItem(int row);
	virtual void  Render(SDL_Renderer* renderer) override;
	bool          Select(int row);
	void          SelectFirstRow();
	void          SelectLastRow();
	void          SelectRow(int offset);
	void          SetItem(int row, const std::string& item);
	void          SetItems(const LSG_Strings& items);
	void          SetItems();
	void          Sort(LSG_SortOrder sortOrder);

protected:
	void         renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& backgroundArea, const SDL_Rect& rowBackground);
	virtual void sendEvent(LSG_EventType type) override;

private:
	int         getLastRow();
	LSG_Strings getPageItems();
	int         getRowHeight();
	void        renderRowBorder(SDL_Renderer* renderer, int rowHeight, const SDL_Rect& backgroundArea);
	void        setRowHeights(int rowHeight, const SDL_Rect& backgroundArea);
	void        sort();
	void        updatePage(bool reset = true);

};

#endif
