#include "LSG_ProgressBar.h"

LSG_ProgressBar::LSG_ProgressBar(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->lastProgressWidth = 0;
	this->progressColor     = {};
	this->value             = 0.0;

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("value"))
		this->value = std::atof(attributes["value"].c_str());

	this->SetColors();
}

double LSG_ProgressBar::GetValue() const
{
	return this->value;
}

void LSG_ProgressBar::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_ProgressBar::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_ProgressBar::render(SDL_Renderer* renderer)
{
	LSG_Component::Render(renderer);

	auto fillArea = LSG_Graphics::GetFillArea(this->background, this->borderWidth);

	auto progressWidth = (int)((double)fillArea.w * this->value);
	auto progressArea  = SDL_Rect(fillArea);

	progressArea.w = progressWidth;

	this->renderProgress(renderer, progressArea, progressWidth, fillArea);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_ProgressBar::renderProgress(SDL_Renderer* renderer, const SDL_Rect& progressArea, int progressWidth, const SDL_Rect& fillArea)
{
	if (this->borderRadius <= 0) {
		LSG_Graphics::RenderFill(renderer, this->borderWidth, this->progressColor, progressArea);
		return;
	}

	auto textureId = std::format("{}_fill", this->id);

	if (progressWidth != this->lastProgressWidth)
	{
		LSG_Graphics::DestroyTexture(textureId);

		this->lastProgressWidth = progressWidth;
	}

	bool isVertical   = this->IsVertical();
	auto progressSize = isVertical ? progressArea.h : progressArea.w;
	auto fillSize     = isVertical ? fillArea.h     : fillArea.w;

	if (progressSize >= (fillSize - this->borderRadius))
		LSG_Graphics::RenderFillRounded(renderer, this->borderRadius, this->progressColor, progressArea, textureId);
	else if (isVertical)
		LSG_Graphics::RenderFillRoundedBottom(renderer, this->borderRadius, this->progressColor, progressArea, textureId);
	else
		LSG_Graphics::RenderFillRoundedLeft(renderer, this->borderRadius, this->progressColor, progressArea, textureId);
}

void LSG_ProgressBar::SetColors()
{
	LSG_Component::SetColors();

	auto backgroundColor = this->getXmlColor("background-color", false);
	auto progressColor   = this->getXmlColor("progress-color",   false);

	this->backgroundColor = (!backgroundColor.empty()  ? LSG_Graphics::ToSdlColor(backgroundColor)  : LSG_ProgressBar::DefaultBackgroundColor);
	this->progressColor   = (!progressColor.empty()    ? LSG_Graphics::ToSdlColor(progressColor)    : LSG_ProgressBar::DefaultProgressColor);
}

void LSG_ProgressBar::SetValue(double value)
{
	this->value = std::max(0.0, std::min(1.0, value));
}
