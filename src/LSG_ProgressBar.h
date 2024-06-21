#include "main.h"

#ifndef LSG_PROGRESS_BAR_H
#define LSG_PROGRESS_BAR_H

class LSG_ProgressBar : public LSG_Component
{
public:
	LSG_ProgressBar(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_ProgressBar() {}

private:
	static inline const SDL_Color DefaultBackgroundColor = { 192, 192, 192, 255 };
	static inline const SDL_Color DefaultProgressColor   = { 82, 180, 30, 255 };

private:
	static const int MinHeight = 10;

private:
	SDL_Color progressColor;
	double    value;

public:
	double       GetValue() const;
	void         Render(SDL_Renderer* renderer, const SDL_Point& position);
	virtual void Render(SDL_Renderer* renderer) override;
	virtual void SetColors() override;
	void         SetValue(double value);

private:
	void render(SDL_Renderer* renderer);
};

#endif
