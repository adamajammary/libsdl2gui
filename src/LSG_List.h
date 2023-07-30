#include "main.h"

#ifndef LSG_LIST_H
#define LSG_LIST_H

struct LSG_ListRow
{
	SDL_Rect    background = {};
	LSG_Strings columns    = {};
	int         index      = -1;
	std::string text       = "";
};

using LSG_ListRows = std::vector<LSG_ListRow>;

class LSG_List : public LSG_Text, public LSG_IEvent
{
public:
	LSG_List(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_List() {}

public:
	bool showRowBorder;

protected:
	LSG_ListRows groups;
	LSG_ListRow  header;
	int          row;
	LSG_ListRows rows;
	std::string  sortOrder;

public:
	void         AddItem(const std::string& item);
	void         Activate();
	std::string  GetItem(int row);
	LSG_Strings  GetItems();
	int          GetSelectedRow();
	virtual bool MouseClick(const SDL_MouseButtonEvent& event) override;
	void         RemoveItem(int row);
	virtual void Render(SDL_Renderer* renderer) override;
	bool         Select(int row);
	void         SelectFirstRow();
	void         SelectLastRow();
	void         SelectRow(int offset);
	void         SetItem(int row, const std::string& item);
	void         SetItems(const LSG_Strings& items);
	void         SetItems();
	void         Sort(LSG_SortOrder sortOrder);

protected:
	int          getFirstRow();
	int          getLastRow();
	int          getRowHeight();
	void         renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& backgroundArea);
	void         renderRowBorder(SDL_Renderer* renderer, int rowHeight, const SDL_Rect& backgroundArea);
	virtual void sendEvent(LSG_EventType type) override;
	void         setRowHeights(int rowHeight, const SDL_Rect& backgroundArea);
	void         setRowHeights(LSG_ListRows& rows, int rowHeight, const SDL_Rect& backgroundArea);
	LSG_Strings  sort(const LSG_Strings& items);

};

#endif
