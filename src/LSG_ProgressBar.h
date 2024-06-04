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
	static inline const SDL_Color DefaultProgressColor   = { 20, 130, 255, 255 };

private:
	SDL_Color progressColor;
	double    value;

public:
	double       GetValue() const;
	virtual void Render(SDL_Renderer* renderer) override;
	virtual void SetColors() override;
	void         SetValue(double value);
};

#endif
