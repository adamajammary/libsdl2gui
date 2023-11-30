#include "LSG_Image.h"

LSG_Image::LSG_Image(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->file        = "";
	this->fill        = false;
	this->orientation = {};

	auto attributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (attributes.contains("width")  ? attributes["width"]  : "");
	auto height = (attributes.contains("height") ? attributes["height"] : "");

	if (!width.empty() && (width[width.length() - 1] != '%'))
		this->size.width = LSG_Graphics::GetDPIScaled(std::atoi(width.c_str()));

	if (!height.empty() && (height[height.length() - 1] != '%'))
		this->size.height = LSG_Graphics::GetDPIScaled(std::atoi(height.c_str()));
}

void LSG_Image::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto background = this->getArea();

	if (this->size.width == 0)
		this->size.width = background.w;

	if (this->size.height == 0)
		this->size.height = background.h;

	auto textureSize    = this->getTextureSize();
	auto downscaledSize = LSG_Graphics::GetDownscaledSize(textureSize, this->size);

	if ((textureSize.width > downscaledSize.width) || (textureSize.height > downscaledSize.height)) {
		this->destroyTextures();
		this->texture = LSG_Graphics::GetTextureDownScaled(this->file, downscaledSize);
	}

	if (this->fill) {
		SDL_RenderCopy(renderer, this->texture, nullptr, &background);
		return;
	}

	auto      destination = LSG_Graphics::GetDestinationAligned(background, textureSize, this->getAlignment());
	SDL_Point center      = { (destination.w / 2), (destination.h / 2) };

	SDL_RenderCopyEx(renderer, this->texture, nullptr, &destination, this->orientation.rotation, &center, this->orientation.flip);
}

void LSG_Image::SetImage(const std::string& file, bool fill)
{
	if (file.empty())
		return;

	this->destroyTextures();

	this->file        = file;
	this->fill        = fill;
	this->orientation = LSG_Graphics::GetImageOrientation(file);
	this->texture     = LSG_Window::ToTexture(file);
}

void LSG_Image::SetImage()
{
	if (!this->file.empty() && this->texture)
		return;

	this->destroyTextures();

	auto attributes = LSG_XML::GetAttributes(this->xmlNode);
	auto xmlFile    = (attributes.contains("file") ? attributes["file"] : "");
	auto xmlFill    = (attributes.contains("fill") ? attributes["fill"] : "");

	this->file        = xmlFile;
	this->fill        = (xmlFill == "true");
	this->orientation = LSG_Graphics::GetImageOrientation(xmlFile);
	this->texture     = LSG_Window::ToTexture(xmlFile);
}
