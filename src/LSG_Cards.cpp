#include "LSG_Cards.h"

LSG_Cards::LSG_Cards(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->border         = 0;
	this->cardBorderType = LSG_CARD_BORDER_NONE;
	this->highlightedRow = -1;
	this->margin         = 0;
	this->offset         = {};
	this->padding        = 0;
	this->renderTarget   = nullptr;
	this->selectedRows   = {};
	this->spacing        = 0;
	this->text           = "";
	this->wrap           = true;

	this->cardBorder  = LSG_Graphics::GetDPIScaled(LSG_Cards::CardBorder);
	this->cardPadding = LSG_Graphics::GetDPIScaled(LSG_Cards::CardPadding);
	this->cardSpacing = LSG_Graphics::GetDPIScaled(LSG_Cards::CardSpacing);

	auto xmlCardHeight = LSG_XML::GetAttribute(xmlNode, "card-height");
	auto xmlCardBorder = LSG_XML::GetAttribute(xmlNode, "card-border");

	this->cardHeight = LSG_Graphics::GetDPIScaled(!xmlCardHeight.empty() ? std::atoi(xmlCardHeight.c_str()) : LSG_Cards::CardHeight);

	if (xmlCardBorder == "full")
		this->cardBorderType = LSG_CARD_BORDER_FULL;
	else if (xmlCardBorder == "line")
		this->cardBorderType = LSG_CARD_BORDER_LINE;
}

LSG_Cards::~LSG_Cards()
{
	this->destroyTextures();
}

void LSG_Cards::Activate()
{
	if (!this->selectedRows.empty())
		this->select(LSG_EVENT_ROW_ACTIVATED);
}

void LSG_Cards::Activate(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->cards.empty() || this->selectedRows.empty())
		return;

	auto positionY = (mousePosition.y - this->background.y + this->scrollOffsetY);

	for (const auto& card : this->cards)
	{
		if ((positionY >= card.background.y) && (positionY <= (card.background.y + card.background.h))) {
			this->sendEvent(LSG_EVENT_ROW_ACTIVATED);
			break;
		}
	}
}

void LSG_Cards::AddCard(const LSG_CardItem& cardItem)
{
	this->cards.push_back(LSG_Cards::ToCard(cardItem));

	this->reset();
}

void LSG_Cards::AddCard(LibXml::xmlNode* node)
{
	auto attributes = LSG_XML::GetAttributes(node);

	this->cards.push_back({
		.description = { .text     = (attributes.contains("description") ? attributes["description"] : "") },
		.thumbnail   = { .filePath = (attributes.contains("thumbnail")   ? attributes["thumbnail"] : "") },
		.title       = { .text     = (attributes.contains("title")       ? attributes["title"]  : "") }
	});
}

void LSG_Cards::destroyTextures(LSG_Card& card)
{
	if (card.description.texture.texture) {
		SDL_DestroyTexture(card.description.texture.texture);
		card.description.texture.texture = nullptr;
	}

	if (card.thumbnail.texture.texture) {
		SDL_DestroyTexture(card.thumbnail.texture.texture);
		card.thumbnail.texture.texture = nullptr;
	}

	if (card.title.texture.texture) {
		SDL_DestroyTexture(card.title.texture.texture);
		card.title.texture.texture = nullptr;
	}
}

void LSG_Cards::destroyTextures()
{
	for (auto& card : this->cards)
		this->destroyTextures(card);

	if (this->renderTarget) {
		SDL_DestroyTexture(this->renderTarget);
		this->renderTarget = nullptr;
	}
}

SDL_Size LSG_Cards::GetSize() const
{
	SDL_Size maxSize = { this->background.w, this->background.h };

	for (const auto& card : this->cards)
	{
		auto maxTitleWidth = (this->cardHeight + card.title.texture.size.width + this->cardPadding);

		if (maxTitleWidth > maxSize.width)
			maxSize.width = maxTitleWidth;

		auto maxDescriptionWidth = (this->cardHeight + card.description.texture.size.width + this->cardPadding);

		if (maxDescriptionWidth > maxSize.width)
			maxSize.width = maxDescriptionWidth;
	}

	auto textureHeight = (((this->cardHeight + this->cardSpacing) * (int)this->cards.size()) - this->cardSpacing);

	if (textureHeight > maxSize.height)
		maxSize.height = textureHeight;

	return maxSize;
}

LSG_CardItem LSG_Cards::GetCard(int row) const
{
	if ((row < 0) || (row >= (int)this->cards.size()))
		return {};

	return LSG_Cards::ToCardItem(this->cards[row]);
}

LSG_CardItems LSG_Cards::GetCards() const
{
	LSG_CardItems cards;

	for (const auto& card : this->cards)
		cards.push_back(LSG_Cards::ToCardItem(card));

	return cards;
}

size_t LSG_Cards::GetCardsCount() const
{
	return this->cards.size();
}

std::vector<int> LSG_Cards::GetSelectedCards() const
{
	return this->selectedRows;
}

bool LSG_Cards::initRenderTarget(const SDL_Size& textureSize)
{
	if (!this->renderTarget) {
		LSG_Window::InitRenderTarget(&this->renderTarget, textureSize);
		return true;
	}

	auto targetSize = LSG_Graphics::GetTextureSize(this->renderTarget);

	if ((textureSize.width != targetSize.width) || (textureSize.height != targetSize.height)) {
		LSG_Window::InitRenderTarget(&this->renderTarget, textureSize);
		return true;
	}

	return false;
}

bool LSG_Cards::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->cards.empty())
		return false;

	auto positionY = (mousePosition.y - this->background.y + this->scrollOffsetY);

	for (int row = 0; row < (int)this->cards.size(); row++)
	{
		const auto& card  = this->cards[row].background;
		auto        cardY = (card.y - this->offset.y);

		if ((positionY < cardY) || (positionY > (cardY + card.h)))
			continue;

		auto keyState = SDL_GetKeyboardState(nullptr);
		
		if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL])
			this->selectCtrl(row);
		else if (keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT])
			this->selectShift(row);
		else
			this->Select(row);

		break;
	}

	return true;
}

void LSG_Cards::OnMouseOver(const SDL_Point& mousePosition)
{
	if (!this->enabled)
		return;

	auto highlightedRow = -1;
	auto positionY      = (mousePosition.y - this->background.y + this->scrollOffsetY);

	for (int row = 0; row < (int)this->cards.size(); row++)
	{
		const auto& card = this->cards[row].background;

		if ((positionY >= card.y) && (positionY <= (card.y + card.h))) {
			highlightedRow = row;
			break;
		}
	}

	if (!this->renderTarget || (highlightedRow == this->highlightedRow))
		return;

	SDL_DestroyTexture(this->renderTarget);

	this->highlightedRow = highlightedRow;
	this->renderTarget   = nullptr;
}

void LSG_Cards::RemoveCard(int row)
{
	if ((row < 0) || (row >= (int)this->cards.size()))
		return;

	this->destroyTextures(this->cards[row]);

	this->cards.erase(this->cards.begin() + (size_t)row);

	this->reset();

	this->Select(!this->cards.empty() && !this->selectedRows.empty() ? this->selectedRows[0] : -1);
}

void LSG_Cards::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto textureSize = this->GetSize();

	this->offset = position;

	this->background.x = this->offset.x;
	this->background.y = this->offset.y;
	this->background.w = textureSize.width;
	this->background.h = textureSize.height;

	this->renderContent(renderer, textureSize);
}

void LSG_Cards::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Cards::render(SDL_Renderer* renderer)
{
	this->background.w = std::min(this->background.w, this->parent->background.w);
	this->background.h = std::min(this->background.h, this->parent->background.h);

	SDL_Size componentSize  = { this->background.w, this->background.h };
	auto     parentFillArea = LSG_Graphics::GetFillArea(this->parent->background, this->parent->border, this->parent->padding);

	this->background = LSG_Graphics::GetDestinationAligned(parentFillArea, componentSize, this->getAlignment());

	LSG_Component::Render(renderer);

	if (this->cards.empty())
		return;

	if (!this->highlighted)
		this->resetHighlight();

	auto textureSize    = this->GetSize();
	bool renderToTarget = this->initRenderTarget(textureSize);

	if (renderToTarget)
		this->renderToTarget(renderer, textureSize);

	LSG_Graphics::RenderFill(renderer, 0, this->backgroundColor, this->background);

	auto scrollBarSize = LSG_ScrollBar::GetSize();

	this->showScrollX = (textureSize.width  > this->background.w);
	this->showScrollY = (textureSize.height > this->background.h);

	auto fillArea = SDL_Rect(this->background);
	auto clip     = this->getClipWithOffset({ 0, 0, fillArea.w, fillArea.h }, textureSize);

	if (this->showScrollX)
		fillArea.h -= scrollBarSize;

	if (this->showScrollY)
		fillArea.w -= scrollBarSize;

	SDL_RenderCopy(renderer, this->renderTarget, &clip, &fillArea);

	this->renderScrollBar(renderer, textureSize);
}

void LSG_Cards::renderCardBorder(SDL_Renderer* renderer, int row, const SDL_Rect& background) const
{
	switch (this->cardBorderType) {
	case LSG_CARD_BORDER_FULL:
		LSG_Graphics::RenderBorder(renderer, this->cardBorder, this->borderColor, this->cards[row].background);
		break;
	case LSG_CARD_BORDER_LINE:
		this->renderCardBorderLine(renderer, row, background);
		break;
	default:
		break;
	}
}

void LSG_Cards::renderCardBorderLine(SDL_Renderer* renderer, int row, const SDL_Rect& background) const
{
	if (this->highlighted && ((this->highlightedRow == row) || (this->highlightedRow == (row + 1))))
		return;

	for (auto selectedRow : this->selectedRows) {
		if ((selectedRow == row) || (selectedRow == (row + 1)))
			return;
	}

	auto bottom      = (background.y + background.h);
	auto spacingHalf = (this->cardSpacing / 2);

	auto y  = (this->cards[row].background.y + this->cards[row].background.h + spacingHalf);
	auto x2 = (this->cards[row].background.x + this->cards[row].background.w);

	if (y <= bottom)
		LSG_Graphics::RenderLine(renderer, this->borderColor, this->cards[row].background.x, y, x2, y);
}

void LSG_Cards::renderContent(SDL_Renderer* renderer, const SDL_Size& textureSize)
{
	SDL_Rect background = {
		this->offset.x,
		this->offset.y,
		textureSize.width,
		textureSize.height
	};

	LSG_Graphics::RenderFill(renderer, 0, this->backgroundColor, background);

	auto offsetY = this->offset.y;

	for (int i = 0; i < (int)this->cards.size(); i++)
	{
		this->cards[i].background = {
			this->offset.x,
			offsetY,
			textureSize.width,
			this->cardHeight
		};

		this->renderThumbnail(renderer,   this->cards[i]);
		this->renderTitle(renderer,       this->cards[i]);
		this->renderDescription(renderer, this->cards[i]);

		SDL_Rect spacingArea = {
			this->cards[i].background.x,
			(this->cards[i].background.y + this->cards[i].background.h),
			this->cards[i].background.w,
			this->cardSpacing
		};

		LSG_Graphics::RenderFill(renderer, 0, this->parent->backgroundColor, spacingArea);

		this->renderCardBorder(renderer, i, background);

		offsetY += (this->cardHeight + this->cardSpacing);
	}

	auto border3x     = (this->cardBorder * 3);
	auto inverseColor = LSG_Graphics::GetInverseColor(this->backgroundColor);

	SDL_Color selectColor    = { this->borderColor.r, this->borderColor.g, this->borderColor.b, 255 };
	SDL_Color highlightColor = { inverseColor.r, inverseColor.g, inverseColor.b, 32 };

	for (auto row : this->selectedRows)
		LSG_Graphics::RenderBorder(renderer, border3x, selectColor, this->cards[row].background);

	if (this->highlighted && (this->highlightedRow >= 0))
		LSG_Graphics::RenderFill(renderer, 0, highlightColor, this->cards[this->highlightedRow].background);
}

void LSG_Cards::renderDescription(SDL_Renderer* renderer, const LSG_Card& card) const
{
	if (card.description.text.empty() || !card.description.texture.texture)
		return;

	auto border2x    = (this->cardBorder + this->cardBorder);
	auto padding2x   = (this->cardPadding + this->cardPadding);
	auto titleOffset = ((this->getFontSize() + 4) * 2);

	SDL_Rect clip = {
		0,
		0,
		card.description.texture.size.width,
		std::min(card.description.texture.size.height, (this->cardHeight - titleOffset - padding2x - border2x)),
	};

	SDL_Rect destination = {
		(card.background.x + this->cardHeight),
		(card.background.y + this->cardPadding + titleOffset),
		clip.w,
		clip.h
	};

	SDL_RenderCopy(renderer, card.description.texture.texture, &clip, &destination);
}

void LSG_Cards::renderScrollBar(SDL_Renderer* renderer, const SDL_Size& textureSize)
{
	if (this->background.h < LSG_ScrollBar::GetSize2x())
		return;

	if (this->showScrollX)
		this->renderScrollBarHorizontal(renderer, this->background, textureSize.width, this->backgroundColor, this->highlighted);

	if (this->showScrollY)
		this->renderScrollBarVertical(renderer, this->background, textureSize.height, this->backgroundColor, this->highlighted);
}

void LSG_Cards::renderTitle(SDL_Renderer* renderer, const LSG_Card& card) const
{
	if (card.title.text.empty() || !card.title.texture.texture)
		return;

	SDL_Rect destination = {
		(card.background.x + this->cardHeight),
		(card.background.y + this->cardPadding),
		card.title.texture.size.width,
		card.title.texture.size.height
	};

	SDL_RenderCopy(renderer, card.title.texture.texture, nullptr, &destination);
}

void LSG_Cards::renderThumbnail(SDL_Renderer* renderer, const LSG_Card& card) const
{
	if (!card.thumbnail.texture.texture)
		return;

	auto padding2x = (this->cardPadding + this->cardPadding);

	SDL_Rect destination = {
		(card.background.x + this->cardPadding),
		(card.background.y + this->cardPadding),
		(this->cardHeight - padding2x),
		(this->cardHeight - padding2x)
	};

	auto imageSize = std::min(card.thumbnail.texture.size.width, card.thumbnail.texture.size.height);

	SDL_Rect clip = {
		std::max(((card.thumbnail.texture.size.width  - card.thumbnail.texture.size.height) / 2), 0),
		std::max(((card.thumbnail.texture.size.height - card.thumbnail.texture.size.width)  / 2), 0),
		imageSize,
		imageSize
	};

	SDL_RenderCopy(renderer, card.thumbnail.texture.texture, &clip, &destination);
}

void LSG_Cards::renderToTarget(SDL_Renderer* renderer, const SDL_Size& textureSize)
{
	SDL_SetRenderTarget(renderer, this->renderTarget);

	this->renderContent(renderer, textureSize);

	SDL_SetRenderTarget(renderer, nullptr);
}

void LSG_Cards::reset(bool resetScroll)
{
	if (resetScroll)
		this->resetScroll();

	this->destroyTextures();
	this->setCards();
}

void LSG_Cards::resetHighlight()
{
	if (!this->renderTarget || (this->highlightedRow == -1))
		return;

	SDL_DestroyTexture(this->renderTarget);

	this->highlightedRow = -1;
	this->renderTarget   = nullptr;
}

void LSG_Cards::resetScroll()
{
	this->scrollOffsetX = 0;
	this->scrollOffsetY = 0;
}

void LSG_Cards::select(LSG_EventType eventType)
{
	if (this->renderTarget) {
		SDL_DestroyTexture(this->renderTarget);
		this->renderTarget = nullptr;
	}

	this->sendEvent(eventType);
}

bool LSG_Cards::Select(int row)
{
	if (!this->enabled || (row >= (int)this->cards.size()))
		return false;

	if (row < 0) {
		this->selectedRows.clear();
		this->select(LSG_EVENT_ROW_UNSELECTED);
	} else {
		this->selectedRows = { row };
		this->select(LSG_EVENT_ROW_SELECTED);
	}

	return true;
}

bool LSG_Cards::Select(const std::vector<int>& rows)
{
	if (!this->enabled)
		return false;

	this->selectedRows.clear();

	for (auto row : rows) {
		if ((row >= 0) && (row < (int)this->cards.size()))
			this->selectedRows.push_back(row);
	}

	this->select(this->selectedRows.empty() ? LSG_EVENT_ROW_UNSELECTED : LSG_EVENT_ROW_SELECTED);

	return true;
}

void LSG_Cards::SelectAll()
{
	if (!this->enabled || this->cards.empty())
		return;

	this->resetScroll();

	this->selectedRows.clear();

	for (int i = 0; i < (int)this->cards.size(); i++)
		this->selectedRows.push_back(i);

	this->select(LSG_EVENT_ROW_SELECTED);
}

void LSG_Cards::selectCtrl(int row)
{
	if (!this->enabled || (row < 0) || (row >= (int)this->cards.size()))
		return;

	auto rowIter   = std::find(this->selectedRows.begin(), this->selectedRows.end(), row);
	bool rowExists = (rowIter != this->selectedRows.end());
		
	if (!rowExists)
		this->selectedRows.push_back(row);
	else
		this->selectedRows.erase(rowIter);

	this->select(!rowExists ? LSG_EVENT_ROW_SELECTED : LSG_EVENT_ROW_UNSELECTED);
}

void LSG_Cards::SelectFirst(bool keyShift)
{
	if (!this->enabled || this->cards.empty())
		return;

	this->resetScroll();

	if (keyShift)
		this->selectShift(0);
	else
		this->Select(0);
}

void LSG_Cards::SelectLast(bool keyShift)
{
	if (!this->enabled || this->cards.empty())
		return;

	this->scrollOffsetY = LSG_ConstTexture::MaxSize;

	auto last = ((int)this->cards.size() - 1);

	if (keyShift)
		this->selectShift(last);
	else
		this->Select(last);
}

void LSG_Cards::SelectNextPage(bool keyShift)
{
	if (!this->enabled || this->cards.empty())
		return;

	if (this->selectedRows.empty()) {
		this->SelectFirst();
		return;
	}

	auto selectedRow   = this->selectedRows[this->selectedRows.size() - 1];
	auto maxRows       = (int)this->cards.size();
	auto remainingRows = (maxRows - 1 - selectedRow);
	auto pageRows      = std::min(LSG_Cards::CardPageRows, remainingRows);
	auto nextRow       = (selectedRow + pageRows);

	if ((remainingRows < 1) || (nextRow >= maxRows))
		return;

	this->scrollOffsetY += ((this->cardHeight + this->cardSpacing) * pageRows);

	if (keyShift)
		this->selectShift(nextRow);
	else
		this->Select(nextRow);
}

void LSG_Cards::SelectNextRow(bool keyShift)
{
	if (!this->enabled || this->cards.empty())
		return;

	if (this->selectedRows.empty()) {
		this->SelectFirst();
		return;
	}

	auto selectedRow = this->selectedRows[this->selectedRows.size() - 1];
	auto nextRow     = (selectedRow + 1);
	auto maxRows     = (int)this->cards.size();

	if (nextRow >= maxRows)
		return;

	this->scrollOffsetY += (this->cardHeight + this->cardSpacing);

	if (keyShift)
		this->selectShift(nextRow);
	else
		this->Select(nextRow);
}

void LSG_Cards::SelectPreviousPage(bool keyShift)
{
	if (!this->enabled || this->cards.empty())
		return;

	if (this->selectedRows.empty()) {
		this->SelectFirst();
		return;
	}

	auto selectedRow = this->selectedRows[this->selectedRows.size() - 1];
	auto pageRows    = std::min(LSG_Cards::CardPageRows, selectedRow);
	auto previousRow = (selectedRow - pageRows);

	if ((selectedRow < 1) || (previousRow < 0))
		return;

	this->scrollOffsetY = std::max((this->scrollOffsetY - ((this->cardHeight + this->cardSpacing) * pageRows)), 0);

	if (keyShift)
		this->selectShift(previousRow);
	else
		this->Select(previousRow);
}

void LSG_Cards::SelectPreviousRow(bool keyShift)
{
	if (!this->enabled || this->cards.empty())
		return;

	if (this->selectedRows.empty()) {
		this->SelectFirst();
		return;
	}

	auto selectedRow = this->selectedRows[this->selectedRows.size() - 1];
	auto previousRow = (selectedRow - 1);

	if (previousRow < 0)
		return;

	this->scrollOffsetY = std::max((this->scrollOffsetY - (this->cardHeight + this->cardSpacing)), 0);

	if (keyShift)
		this->selectShift(previousRow);
	else
		this->Select(previousRow);
}

void LSG_Cards::SelectRow(int offset)
{
	if (!this->enabled || this->selectedRows.empty() || this->cards.empty())
		return;

	auto currentRow = this->selectedRows[0];
	auto nextRow    = std::max(0, std::min((int)(this->cards.size() - 1), (currentRow + offset)));

	this->Select(nextRow);

	if (this->selectedRows[0] < 0)
		return;

	auto areaBottom = (this->background.y + this->background.h);
	auto areaTop    = this->background.y;

	auto rowTop    = (areaTop + (this->selectedRows[0] * this->cardHeight));
	auto rowBottom = (rowTop + this->cardHeight);

	if ((rowBottom > areaBottom) || (rowTop < areaTop))
		this->scrollOffsetY = (this->selectedRows[0] * this->cardHeight);
}

void LSG_Cards::selectShift(int row)
{
	if (!this->enabled || (row < 0) || (row >= (int)this->cards.size()))
		return;

	if (this->selectedRows.empty()) {
		this->Select(row);
		return;
	}

	int start = this->selectedRows[0];

	this->selectedRows.clear();

	if (start <= row) {
		for (int i = start; i <= row; i++)
			this->selectedRows.push_back(i);
	} else {
		for (int i = start; i >= row; i--)
			this->selectedRows.push_back(i);
	}

	this->select(LSG_EVENT_ROW_SELECTED);
}

void LSG_Cards::sendEvent(LSG_EventType type) const
{
	if (!this->enabled)
		return;

	SDL_Event listEvent = {};

	listEvent.type       = SDL_RegisterEvents(1);
	listEvent.user.code  = (int)type;
	listEvent.user.data1 = (void*)strdup(this->id.c_str());
	listEvent.user.data2 = new std::vector(this->selectedRows);

	SDL_PushEvent(&listEvent);
}

void LSG_Cards::SetCard(int row, const LSG_CardItem& cardItem)
{
	if ((row < 0) || (row >= (int)this->cards.size()))
		return;

	this->destroyTextures(this->cards[row]);

	this->cards[row] = LSG_Cards::ToCard(cardItem);

	this->reset();
}

void LSG_Cards::SetCards(const LSG_CardItems& cardItems)
{
	this->destroyTextures();

	this->cards.clear();

	for (const auto& cardItem : cardItems)
		this->cards.push_back(LSG_Cards::ToCard(cardItem));

	this->reset(true);

	this->Select(!this->cards.empty() ? 0 : -1);
}

void LSG_Cards::SetCards()
{
	this->reset();
}

void LSG_Cards::setCards()
{
	for (auto& card : this->cards)
	{
		if (!card.thumbnail.filePath.empty() && !card.thumbnail.texture.texture)
		{
			auto filePath = LSG_Text::GetFullPath(card.thumbnail.filePath);
			auto surface  = IMG_Load(filePath.c_str());

			if (surface)
			{
				card.thumbnail.texture.size    = { surface->w, surface->h };
				card.thumbnail.texture.texture = LSG_Window::ToTexture(surface);

				SDL_FreeSurface(surface);
			}
		}

		if (!card.title.text.empty() && !card.title.texture.texture)
		{
			card.title.texture.texture = this->getTexture(card.title.text, (this->getFontSize() + 4));
			card.title.texture.size    = LSG_Graphics::GetTextureSize(card.title.texture.texture);
		}

		if (!card.description.text.empty() && !card.description.texture.texture)
		{
			card.description.texture.texture = this->getTexture(card.description.text);
			card.description.texture.size    = LSG_Graphics::GetTextureSize(card.description.texture.texture);
		}
	}

	if (this->renderTarget) {
		SDL_DestroyTexture(this->renderTarget);
		this->renderTarget = nullptr;
	}
}

LSG_Card LSG_Cards::ToCard(const LSG_CardItem& cardItem)
{
	LSG_Card card = {
		.description = {.text = cardItem.description },
		.thumbnail   = { .filePath = cardItem.thumbnail },
		.title       = { .text = cardItem.title }
	};

	return card;
}

LSG_CardItem LSG_Cards::ToCardItem(const LSG_Card& card)
{
	LSG_CardItem cardItem = {
		.title       = card.title.text,
		.description = card.description.text,
		.thumbnail   = card.thumbnail.filePath
	};

	return cardItem;
}
