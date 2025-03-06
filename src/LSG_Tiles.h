#include "main.h"

#ifndef LSG_TILES_H
#define LSG_TILES_H

struct LSG_TileRender
{
	SDL_Rect clip        = {};
	SDL_Rect destination = {};
};

struct LSG_TileTexture
{
	SDL_Texture* texture = nullptr;
	SDL_Size     size    = {};
};

struct LSG_TileImage
{
	std::string     filePath = "";
	LSG_TileTexture texture  = {};
};

struct LSG_TileText
{
	std::string     text    = "";
	LSG_TileTexture texture = {};
};

struct LSG_Tile
{
	SDL_Rect      background  = {};
	bool          highlighted = false;
	LSG_TileImage image       = {};
	LSG_TileText  text        = {};
};

class LSG_Tiles : public LSG_Pagination, public LSG_ScrollBar, public LSG_Text, public LSG_IEvent
{
public:
	LSG_Tiles(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Tiles() {}

public:
	static const int LayerOffset = 100000000;
	static const int TextPadding = 2;
	static const int TileBorder  = 2;

private:
	static inline const SDL_Color TextBackground = { 0, 0, 0, 196 };
	static inline const SDL_Color TileBackground = { 0, 0, 0,  64 };

private:
	SDL_Rect              fillArea;
	SDL_Rect              grid;
	int                   gridEnd;
	LSG_TileRender        image;
	int                   offset;
	int                   rows;
	std::vector<int>      selectedTiles;
	int                   spacing;
	LSG_TileRender        text;
	LSG_Alignment         textAlignment;
	int                   tileBorder;
	int                   tileSize;
	std::vector<LSG_Tile> tiles;
	int                   tilesPerRow;
	int                   totalSize;
	bool                  wrapTiles;
	std::string           xmlTileSize;

public:
	void             Activate();
	void             AddTile(const LSG_TileItem& tile);
	void             AddTile(LibXml::xmlNode* node);
	std::vector<int> GetSelectedTiles() const;
	SDL_Size         GetSize();
	LSG_TileItem     GetTile(int index);
	LSG_TileItems    GetTiles();
	virtual bool     OnMouseClick(const SDL_Point& mousePosition) override;
	void             OnMouseOver(const SDL_Point& mousePosition);
	void             RemoveTile(int index);
	void             Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void     Render(SDL_Renderer* renderer) override;
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
	void             SetTiles(bool resetScroll);

private:
	void          advanceToNextTile();
	void          calculateGridDimensions();
	void          clipTileX(const LSG_Tile& tile);
	void          clipTileY(const LSG_Tile& tile);
	void          destroyTextures(LSG_Tile& tile);
	virtual void  destroyTextures() override;
	SDL_Rect      getGrid();
	SDL_Rect      getImageDestination() const;
	int           getRowCount();
	int           getScrollOffsetX();
	int           getScrollOffsetY();
	int           getSelectedTile();
	LSG_Alignment getTextAlignment(const LSG_UMapStrStr& xmlAttributes);
	SDL_Rect      getTextDestination();
	int           getTileSize();
	int           getTilesPerRow();
	bool          isTextVisible() const;
	bool          isTileVisible() const;
	void          render(SDL_Renderer* renderer);
	void          renderHighlightSelection(SDL_Renderer* renderer, int index);
	void          renderImage(SDL_Renderer* renderer, const LSG_TileImage& image);
	void          renderScrollBar(SDL_Renderer* renderer);
	void          renderText(SDL_Renderer* renderer, const LSG_TileText& text);
	void          reset(bool resetScroll = true);
	void          resetScroll();
	virtual void  sendEvent(LSG_EventType type) override;
	void          selectCtrl(int index);
	void          selectShift(int index);
	void          setGrid();
	void          setTiles();
};

#endif
