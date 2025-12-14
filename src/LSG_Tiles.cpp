#include "LSG_Tiles.h"

LSG_Tiles::LSG_Tiles(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->fillArea      = {};
	this->grid          = {};
	this->gridEnd       = 0;
	this->image         = {};
	this->offset        = 0;
	this->rows          = 0;
	this->selectedTiles = {};
	this->text          = {};
	this->tileSize      = 0;
	this->tiles         = {};
	this->tilesPerRow   = 0;
	this->totalSize     = 0;
	this->wrap          = true;

	this->spacing    = LSG_Window::GetDPIScaled(this->GetSpacing());
	this->tileBorder = LSG_Window::GetDPIScaled(LSG_Tiles::TileBorder);

	auto attributes = LSG_XML::GetAttributes(xmlNode);

	this->textAlignment = this->getTextAlignment(attributes);

	this->wrapTiles = (attributes.contains("wrap") ? (attributes["wrap"] != "false") : true);

	this->xmlTileSize = (attributes.contains("tile-size") ? attributes["tile-size"] : "");
}

LSG_Tiles::~LSG_Tiles()
{
	this->destroyTextures();
}

void LSG_Tiles::Activate() const
{
	if (!this->selectedTiles.empty())
		this->sendEvent(LSG_EVENT_TILE_ACTIVATED);
}

void LSG_Tiles::Activate(const SDL_Point& mousePosition) const
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->tiles.empty() || this->selectedTiles.empty())
		return;

	for (auto& tile : this->tiles)
	{
		if (SDL_PointInRect(&mousePosition, &tile.background)) {
			this->sendEvent(LSG_EVENT_TILE_ACTIVATED);
			break;
		}
	}
}

void LSG_Tiles::AddTile(const LSG_TileItem& tile)
{
	this->tilesLock.lock();

	this->tiles.push_back({
		.image = { .filePath = tile.image },
		.text  = { .text     = tile.text  }
	});

	this->tilesLock.unlock();

	this->reset();
}

void LSG_Tiles::AddTile(LibXml::xmlNode* node)
{
	auto attributes = LSG_XML::GetAttributes(node);

	this->tiles.push_back({
		.image = { .filePath = (attributes.contains("image") ? attributes["image"] : "") },
		.text  = { .text     = (attributes.contains("text")  ? attributes["text"]  : "") }
	});
}

void LSG_Tiles::advanceToNextTile()
{
	if (!this->wrapTiles) {
		this->offset += (this->tileSize + this->spacing);
		return;
	}

	this->image.destination.x += (this->image.destination.w + this->spacing);
	this->text.destination.x   = this->image.destination.x;

	auto gridRight = (this->grid.x + this->grid.w);
	auto tileRight = (this->image.destination.x + this->image.destination.w);

	if ((tileRight <= gridRight))
		return;

	auto tileBottom = (this->image.destination.y + this->image.destination.h);

	this->image.destination.x = this->grid.x;
	this->text.destination.x  = this->image.destination.x;

	this->offset = (tileBottom + this->spacing);
}

void LSG_Tiles::calculateGridDimensions()
{
	this->tilesPerRow = this->getTilesPerRow();
	this->rows        = this->getRowCount();

	if (!this->wrapTiles)
		this->totalSize = (((this->tileSize + this->spacing) * this->tilesPerRow) - this->spacing);
	else
		this->totalSize = (((this->tileSize + this->spacing) * this->rows) - this->spacing);
}

void LSG_Tiles::clipTileX(const LSG_Tile& tile)
{
	auto imageSize = std::min(tile.image.texture.size.width, tile.image.texture.size.height);

	this->image.clip = {
		std::max(((tile.image.texture.size.width  - tile.image.texture.size.height) / 2), 0),
		std::max(((tile.image.texture.size.height - tile.image.texture.size.width)  / 2), 0),
		imageSize,
		imageSize
	};

	this->image.destination.x = this->offset;
	this->image.destination.w = this->tileSize;

	this->text.clip = { 0, 0, std::min(tile.text.texture.size.width, (this->tileSize - LSG_Tiles::TextPadding)), tile.text.texture.size.height };

	this->text.destination   = SDL_Rect(this->image.destination);
	this->text.destination.h = std::min((tile.text.texture.size.height + LSG_Tiles::TextPadding), this->tileSize);

	if (tile.text.text.empty() || !tile.text.texture.size.height)
		this->text.destination.h = 0;

	switch (this->textAlignment.valign) {
		case LSG_VALIGN_MIDDLE: this->text.destination.y += ((this->tileSize - this->text.destination.h) / 2); break;
		case LSG_VALIGN_BOTTOM: this->text.destination.y += (this->tileSize  - this->text.destination.h); break;
		default: break;
	}

	auto tileRight     = (this->image.destination.x + this->image.destination.w);
	auto overflowRight = std::max((tileRight - this->gridEnd), 0);

	if (overflowRight)
	{
		auto imageOverflowRight = (int)(((double)overflowRight / (double)this->tileSize) * (double)imageSize);

		this->image.clip.w        -= imageOverflowRight;
		this->image.destination.w -= overflowRight;

		this->text.destination.w = this->image.destination.w;
	}
	
	auto overflowLeft = std::max((this->grid.x - this->image.destination.x), 0);

	if (overflowLeft)
	{
		auto imageOverflowLeft = (int)(((double)overflowLeft / (double)this->tileSize) * (double)imageSize);

		this->image.clip.x        += imageOverflowLeft;
		this->image.destination.x += overflowLeft;

		this->image.clip.w        -= imageOverflowLeft;
		this->image.destination.w -= overflowLeft;

		this->text.destination.x = this->image.destination.x;
		this->text.destination.w = this->image.destination.w;
	}
}

void LSG_Tiles::clipTileY(const LSG_Tile& tile)
{
	auto imageSize = std::min(tile.image.texture.size.width, tile.image.texture.size.height);

	this->image.clip = {
		std::max(((tile.image.texture.size.width  - tile.image.texture.size.height) / 2), 0),
		std::max(((tile.image.texture.size.height - tile.image.texture.size.width)  / 2), 0),
		imageSize,
		imageSize
	};

	this->image.destination.y = this->offset;
	this->image.destination.h = this->tileSize;

	this->text.clip = { 0, 0, std::min(tile.text.texture.size.width, (this->image.destination.w - LSG_Tiles::TextPadding)), tile.text.texture.size.height };

	this->text.destination.y = this->image.destination.y;
	this->text.destination.h = std::min((tile.text.texture.size.height + LSG_Tiles::TextPadding), this->image.destination.h);

	if (tile.text.text.empty() || !tile.text.texture.size.height)
		this->text.destination.h = 0;

	switch (this->textAlignment.valign) {
		case LSG_VALIGN_MIDDLE: this->text.destination.y += ((this->image.destination.h - this->text.destination.h) / 2); break;
		case LSG_VALIGN_BOTTOM: this->text.destination.y += (this->image.destination.h  - this->text.destination.h); break;
		default: break;
	}

	auto tileBottom = (this->image.destination.y + this->image.destination.h);
	auto textBottom = (this->text.destination.y  + this->text.destination.h);

	auto overflowBottom     = std::max((tileBottom - this->gridEnd), 0);
	auto overflowTextBottom = std::max((textBottom - this->gridEnd), 0);

	if (overflowBottom)
	{
		auto imageOverflowBottom = (int)(((double)overflowBottom / (double)this->image.destination.h) * (double)imageSize);

		this->image.clip.h        -= imageOverflowBottom;
		this->image.destination.h -= overflowBottom;
	}

	if (overflowTextBottom) {
		this->text.clip.h        -= overflowTextBottom;
		this->text.destination.h -= overflowTextBottom;
	}
	
	auto overflowTop     = std::max((this->grid.y - this->image.destination.y), 0);
	auto overflowTextTop = std::max((this->grid.y - this->text.destination.y),  0);

	if (overflowTop)
	{
		auto imageOverflowTop = (int)(((double)overflowTop / (double)this->image.destination.h) * (double)imageSize);

		this->image.clip.y        += imageOverflowTop;
		this->image.destination.y += overflowTop;

		this->image.clip.h        -= imageOverflowTop;
		this->image.destination.h -= overflowTop;
	}

	if (overflowTextTop)
	{
		this->text.clip.y        += overflowTextTop;
		this->text.destination.y += overflowTextTop;

		this->text.clip.h        -= overflowTextTop;
		this->text.destination.h -= overflowTextTop;;
	}
}

void LSG_Tiles::destroySurfaces(LSG_Tile& tile)
{
	if (tile.image.surface) {
		SDL_FreeSurface(tile.image.surface);
		tile.image.surface = nullptr;
	}

	if (tile.text.surface) {
		SDL_FreeSurface(tile.text.surface);
		tile.text.surface = nullptr;
	}
}

void LSG_Tiles::destroySurfaces()
{
	for (auto& tile : this->tiles)
		this->destroySurfaces(tile);
}

void LSG_Tiles::destroyTextures(LSG_Tile& tile)
{
	if (tile.image.texture.texture) {
		SDL_DestroyTexture(tile.image.texture.texture);
		tile.image.texture.texture = nullptr;
	}

	if (tile.text.texture.texture) {
		SDL_DestroyTexture(tile.text.texture.texture);
		tile.text.texture.texture = nullptr;
	}
}

void LSG_Tiles::destroyTextures()
{
	for (auto& tile : this->tiles)
		this->destroyTextures(tile);
}

SDL_Rect LSG_Tiles::getGrid()
{
	if (!this->wrapTiles)
	{
		auto grid = LSG_Graphics::GetDestinationAligned(this->fillArea, { this->totalSize, this->tileSize }, this->getParentAlignment());

		this->image.destination = { grid.x, grid.y, this->tileSize, this->tileSize };
		this->text.destination  = SDL_Rect(this->image.destination);

		return grid;
	}

	auto tileSizeSpaced = (this->tileSize + this->spacing);
	auto totalWidth     = (this->rows > 1 ? this->fillArea.w : ((tileSizeSpaced * this->tilesPerRow) - this->spacing));

	auto grid = LSG_Graphics::GetDestinationAligned(this->fillArea, { totalWidth, this->totalSize }, this->getParentAlignment());

	this->image.destination    = { grid.x, grid.y, this->tileSize, this->tileSize };
	this->image.destination.w += (((grid.w + this->spacing) % tileSizeSpaced) / this->tilesPerRow);

	this->text.destination = SDL_Rect(this->image.destination);

	return grid;
}

int LSG_Tiles::getRowCount() const
{
	if (!this->wrapTiles)
		return (!this->tiles.empty() ? 1 : 0);

	return (((int)this->tiles.size() / this->tilesPerRow) + (((int)this->tiles.size() % this->tilesPerRow != 0) ? 1 : 0));
}

int LSG_Tiles::getScrollOffsetX() const
{
	return (this->showScrollX ? std::min(this->scrollOffsetX, (this->totalSize - this->fillArea.w)) : 0);
}

int LSG_Tiles::getScrollOffsetY() const
{
	return (this->showScrollY ? std::min(this->scrollOffsetY, (this->totalSize - this->fillArea.h)) : 0);
}

int LSG_Tiles::getSelectedTile() const
{
	return (!this->selectedTiles.empty() ? this->selectedTiles[this->selectedTiles.size() - 1] : -1);
}

std::vector<int> LSG_Tiles::GetSelectedTiles() const
{
	return this->selectedTiles;
}

SDL_Size LSG_Tiles::GetSize() const
{
	auto maxWidth = (this->background.w - this->border);

	auto tileCount = (int)this->tiles.size();
	auto tileSize  = this->getTileSize(maxWidth);

	SDL_Size size = {};

	if (!this->wrapTiles)
	{
		size.width  = (((tileSize + this->spacing) * tileCount) - this->spacing);
		size.height = tileSize;
	}
	else if (SDL_RectEmpty(&this->background))
	{
		size.width  = tileSize;
		size.height = (((tileSize + this->spacing) * tileCount) - this->spacing);
	}
	else
	{
		auto tilesPerRow = std::max(std::min((maxWidth / (tileSize + this->spacing)), tileCount), 1);
		auto rowCount    = ((tileCount / tilesPerRow) + ((tileCount % tilesPerRow != 0) ? 1 : 0));

		size.width  = maxWidth;
		size.height = (((tileSize + this->spacing) * rowCount) - this->spacing);
	}

	return size;
}

LSG_Alignment LSG_Tiles::getTextAlignment(const LSG_UMapStrStr& xmlAttributes) const
{
	auto halign = (xmlAttributes.contains("text-halign") ? xmlAttributes.at("text-halign") : "");
	auto valign = (xmlAttributes.contains("text-valign") ? xmlAttributes.at("text-valign") : "");

	LSG_Alignment alignment = { LSG_HALIGN_LEFT, LSG_VALIGN_TOP };

	if (halign == "center")
		alignment.halign = LSG_HALIGN_CENTER;
	else if (halign == "right")
		alignment.halign = LSG_HALIGN_RIGHT;

	if (valign == "middle")
		alignment.valign = LSG_VALIGN_MIDDLE;
	else if (valign == "bottom")
		alignment.valign = LSG_VALIGN_BOTTOM;

	return alignment;
}

SDL_Rect LSG_Tiles::getTextDestination()
{
	SDL_Rect textDestination = {
		(this->text.destination.x + LSG_Tiles::TextPadding),
		this->text.destination.y,
		this->text.clip.w,
		this->text.clip.h
	};

	if (this->wrapTiles)
	{
		switch (this->textAlignment.halign) {
			case LSG_HALIGN_CENTER: textDestination.x += ((this->image.destination.w - LSG_Tiles::TextPadding - textDestination.w) / 2); break;
			case LSG_HALIGN_RIGHT:  textDestination.x += (this->image.destination.w  - LSG_Tiles::TextPadding - textDestination.w); break;
			default: break;
		}

		return textDestination;
	}

	switch (this->textAlignment.halign) {
		case LSG_HALIGN_CENTER: textDestination.x += ((this->tileSize - LSG_Tiles::TextPadding - textDestination.w) / 2); break;
		case LSG_HALIGN_RIGHT:  textDestination.x += (this->tileSize  - LSG_Tiles::TextPadding - textDestination.w); break;
		default: break;
	}

	auto textRight     = (textDestination.x + textDestination.w);
	auto overflowRight = std::max((textRight - this->gridEnd), 0);

	if (overflowRight) {
		this->text.clip.w -= overflowRight;
		textDestination.w  = this->text.clip.w;
	}

	auto overflowLeft = std::max((this->grid.x - this->offset),  0);

	if (overflowLeft)
		textDestination.x -= overflowLeft;

	auto overflowTextLeft = std::max((this->grid.x - textDestination.x), 0);

	if (overflowTextLeft)
	{
		this->text.clip.x += overflowTextLeft;
		textDestination.x += overflowTextLeft;

		this->text.clip.w -= overflowTextLeft;
		textDestination.w -= overflowTextLeft;
	}

	return textDestination;
}

LSG_TileItem LSG_Tiles::GetTile(int index) const
{
	if ((index < 0) || (index >= (int)this->tiles.size()))
		return {};

	LSG_TileItem tile = {
		.image = this->tiles[index].image.filePath,
		.text  = this->tiles[index].text.text
	};

	return tile;
}

int LSG_Tiles::getTileSize(int maxWidth) const
{
	if (maxWidth < 1)
		return LSG_Window::GetDPIScaled(LSG_Tiles::DefaultTileSize);

	int tileSize;

	if (!this->xmlTileSize.empty() && (this->xmlTileSize[this->xmlTileSize.length() - 1] == '%'))
		tileSize = (int)((double)maxWidth * std::atof(this->xmlTileSize.c_str()) * 0.01);
	else if (!this->xmlTileSize.empty())
		tileSize = std::atoi(this->xmlTileSize.c_str());
	else
		tileSize = LSG_Tiles::DefaultTileSize;

	return LSG_Window::GetDPIScaled(tileSize);
}

LSG_TileItems LSG_Tiles::GetTiles() const
{
	LSG_TileItems tiles;

	for (const auto& tile : this->tiles)
		tiles.push_back({ .image = tile.image.filePath, .text = tile.text.text });

	return tiles;
}

size_t LSG_Tiles::GetTilesCount() const
{
	return this->tiles.size();
}

int LSG_Tiles::getTilesPerRow() const
{
	int tilesPerRow;

	if (!this->wrapTiles)
		tilesPerRow = (int)this->tiles.size();
	else
		tilesPerRow = std::min((this->fillArea.w / (this->tileSize + this->spacing)), (int)this->tiles.size());

	return std::max(tilesPerRow, 1);
}

bool LSG_Tiles::isTextVisible() const
{
	if (!this->wrapTiles)
		return true;

	auto textBottom = (this->text.destination.y + this->text.destination.h);

	if ((textBottom < this->grid.y) || (this->text.destination.y >= this->gridEnd))
		return false;

	return true;
}

bool LSG_Tiles::isTileVisible() const
{
	if (!this->wrapTiles)
	{
		auto imageRight = (this->image.destination.x + this->image.destination.w);

		if (imageRight < this->grid.x)
			return false;
	}

	auto imageBottom = (this->image.destination.y + this->image.destination.h);

	if (imageBottom < this->grid.y)
		return false;

	return true;
}

bool LSG_Tiles::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || LSG_Events::IsMouseDown() || this->tiles.empty())
		return false;

	for (int i = 0; i < (int)this->tiles.size(); i++)
	{
		if (!SDL_PointInRect(&mousePosition, &this->tiles[i].background))
			continue;

		auto keyState = SDL_GetKeyboardState(nullptr);
		
		if (keyState[SDL_SCANCODE_LCTRL] || keyState[SDL_SCANCODE_RCTRL])
			this->selectCtrl(i);
		else if (keyState[SDL_SCANCODE_LSHIFT] || keyState[SDL_SCANCODE_RSHIFT])
			this->selectShift(i);
		else
			this->Select(i);

		break;
	}

	return true;
}

void LSG_Tiles::OnMouseOver(const SDL_Point& mousePosition)
{
	if (!this->enabled)
		return;

	bool isHighlighted = false;

	for (auto& tile : this->tiles)
	{
		if (isHighlighted) {
			tile.highlighted = false;
			continue;
		}

		isHighlighted    = SDL_PointInRect(&mousePosition, &tile.background);
		tile.highlighted = isHighlighted;
	}
}

void LSG_Tiles::RemoveTile(int index)
{
	if ((index < 0) || (index >= (int)this->tiles.size()))
		return;

	this->destroyTextures(this->tiles[index]);

	this->tilesLock.lock();

	this->tiles.erase(this->tiles.begin() + (size_t)index);

	this->tilesLock.unlock();

	this->reset();

	this->Select(!this->tiles.empty() && !this->selectedTiles.empty() ? this->selectedTiles[0] : -1);
}

void LSG_Tiles::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto textureSize = this->GetSize();

	this->background.x = position.x;
	this->background.y = position.y;
	this->background.w = textureSize.width;
	this->background.h = textureSize.height;

	this->setTileTextures();

	this->setGrid();

	this->render(renderer);
}

void LSG_Tiles::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	this->setTileTextures();

	this->setGrid();

	this->render(renderer);
}

void LSG_Tiles::render(SDL_Renderer* renderer)
{
	LSG_Component::Render(renderer);

	if (this->tiles.empty())
		return;

	auto scrollBarSize2x = LSG_ScrollBar::GetSize2x();

	if (this->fillArea.h < scrollBarSize2x)
		return;

	for (int i = 0; i < (int)this->tiles.size(); i++)
	{
		if (this->offset >= this->gridEnd)
			break;

		if (!this->wrapTiles)
			this->clipTileX(this->tiles[i]);
		else
			this->clipTileY(this->tiles[i]);

		this->tiles[i].background = SDL_Rect(this->image.destination);

		if (this->isTileVisible())
		{
			this->renderImage(renderer, this->tiles[i].image);
			this->renderText(renderer,  this->tiles[i].text);

			this->renderHighlightSelection(renderer, i);
		}

		this->advanceToNextTile();
	}

	this->renderScrollBar(renderer);
}

void LSG_Tiles::renderHighlightSelection(SDL_Renderer* renderer, int index)
{
	if (!this->enabled || (index < 0) || (index >= (int)this->tiles.size()))
		return;

	auto tileIter      = std::find(this->selectedTiles.begin(), this->selectedTiles.end(), index);
	bool isSelected    = (tileIter != this->selectedTiles.end());
	bool isHighlighted = (this->tiles[index].highlighted && this->highlighted);

	if (!isSelected && !isHighlighted)
		return;

	auto color = LSG_Graphics::GetInverseColor(this->backgroundColor);

	if (isSelected)
		LSG_Graphics::RenderBorder(renderer, this->tileBorder, { color.r, color.g, color.b, 255 }, this->image.destination);

	if (isHighlighted)
		LSG_Graphics::RenderFill(renderer, 0, { color.r, color.g, color.b, 64 }, this->image.destination);
}

void LSG_Tiles::renderImage(SDL_Renderer* renderer, const LSG_TileImage& image) const
{
	LSG_Graphics::RenderFill(renderer, 0, { 0, 0, 0, 128 }, this->image.destination);

	if (!image.texture.texture)
		return;

	auto destination = SDL_Rect(this->image.destination);

	if (this->text.destination.h > 0)
	{
		switch (this->textAlignment.valign) {
		case LSG_VALIGN_TOP:
			destination.y += this->text.destination.h;
			destination.h -= this->text.destination.h;
			break;
		case LSG_VALIGN_BOTTOM:
			destination.h -= this->text.destination.h;
			break;
		default:
			break;
		}
	}

	SDL_RenderCopy(renderer, image.texture.texture, &this->image.clip, &destination);
}

void LSG_Tiles::renderText(SDL_Renderer* renderer, const LSG_TileText& text)
{
	if (text.text.empty() || !this->isTextVisible())
		return;

	LSG_Graphics::RenderFill(renderer, 0, LSG_Tiles::TextBackground, this->text.destination);

	if (!text.texture.texture)
		return;

	auto destination = this->getTextDestination();

	SDL_RenderCopy(renderer, text.texture.texture, &this->text.clip, &destination);
}

void LSG_Tiles::renderScrollBar(SDL_Renderer* renderer)
{
	if (this->showScrollX)
	{
		this->grid.h += LSG_ScrollBar::GetSize();

		this->renderScrollBarHorizontal(renderer, this->grid, this->totalSize, this->backgroundColor, this->highlighted);
	}
	else if (this->showScrollY)
	{
		this->grid.w += LSG_ScrollBar::GetSize();

		this->renderScrollBarVertical(renderer, this->grid, this->totalSize, this->backgroundColor, this->highlighted);
	}
}

void LSG_Tiles::rotate(LSG_TileImage& image)
{
	auto exif        = LSG_Exif::Get(LSG_Text::GetFullPath(image.filePath));
	auto orientation = LSG_Exif::GetOrientation(exif.tags);

	if (orientation.rotation > 0.0)
	{
		auto maxSize = std::max(image.surface->w, image.surface->h);

		image.texture.size    = { maxSize, maxSize };
		image.texture.texture = LSG_Window::RotateTexture(image.texture.texture, orientation, image.texture.size, image.surface->format->format);
	}
}

void LSG_Tiles::reset(bool resetScroll)
{
	if (resetScroll)
		this->resetScroll();

	this->destroyTextures();

	this->setTiles();
}

void LSG_Tiles::resetScroll()
{
	if (!this->wrapTiles)
		this->scrollOffsetX = 0;
	else
		this->scrollOffsetY = 0;
}

bool LSG_Tiles::Select(int index)
{
	if (!this->enabled || (index >= (int)this->tiles.size()))
		return false;

	if (index < 0) {
		this->selectedTiles.clear();
		this->sendEvent(LSG_EVENT_TILE_UNSELECTED);
	} else {
		this->selectedTiles = { index };
		this->sendEvent(LSG_EVENT_TILE_SELECTED);
	}

	return true;
}

bool LSG_Tiles::Select(const std::vector<int>& indices)
{
	if (!this->enabled)
		return false;

	this->selectedTiles.clear();

	for (auto index : indices) {
		if ((index >= 0) && (index < (int)this->tiles.size()))
			this->selectedTiles.push_back(index);
	}

	this->sendEvent(this->selectedTiles.empty() ? LSG_EVENT_TILE_UNSELECTED : LSG_EVENT_TILE_SELECTED);

	return true;
}

void LSG_Tiles::SelectAll()
{
	if (!this->enabled || this->tiles.empty())
		return;

	this->resetScroll();

	this->selectedTiles.clear();

	for (int i = 0; i < (int)this->tiles.size(); i++)
		this->selectedTiles.push_back(i);

	this->sendEvent(LSG_EVENT_TILE_SELECTED);
}

void LSG_Tiles::selectCtrl(int index)
{
	if (!this->enabled || (index < 0) || (index >= (int)this->tiles.size()))
		return;

	auto tileIter   = std::find(this->selectedTiles.begin(), this->selectedTiles.end(), index);
	bool tileExists = (tileIter != this->selectedTiles.end());
		
	if (!tileExists)
		this->selectedTiles.push_back(index);
	else
		this->selectedTiles.erase(tileIter);

	this->sendEvent(!tileExists ? LSG_EVENT_TILE_SELECTED : LSG_EVENT_TILE_UNSELECTED);
}

void LSG_Tiles::SelectFirst(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	this->resetScroll();

	if (keyShift)
		this->selectShift(0);
	else
		this->Select(0);
}

void LSG_Tiles::SelectLast(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	if (!this->wrapTiles)
		this->scrollOffsetX = LSG_ConstTexture::MaxSize;
	else
		this->scrollOffsetY = LSG_ConstTexture::MaxSize;

	auto last = ((int)this->tiles.size() - 1);

	if (keyShift)
		this->selectShift(last);
	else
		this->Select(last);
}

void LSG_Tiles::SelectNext(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	if (this->selectedTiles.empty()) {
		this->SelectFirst();
		return;
	}

	auto next           = (this->selectedTiles[this->selectedTiles.size() - 1] + 1);
	auto tileSizeSpaced = (this->tileSize + this->spacing);

	if (!this->wrapTiles)
	{
		auto maxTilesInView = std::min((this->fillArea.w / tileSizeSpaced), (int)this->tiles.size());

		if ((next % maxTilesInView) == 0)
			this->scrollOffsetX = (tileSizeSpaced * next);
	} else if ((next % this->tilesPerRow) == 0) {
		this->scrollOffsetY += tileSizeSpaced;
	}

	if (keyShift)
		this->selectShift(next);
	else
		this->Select(next);
}

void LSG_Tiles::SelectNextPage(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	if (this->selectedTiles.empty()) {
		this->SelectFirst();
		return;
	}

	if (this->rows < 2)
		return;

	auto extraTiles = ((int)this->tiles.size() % this->tilesPerRow);
	auto maxTiles   = ((this->tilesPerRow * (this->rows + 1)) + (this->tilesPerRow - extraTiles));
	auto next       = (this->getSelectedTile() + (this->tilesPerRow * 2));

	if ((next >= (int)this->tiles.size()) && (next < maxTiles))
		next = ((int)this->tiles.size() - 1);

	this->scrollOffsetY += ((this->tileSize + this->spacing) * 2);

	if (keyShift)
		this->selectShift(next);
	else
		this->Select(next);
}

void LSG_Tiles::SelectNextRow(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	if (this->selectedTiles.empty()) {
		this->SelectFirst();
		return;
	}

	auto maxTiles = (this->tilesPerRow * this->rows);
	auto next     = (this->getSelectedTile() + this->tilesPerRow);

	if ((next >= (int)this->tiles.size()) && (next < maxTiles))
		next = ((int)this->tiles.size() - 1);

	this->scrollOffsetY += (this->tileSize + this->spacing);

	if (keyShift)
		this->selectShift(next);
	else
		this->Select(next);
}

void LSG_Tiles::SelectPrevious(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	if (this->selectedTiles.empty()) {
		this->SelectFirst();
		return;
	}

	auto current  = this->getSelectedTile();
	auto previous = (current - 1);

	if (previous < 0)
		return;

	auto tileSizeSpaced = (this->tileSize + this->spacing);

	if (!this->wrapTiles)
	{
		auto maxTilesInView = std::min((this->fillArea.w / tileSizeSpaced), (int)this->tiles.size());

		if ((previous % maxTilesInView) == (maxTilesInView - 1))
			this->scrollOffsetX = std::max((tileSizeSpaced * (current - maxTilesInView)), 0);
	} else if ((previous % this->tilesPerRow) == (this->tilesPerRow - 1)) {
		this->scrollOffsetY = std::max((this->scrollOffsetY - tileSizeSpaced), 0);
	}

	if (keyShift)
		this->selectShift(previous);
	else
		this->Select(previous);
}

void LSG_Tiles::SelectPreviousPage(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	if (this->selectedTiles.empty()) {
		this->SelectFirst();
		return;
	}

	if (this->rows < 2)
		return;

	auto maxTiles = (this->tilesPerRow * 2);
	auto previous = (this->getSelectedTile() - maxTiles);

	if ((previous < 0) && (previous > -maxTiles))
		previous = 0;

	if (previous < 0)
		return;

	this->scrollOffsetY = std::max((this->scrollOffsetY - ((this->tileSize + this->spacing) * 2)), 0);

	if (keyShift)
		this->selectShift(previous);
	else
		this->Select(previous);
}

void LSG_Tiles::SelectPreviousRow(bool keyShift)
{
	if (!this->enabled || this->tiles.empty())
		return;

	if (this->selectedTiles.empty()) {
		this->SelectFirst();
		return;
	}

	auto previous = (this->getSelectedTile() - this->tilesPerRow);

	if (previous < 0)
		return;

	this->scrollOffsetY = std::max((this->scrollOffsetY - (this->tileSize + this->spacing)), 0);

	if (keyShift)
		this->selectShift(previous);
	else
		this->Select(previous);
}

void LSG_Tiles::selectShift(int index)
{
	if (!this->enabled || (index < 0) || (index >= (int)this->tiles.size()))
		return;

	if (this->selectedTiles.empty())
	{
		this->selectedTiles = { index };
		
		this->sendEvent(LSG_EVENT_TILE_SELECTED);

		return;
	}

	int start = this->selectedTiles[0];

	this->selectedTiles.clear();

	if (start <= index) {
		for (int i = start; i <= index; i++)
			this->selectedTiles.push_back(i);
	} else {
		for (int i = start; i >= index; i--)
			this->selectedTiles.push_back(i);
	}

	this->sendEvent(LSG_EVENT_TILE_SELECTED);
}

void LSG_Tiles::sendEvent(LSG_EventType type) const
{
	if (!this->enabled)
		return;

	SDL_Event listEvent = {};

	listEvent.type       = SDL_RegisterEvents(1);
	listEvent.user.code  = (int)type;
	listEvent.user.data1 = (void*)strdup(this->id.c_str());
	listEvent.user.data2 = new std::vector(this->selectedTiles);

	SDL_PushEvent(&listEvent);
}

void LSG_Tiles::setGrid()
{
	this->fillArea = LSG_Graphics::GetFillArea(this->background, this->border);
	this->tileSize = this->getTileSize(this->fillArea.w);

	this->calculateGridDimensions();

	if (!this->wrapTiles)
		this->showScrollX = (this->totalSize > this->fillArea.w);
	else
		this->showScrollY = (this->totalSize > this->fillArea.h);

	if (this->showScrollX)
	{
		this->fillArea.h -= LSG_ScrollBar::GetSize();

		this->tileSize = this->getTileSize(this->fillArea.w);

		this->calculateGridDimensions();
	}
	else if (this->showScrollY)
	{
		this->fillArea.w -= LSG_ScrollBar::GetSize();

		this->calculateGridDimensions();
	}

	if (!this->wrapTiles)
		this->scrollOffsetX = this->getScrollOffsetX();
	else
		this->scrollOffsetY = this->getScrollOffsetY();

	this->grid = this->getGrid();

	if (!this->wrapTiles) {
		this->gridEnd = (this->grid.x + this->grid.w);
		this->offset  = (this->grid.x - this->scrollOffsetX);
	} else {
		this->gridEnd = (this->grid.y + this->grid.h);
		this->offset  = (this->grid.y - this->scrollOffsetY);
	}
}

void LSG_Tiles::SetTile(int index, const LSG_TileItem& tile)
{
	if ((index < 0) || (index >= (int)this->tiles.size()))
		return;

	this->tilesLock.lock();

	this->tiles[index] = {
		.image = { .filePath = tile.image },
		.text  = { .text     = tile.text  }
	};

	this->tilesLock.unlock();

	this->reset();
}

void LSG_Tiles::SetTiles(const LSG_TileItems& tiles)
{
	this->destroyTextures();

	this->tilesLock.lock();

	this->tiles.clear();

	for (const auto& tile : tiles)
	{
		this->tiles.push_back({
			.image = { .filePath = tile.image },
			.text  = { .text     = tile.text  }
		});
	}

	this->tilesLock.unlock();

	this->reset(true);

	this->Select(!this->tiles.empty() ? 0 : -1);
}

void LSG_Tiles::SetTiles()
{
	this->reset();
}

void LSG_Tiles::setTiles()
{
	std::thread(&LSG_Tiles::setTileSurfaces, this).detach();
}

void LSG_Tiles::setTileSurfaces()
{
	this->tilesLock.lock();

	this->destroySurfaces();

	for (auto& tile : this->tiles)
	{
		if (!tile.image.filePath.empty())
			tile.image.surface = IMG_Load(LSG_Text::GetFullPath(tile.image.filePath).c_str());

		if (!tile.text.text.empty())
			tile.text.surface = this->getSurface(tile.text.text);
	}

	this->tilesLock.unlock();
}

void LSG_Tiles::setTileTextures()
{
	this->tilesLock.lock();

	for (auto& tile : this->tiles)
	{
		if (!tile.image.filePath.empty() && !tile.image.texture.texture && tile.image.surface)
		{
			tile.image.texture.size    = { tile.image.surface->w, tile.image.surface->h };
			tile.image.texture.texture = LSG_Window::ToTexture(tile.image.surface);

			this->rotate(tile.image);
		}

		if (!tile.text.text.empty() && !tile.text.texture.texture && tile.text.surface)
		{
			tile.text.texture.size    = { tile.text.surface->w, tile.text.surface->h };
			tile.text.texture.texture = LSG_Window::ToTexture(tile.text.surface);
		}

		this->destroySurfaces(tile);
	}

	this->tilesLock.unlock();
}
