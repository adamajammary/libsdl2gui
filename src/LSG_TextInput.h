#include "main.h"

#ifndef LSG_TEXT_INPUT_H
#define LSG_TEXT_INPUT_H

class LSG_TextInput : public LSG_Text
{
public:
	LSG_TextInput(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_TextInput() {}

private:
	bool        active;
	size_t      cursorPosition;
	int         cursorTextWidth;
	bool        highlightedIconClear;
	int         highlightedTextLength;
	SDL_Size    highlightedTextSize;
	uint32_t    lastCursorActive;
	int         maxIconSize;
	std::string placeholder;
	SDL_Size    textSize;
	std::string value;

public:
	void         Clear();
	void         Complete();
	void         Copy() const;
	void         Delete();
	std::string  GetValue() const;
	void         Input(const std::string& text);
	bool         IsMouseOverIconClear(const SDL_Point& mousePosition);
	void         MoveCursorEnd();
	void         MoveCursorHome();
	void         MoveCursorLeft();
	void         MoveCursorRight();
	bool         OnMouseDown(const SDL_Point& mousePosition);
	bool         OnMouseMove(const SDL_Point& mousePosition);
	void         Paste();
	void         Remove();
	void         Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void Render(SDL_Renderer* renderer) override;
	void         SelectAll();
	void         SelectEnd();
	void         SelectHome();
	void         SelectLeft();
	void         SelectRight();
	void         SelectWord(const SDL_Point& mousePosition);
	void         SetText();
	void         SetValue(const std::string& text);
	void         Stop();

private:
	size_t       getCursorPosition(const SDL_Point& mousePosition);
	SDL_Rect     getIconClear(const SDL_Rect& fillArea) const;
	size_t       getIndex(int mousePositionX);
	void         moveCursorTo(const SDL_Point& mousePosition);
	void         render(SDL_Renderer* renderer);
	void         renderCursor(SDL_Renderer* renderer, const SDL_Rect& background);
	void         renderHighlightedText(SDL_Renderer* renderer, const SDL_Rect& background);
	void         renderIconClear(SDL_Renderer* renderer, const SDL_Rect& icon);
	void         renderText(SDL_Renderer* renderer, const SDL_Rect& background);
	void         setCursor();
	void         setIconClear();
	void         setPlaceholder();
	void         setValue();
	void         start();
};

#endif
