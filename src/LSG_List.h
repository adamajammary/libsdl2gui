#include "main.h"

#ifndef LSG_LIST_H
#define LSG_LIST_H

class LSG_List : public LSG_Pagination, public LSG_ScrollBar, public LSG_Text
{
public:
	LSG_List(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_List() {}

public:
	static const int UnitPage = 5;

protected:
	std::vector<int> selectedRows;

public:
	void             Activate() const;
	void             Activate(const SDL_Point& mousePosition) const;
	void             AddItem(const std::string& item);
	std::vector<int> GetSelectedRows() const;
	LSG_SortOrder    GetSortOrder() const;
	SDL_Size         GetSize() const;
	void             OnMouseClick(const SDL_Point& mousePosition);
	void             RemoveItem(int row);
	void             RemovePageItem(int row);
	void             Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void     Render(SDL_Renderer* renderer) override;
	bool             Select(int row);
	bool             Select(int start, int end);
	bool             Select(const std::vector<int>& rows);
	void             SelectAll();
	void             SelectFirstRow();
	void             SelectFirstRowShift();
	void             SelectLastRow();
	void             SelectLastRowShift();
	void             SelectRow(int offset, bool multiSelect = false);
	void             SetItem(int row, const std::string& item);
	void             SetItems(const LSG_Strings& items);
	void             SetItems();
	void             SetPage(int page);
	void             SetPageItem(int row, const std::string& item);
	void             Sort(LSG_SortOrder sortOrder);
	void             Update();

protected:
	int          getRowHeight() const;
	void         renderHighlightSelection(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight) const;
	void         renderRowBorder(SDL_Renderer* renderer, const SDL_Rect& background, int rowHeight) const;
	virtual void reset();
	virtual void sendEvent(LSG_EventType type) const override;

private:
	void removeItem(int row, int lastRow);
	void render(SDL_Renderer* renderer);
	void setItem(int row, int lastRow, const std::string& item);
	void setItems(bool sort = true);
	void sort();
};

#endif
