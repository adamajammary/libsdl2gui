#include "LSG_Image.h"

LSG_Image::LSG_Image(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->file = "";
	this->fill = false;
}

void LSG_Image::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto backgroundArea = this->getFillArea(this->background, this->border);

	if (this->fill) {
		SDL_RenderCopy(renderer, this->texture, nullptr, &backgroundArea);
		return;
	}

	auto size = this->GetTextureSize();
	auto dest = this->getRenderDestinationAligned(backgroundArea, size);

	SDL_RenderCopy(renderer, this->texture, nullptr, &dest);
}

void LSG_Image::SetImage(const std::string& file, bool fill)
{
	if (file.empty())
		return;

	this->destroyTextures();

	this->file    = file;
	this->texture = LSG_Window::ToTexture(this->file);
	this->fill    = fill;
}

void LSG_Image::SetImage()
{
	if (!this->file.empty() && this->texture)
		return;

	this->destroyTextures();

	auto attributes = LSG_XML::GetAttributes(this->xmlNode);
	auto xmlFile    = (attributes.contains("file") ? attributes["file"] : "");

	this->file = xmlFile;

	if (this->file.empty())
		return;

	auto xmlFill = (attributes.contains("fill") ? attributes["fill"] : "");

	this->texture = LSG_Window::ToTexture(this->file);
	this->fill    = (xmlFill == "true");
}
