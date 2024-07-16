#include "main.h"

#ifndef LSG_TEXT_INPUT_H
#define LSG_TEXT_INPUT_H

class LSG_TextInput : public LSG_Text, public LSG_IEvent
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
	void         Copy();
	void         Delete();
	SDL_Size     GetSize();
	std::string  GetValue();
	void         Highlight(const SDL_Point& mousePosition);
	void         Input(const std::string& text);
	bool         IsHighlightedIconClear() const;
	void         MoveCursorEnd();
	void         MoveCursorHome();
	void         MoveCursorLeft();
	void         MoveCursorRight();
	virtual bool OnMouseClick(const SDL_Point& mousePosition) override;
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
	SDL_Rect     getIconClear(const SDL_Rect& fillArea);
	bool         isMouseOverIconClear(const SDL_Point& mousePosition);
	void         moveCursorTo(const SDL_Point& mousePosition);
	void         render(SDL_Renderer* renderer);
	void         renderCursor(SDL_Renderer* renderer, const SDL_Rect& background);
	void         renderHighlightedText(SDL_Renderer* renderer, const SDL_Rect& background);
	void         renderIconClear(SDL_Renderer* renderer, const SDL_Rect& icon);
	void         renderText(SDL_Renderer* renderer, const SDL_Rect& background);
	virtual void sendEvent(LSG_EventType type) override;
	void         setCursor();
	void         setIconClear();
	void         setPlaceholder();
	void         setValue();
	void         start();
};

#endif
