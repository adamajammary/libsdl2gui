#include "main.h"

#ifndef LSG_PROGRESS_BAR_H
#define LSG_PROGRESS_BAR_H

class LSG_ProgressBar : public LSG_Component
{
public:
	LSG_ProgressBar(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_ProgressBar() {}

protected:
	static inline const SDL_Color DefaultBackgroundColor = { 192, 192, 192, 255 };
	static inline const SDL_Color DefaultProgressColor   = { 82, 180, 30, 255 };

protected:
	SDL_Color progressColor;
	double    value;

public:
	double       GetValue() const;
	virtual void Render(SDL_Renderer* renderer, const SDL_Point& position) override;
	virtual void Render(SDL_Renderer* renderer) const override;
	virtual void SetColors() override;
	void         SetValue(double value);

protected:
	void renderProgress(SDL_Renderer* renderer, const SDL_Rect& progressArea, int progressValue, const SDL_Rect& fillArea) const;

private:
	void render(SDL_Renderer* renderer) const;
};

#endif
