#include "main.h"

#ifndef LSG_CARDS_H
#define LSG_CARDS_H

struct LSG_Card
{
	SDL_Rect      background  = {};
	LSG_ItemText  description = {};
	LSG_ItemImage thumbnail   = {};
	LSG_ItemText  title       = {};
};

class LSG_Cards : public LSG_Pagination, public LSG_ScrollBar, public LSG_Text, public LSG_IEvent
{
public:
	LSG_Cards(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Cards();

private:
	static const int CardBorder   = 1;
	static const int CardHeight   = 128;
	static const int CardPadding  = 10;
	static const int CardPageRows = 3;
	static const int CardSpacing  = 10;

private:
	int                   cardBorder;
	LSG_CardBorder        cardBorderType;
	int                   cardHeight;
	int                   cardPadding;
	int                   cardSpacing;
	std::vector<LSG_Card> cards;
	std::mutex            cardsLock;
	int                   highlightedRow;
	SDL_Point             offset;
	SDL_Texture*          renderTarget;
	std::vector<int>      selectedRows;
	int                   spacing;

public:
	void             Activate();
	void             Activate(const SDL_Point& mousePosition);
	void             AddCard(const LSG_CardItem& cardItem);
	void             AddCard(LibXml::xmlNode* node);
	LSG_CardItem     GetCard(int row) const;
	LSG_CardItems    GetCards() const;
	size_t           GetCardsCount() const;
	std::vector<int> GetSelectedCards() const;
	SDL_Size         GetSize() const;
	virtual void     OnMouseClick(const SDL_Point& mousePosition) override;
	void             OnMouseOver(const SDL_Point& mousePosition);
	void             RemoveCard(int row);
	virtual void     Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	void             Render(SDL_Renderer* renderer);
	bool             Select(int row);
	bool             Select(const std::vector<int>& rows);
	void             SelectAll();
	void             SelectFirst(bool keyShift = false);
	void             SelectLast(bool keyShift = false);
	void             SelectNextPage(bool keyShift = false);
	void             SelectNextRow(bool keyShift = false);
	void             SelectPreviousPage(bool keyShift = false);
	void             SelectPreviousRow(bool keyShift = false);
	void             SelectRow(int offset);
	void             SetCard(int row, const LSG_CardItem& cardItem);
	void             SetCards(const LSG_CardItems& cardItems);
	void             SetCards();

private:
	void          destroySurfaces(LSG_Card& card);
	void          destroySurfaces();
	void          destroyTextures(LSG_Card& card);
	virtual void  destroyTextures() override;
	int           getRow(const SDL_Point& mousePosition) const;
	int           getTitleFontSize() const;
	void          render(SDL_Renderer* renderer);
	void          renderCardBorder(SDL_Renderer* renderer, int row, const SDL_Rect& background) const;
	void          renderCardBorderLine(SDL_Renderer* renderer, int row, const SDL_Rect& background) const;
	void          renderContent(SDL_Renderer* renderer, const SDL_Size& textureSize);
	void          renderDescription(SDL_Renderer* renderer, const LSG_Card& card) const;
	void          renderScrollBar(SDL_Renderer* renderer, const SDL_Size& textureSize);
	void          renderTitle(SDL_Renderer* renderer, const LSG_Card& card) const;
	void          renderThumbnail(SDL_Renderer* renderer, const LSG_Card& card) const;
	void          renderToTarget(SDL_Renderer* renderer, const SDL_Size& textureSize);
	void          reset(bool resetScroll = false);
	void          resetHighlight();
	void          resetRenderTarget();
	void          resetScroll();
	void          select(LSG_EventType eventType);
	void          selectCtrl(int row);
	void          selectShift(int row);
	virtual void  sendEvent(LSG_EventType type) const override;
	void          setCards();
	void          setCardSurfaces();
	void          setCardTextures();

private:
	static LSG_Card     ToCard(const LSG_CardItem& cardItem);
	static LSG_CardItem ToCardItem(const LSG_Card& card);
};

#endif
