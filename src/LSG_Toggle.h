#include "main.h"

#ifndef LSG_TOGGLE_H
#define LSG_TOGGLE_H

class LSG_Toggle : public LSG_Component, public LSG_IEvent
{
public:
	LSG_Toggle(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Toggle() {}

private:
	bool on;

public:
	bool         IsMouseOver(const SDL_Point& mousePosition) const;
	bool         IsOn() const;
	virtual void OnMouseClick(const SDL_Point& mousePosition) override;
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	virtual void Render(SDL_Renderer* renderer) const override;
	void         Set(bool on);
	void         Set();

private:
	SDL_Rect     getDestination() const;
	SDL_Size     getMaxSize(const SDL_Rect& background) const;
	void         render(SDL_Renderer* renderer) const;
	virtual void sendEvent(LSG_EventType type) const override;
	void         toggle();
};

#endif
