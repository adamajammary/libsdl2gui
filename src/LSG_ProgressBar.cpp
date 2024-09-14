#include "LSG_ProgressBar.h"

LSG_ProgressBar::LSG_ProgressBar(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->progressColor = {};
	this->value         = 0.0;

	auto attributes = this->GetXmlAttributes();

	if (attributes.contains("value"))
		this->value = std::atof(attributes["value"].c_str());

	auto progressColor = this->getXmlColor("progress-color");

	if (!progressColor.empty())
		this->progressColor = LSG_Graphics::ToSdlColor(progressColor);
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
	auto minHeight = LSG_Graphics::GetDPIScaled(LSG_ProgressBar::MinHeight);

	if (this->background.h < minHeight)
		this->background.h = minHeight;

	auto backgroundArea = SDL_Rect(this->background);
	auto fillArea       = this->getFillArea(backgroundArea, this->border);

	this->renderFill(renderer,   this->border, this->backgroundColor, backgroundArea);
	this->renderBorder(renderer, this->border, this->borderColor,     backgroundArea);

	auto progressValue = (int)((double)fillArea.w * this->value);
	auto progressArea  = SDL_Rect(fillArea);

	progressArea.w = progressValue;

	this->renderFill(renderer, 0, this->progressColor, progressArea);

	if (!this->enabled)
		this->renderDisabled(renderer);
}

void LSG_ProgressBar::SetColors()
{
	LSG_Component::SetColors();

	auto backgroundColor = this->getXmlColor("background-color", false);
	auto progressColor   = this->getXmlColor("progress-color");

	this->backgroundColor = (!backgroundColor.empty()  ? LSG_Graphics::ToSdlColor(backgroundColor)  : LSG_ProgressBar::DefaultBackgroundColor);
	this->progressColor   = (!progressColor.empty()    ? LSG_Graphics::ToSdlColor(progressColor)    : LSG_ProgressBar::DefaultProgressColor);
}

void LSG_ProgressBar::SetValue(double value)
{
	this->value = std::max(0.0, std::min(1.0, value));
}
