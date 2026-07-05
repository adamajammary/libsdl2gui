#include "LSG_Line.h"

LSG_Line::LSG_Line(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{}

void LSG_Line::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_Line::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Line::render(SDL_Renderer* renderer) const
{
	auto xmlColor = LSG_XML::GetAttribute(this->xmlNode, "color");
	auto color    = (!xmlColor.empty() ? LSG_Graphics::ToSdlColor(xmlColor) : LSG_Graphics::GetThumbColor(this->backgroundColor));

	SDL_SetRenderDrawBlendMode(renderer, (color.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer,     color.r, color.g, color.b, color.a);

	LSG_Alignment alignment = { LSG_HALIGN_CENTER, LSG_VALIGN_MIDDLE };
	
	if (this->IsVertical())
	{
		auto dest = LSG_Graphics::GetDestinationAligned(this->background, { 1, this->background.h }, alignment);

		SDL_RenderDrawLine(renderer, dest.x, dest.y, dest.x, (dest.y + dest.h - 1));
	}
	else
	{
		auto dest = LSG_Graphics::GetDestinationAligned(this->background, { this->background.w, 1 }, alignment);

		SDL_RenderDrawLine(renderer, dest.x, dest.y, (dest.x + dest.w - 1), dest.y);
	}
}
