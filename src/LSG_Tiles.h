#include "main.h"

#ifndef LSG_TILES_H
#define LSG_TILES_H

struct LSG_TileRender
{
	SDL_Rect clip        = {};
	SDL_Rect destination = {};
};

struct LSG_Tile
{
	SDL_Rect      background  = {};
	bool          highlighted = false;
	LSG_ItemImage image       = {};
	LSG_ItemText  text        = {};
};

class LSG_Tiles : public LSG_Pagination, public LSG_ScrollBar, public LSG_Text, public LSG_IEvent
{
public:
	LSG_Tiles(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Tiles();

private:
	static const int DefaultSelectedBorderWidth = 2;
	static const int DefaultTextPadding         = 10;
	static const int DefaultTileSize            = 128;

private:
	static inline const SDL_Color DefaultTextBackground = { 0, 0, 0, 196 };

private:
	static std::mutex tilesLock;

private:
	SDL_Rect              fillArea;
	SDL_Rect              grid;
	int                   gridEnd;
	LSG_TileRender        image;
	int                   offset;
	int                   rows;
	int                   selectedBorderWidth;
	std::vector<int>      selectedTiles;
	int                   spacing;
	LSG_TileRender        text;
	LSG_Alignment         textAlignment;
	int                   textPadding;
	int                   tileSize;
	std::vector<LSG_Tile> tiles;
	int                   tilesPerRow;
	int                   totalSize;
	bool                  wrapTiles;
	std::string           xmlTileSize;

public:
	void             Activate() const;
	void             Activate(const SDL_Point& mousePosition) const;
	void             AddTile(const LSG_TileItem& tile);
	void             AddTile(LibXml::xmlNode* node);
	std::vector<int> GetSelectedTiles() const;
	SDL_Size         GetSize() const;
	LSG_TileItem     GetTile(int index) const;
	LSG_TileItems    GetTiles() const;
	size_t           GetTilesCount() const;
	void             OffsetBackgroundY(int headerHeight);
	virtual void     OnMouseClick(const SDL_Point& mousePosition) override;
	void             OnMouseOver(const SDL_Point& mousePosition);
	void             RemoveTile(int index);
	virtual void     Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	void             Render(SDL_Renderer* renderer);
	bool             Select(int index);
	bool             Select(const std::vector<int>& indices);
	void             SelectAll();
	void             SelectFirst(bool keyShift = false);
	void             SelectLast(bool keyShift = false);
	void             SelectNext(bool keyShift = false);
	void             SelectNextPage(bool keyShift = false);
	void             SelectNextRow(bool keyShift = false);
	void             SelectPrevious(bool keyShift = false);
	void             SelectPreviousPage(bool keyShift = false);
	void             SelectPreviousRow(bool keyShift = false);
	void             SetTile(int index, const LSG_TileItem& tile);
	void             SetTiles(const LSG_TileItems& tiles);
	void             SetTiles();

private:
	void          advanceToNextTile();
	void          calculateGridDimensions();
	void          clipTileX(const LSG_Tile& tile);
	void          clipTileY(const LSG_Tile& tile);
	void          destroySurfaces(LSG_Tile& tile);
	void          destroySurfaces();
	void          destroyTextures(LSG_Tile& tile);
	virtual void  destroyTextures() override;
	SDL_Rect      getGrid();
	int           getRowCount() const;
	int           getScrollOffsetX() const;
	int           getScrollOffsetY() const;
	int           getSelectedTile() const;
	LSG_Alignment getTextAlignment(const LSG_UMapStrStr& xmlAttributes) const;
	SDL_Rect      getTextDestination();
	int           getTileSize(int maxWidth) const;
	int           getTilesPerRow() const;
	bool          isTextVisible() const;
	bool          isTileVisible() const;
	void          render(SDL_Renderer* renderer);
	void          renderHighlightSelection(SDL_Renderer* renderer, int index);
	void          renderImage(SDL_Renderer* renderer, const LSG_ItemImage& image) const;
	void          renderScrollBar(SDL_Renderer* renderer);
	void          renderText(SDL_Renderer* renderer, const LSG_ItemText& text);
	void          reset(bool resetScroll = false);
	void          resetScroll();
	virtual void  sendEvent(LSG_EventType type) const override;
	void          selectCtrl(int index);
	void          selectShift(int index);
	void          setGrid();
	void          setTiles();
	void          setTileSurfaces();
	void          setTileSurfacesForTile(LSG_Tile& tile, std::latch& threadCount);
	void          setTileTextures();
};

#endif
