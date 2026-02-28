#include "LSG_ProgressBar.h"

LSG_ProgressBar::LSG_ProgressBar(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->lastProgressValue = 0;
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

	auto progressValue = (int)((double)fillArea.w * this->value);
	auto progressArea  = SDL_Rect(fillArea);

	progressArea.w = progressValue;

	this->renderProgress(renderer, progressArea, progressValue, fillArea);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_ProgressBar::renderProgress(SDL_Renderer* renderer, const SDL_Rect& progressArea, int progressValue, const SDL_Rect& fillArea)
{
	if (this->borderRadius > 0)
	{
		auto textureId = std::format("{}_progress_fill", this->id);

		if (progressValue != this->lastProgressValue)
		{
			LSG_Graphics::DestroyTexture(textureId);

			this->lastProgressValue = progressValue;
		}

		LSG_Graphics::RenderFillRounded(
			renderer,
			this->borderRadius,
			this->progressColor,
			progressArea,
			textureId
		);

		if (progressArea.w < (fillArea.w - this->borderRadius))
		{
			SDL_Rect progressArea2 = progressArea;

			progressArea2.x += (progressValue - this->borderRadius);
			progressArea2.w  = this->borderRadius;

			LSG_Graphics::RenderFill(renderer, this->borderWidth, this->progressColor, progressArea2);
		}
	} else {
		LSG_Graphics::RenderFill(renderer, this->borderWidth, this->progressColor, progressArea);
	}
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
