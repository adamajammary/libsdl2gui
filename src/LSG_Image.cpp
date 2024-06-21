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

SDL_Size LSG_Image::GetSize()
{
	auto attributes  = this->GetXmlAttributes();
	auto textureSize = this->getTextureSize();

	auto border2x  = (this->border  + this->border);
	auto padding2x = (this->padding + this->padding);

	textureSize.width  += (padding2x + border2x);
	textureSize.height += (padding2x + border2x);

	if (attributes.contains("width") && (this->background.w > 0))
		textureSize.width = this->background.w;

	if (attributes.contains("height") && (this->background.h > 0))
		textureSize.height = this->background.h;

	return textureSize;
}

SDL_Size LSG_Image::GetTextureSize()
{
	return this->getTextureSize();
}

void LSG_Image::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	auto border2x    = (this->border  + this->border);
	auto padding2x   = (this->padding + this->padding);
	auto textureSize = this->getTextureSize();

	this->background.x = position.x;
	this->background.y = position.y;
	this->background.w = ((this->size.width  > 0 ? this->size.width  : textureSize.width)  + padding2x + border2x);
	this->background.h = ((this->size.height > 0 ? this->size.height : textureSize.height) + padding2x + border2x);

	this->render(renderer);
}

void LSG_Image::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Image::render(SDL_Renderer* renderer)
{
	LSG_Component::Render(renderer);

	if (!this->texture)
		return;

	auto fillArea = this->getArea(this->background);

	if (this->size.width == 0)
		this->size.width = fillArea.w;

	if (this->size.height == 0)
		this->size.height = fillArea.h;

	auto textureSize    = this->getTextureSize();
	auto downscaledSize = LSG_Graphics::GetDownscaledSize(textureSize, this->size);

	if ((textureSize.width > downscaledSize.width) || (textureSize.height > downscaledSize.height)) {
		this->destroyTextures();
		this->texture = LSG_Graphics::GetTextureDownScaled(this->file, downscaledSize);
	}

	if (this->fill) {
		SDL_RenderCopy(renderer, this->texture, nullptr, &fillArea);
		return;
	}

	auto      dest   = LSG_Graphics::GetDestinationAligned(fillArea, textureSize, this->getAlignment());
	SDL_Point center = { (dest.w / 2), (dest.h / 2) };

	SDL_RenderCopyEx(renderer, this->texture, nullptr, &dest, this->orientation.rotation, &center, this->orientation.flip);
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
