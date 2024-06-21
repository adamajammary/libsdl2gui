#include "LSG_Line.h"

LSG_Line::LSG_Line(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	auto orientation = LSG_XML::GetAttribute(xmlNode, "orientation");

	if (orientation == "vertical") {
		LSG_XML::SetAttribute(xmlNode, "width", "1");
		this->background.w = 1;
	} else {
		LSG_XML::SetAttribute(xmlNode, "height", "1");
		this->background.h = 1;
	}
}

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

void LSG_Line::render(SDL_Renderer* renderer)
{
	auto attributes  = LSG_XML::GetAttributes(xmlNode);
	auto color       = (attributes.contains("color") ? LSG_Graphics::ToSdlColor(attributes["color"]) : LSG_Graphics::GetThumbColor(this->backgroundColor));
	auto orientation = (attributes.contains("orientation") ? attributes["orientation"] : "");

	SDL_SetRenderDrawBlendMode(renderer, (color.a < 255 ? SDL_BLENDMODE_BLEND : SDL_BLENDMODE_NONE));
	SDL_SetRenderDrawColor(renderer,     color.r, color.g, color.b, color.a);

	LSG_Alignment alignment = { LSG_HALIGN_CENTER, LSG_VALIGN_MIDDLE };
	
	if (orientation == "vertical") {
		auto dest = LSG_Graphics::GetDestinationAligned(this->background, { 1, this->background.h }, alignment);

		SDL_RenderDrawLine(renderer, dest.x, dest.y, dest.x, (dest.y + dest.h - 1));
	} else {
		auto dest = LSG_Graphics::GetDestinationAligned(this->background, { this->background.w, 1 }, alignment);

		SDL_RenderDrawLine(renderer, dest.x, dest.y, (dest.x + dest.w - 1), dest.y);
	}
}
