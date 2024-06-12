#include "main.h"

#ifndef LSG_LIST_H
#define LSG_LIST_H

class LSG_List : public LSG_Pagination, public LSG_ScrollBar, public LSG_Text, public LSG_IEvent
{
public:
	LSG_List(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_List() {}

public:
	static const int LayerOffset = 100000000;
	static const int UnitPage    = 5;

protected:
	int row;

public:
	void           AddItem(const std::string& item, bool reset = true);
	void           Activate();
	void           Activate(const SDL_Point& mousePosition);
	int            GetSelectedRow() const;
	LSG_SortOrder  GetSortOrder();
	virtual bool   OnMouseClick(const SDL_Point& mousePosition) override;
	void           RemoveItem(int row);
	void           RemovePageItem(int row);
	virtual void   Render(SDL_Renderer* renderer) override;
	bool           Select(int row);
	void           SelectFirstRow();
	void           SelectLastRow();
	void           SelectRow(int offset);
	void           SetItem(int row, const std::string& item);
	void           SetItems(const LSG_Strings& items);
	void           SetItems();
	void           SetPage(int page);
	void           SetPageItem(int row, const std::string& item);
	void           Sort(LSG_SortOrder sortOrder);
	void           Update();

protected:
	int          getRowHeight();
	void         renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight);
	void         renderRowBorder(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight);
	virtual void reset();
	virtual void sendEvent(LSG_EventType type) override;

private:
	void removeItem(int row, int lastRow);
	void setItem(int row, int lastRow, const std::string& item);
	void setItems(bool sort = true);
	void sort();
};

#endif
