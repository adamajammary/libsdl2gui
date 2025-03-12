#include "LSG_Image.h"

LSG_Image::LSG_Image(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->downscaledTexture = nullptr;
	this->file              = "";
	this->fill              = false;
	this->imageSize         = {};
	this->orientation       = {};

	auto attributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (attributes.contains("width")  ? attributes["width"]  : "");
	auto height = (attributes.contains("height") ? attributes["height"] : "");

	if (!width.empty() && (width[width.length() - 1] != '%'))
		this->imageSize.width = LSG_Graphics::GetDPIScaled(std::atoi(width.c_str()));

	if (!height.empty() && (height[height.length() - 1] != '%'))
		this->imageSize.height = LSG_Graphics::GetDPIScaled(std::atoi(height.c_str()));
}

LSG_Image::~LSG_Image()
{
	this->destroyTextures();
}

void LSG_Image::destroyTextures()
{
	LSG_Component::destroyTextures();

	if (this->downscaledTexture) {
		SDL_DestroyTexture(this->downscaledTexture);
		this->downscaledTexture = nullptr;
	}
}

SDL_Size LSG_Image::GetImageSize() const
{
	if (!this->parent)
		return this->imageSize;
	
	SDL_Size imageSize = {
		std::min(this->imageSize.width,  this->parent->background.w),
		std::min(this->imageSize.height, this->parent->background.h),
	};

	return imageSize;
}

SDL_Size LSG_Image::getMaxSize(const SDL_Rect& background)
{
	auto textureSize = this->getTextureSize();

	SDL_FPoint textureScaleFactor = {
		((float)textureSize.width  / (float)textureSize.height),
		((float)textureSize.height / (float)textureSize.width)
	};

	SDL_Size size;

	if ((this->imageSize.width > 0) && (this->imageSize.height > 0))
		size = SDL_Size(this->imageSize);
	else if (this->imageSize.width > 0)
		size = { this->imageSize.width, (int)(textureScaleFactor.y * (float)this->imageSize.width) };
	else if (this->imageSize.height > 0)
		size = { (int)(textureScaleFactor.x * (float)this->imageSize.height), this->imageSize.height };
	else
		size = textureSize;

	SDL_FPoint scaleFactor = {
		((float)size.width  / (float)size.height),
		((float)size.height / (float)size.width)
	};

	SDL_Size maxSize;

	if (background.w >= background.h)
	{
		auto height = std::min(size.height, background.h);

		if ((this->orientation.rotation == 90.0) || (this->orientation.rotation == 270.0))
			maxSize = { .width  = height, .height = (int)(scaleFactor.y * (float)height) };
		else
			maxSize = { .width  = (int)(scaleFactor.x * (float)height), .height = height };
	}
	else
	{
		auto width = std::min(size.width, background.w);

		maxSize = { .width  = width, .height = (int)(scaleFactor.y * (float)width) };
	}

    if (maxSize.width > background.w)
    {
        maxSize.width  = background.w;
        maxSize.height = (int)(scaleFactor.y * (float)background.w);
    }
    else if (maxSize.height > background.h)
    {
        maxSize.height = background.h;
        maxSize.width  = (int)(scaleFactor.x * (float)background.h);
    }

	return maxSize;
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
	this->background.w = ((this->imageSize.width  > 0 ? this->imageSize.width  : textureSize.width)  + padding2x + border2x);
	this->background.h = ((this->imageSize.height > 0 ? this->imageSize.height : textureSize.height) + padding2x + border2x);

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

	if (this->fill) {
		SDL_RenderCopy(renderer, this->texture, nullptr, &fillArea);
		return;
	}

	auto destination = LSG_Graphics::GetDestinationAligned(fillArea, this->getMaxSize(fillArea), this->getAlignment());

	auto sizeFull = this->getTextureSize();
	auto size     = (this->downscaledTexture ? LSG_Graphics::GetTextureSize(this->downscaledTexture) : sizeFull);

	SDL_Size maxSize         = { destination.w, destination.h };
	auto     downscaleFactor = LSG_Graphics::GetDownscaleFactor(size, maxSize);

	// Downscale (step-wise) to fit background if target size is less than 50% (to avoid pixelated quality)
	if ((downscaleFactor.x > 1) || (downscaleFactor.y > 1))
	{
		if (this->downscaledTexture)
			SDL_DestroyTexture(this->downscaledTexture);

		this->downscaledTexture = LSG_Graphics::GetDownScaledTexture(this->file, LSG_Graphics::GetDownscaleFactor(sizeFull, maxSize));
	}
	// Resize (to a larger downscaled size) to fit background after a window resize (if full texture is large enough)
	else if (((size.width < maxSize.width) || (size.height < maxSize.height)) && ((sizeFull.width > size.width) || (sizeFull.height > size.height)))
	{
		if (this->downscaledTexture)
			SDL_DestroyTexture(this->downscaledTexture);

		this->downscaledTexture = LSG_Graphics::GetDownScaledTexture(this->file, LSG_Graphics::GetDownscaleFactor(sizeFull, maxSize));
	}

	auto texture = (this->downscaledTexture ? this->downscaledTexture : this->texture);

	SDL_RenderCopyEx(renderer, texture, nullptr, &destination, this->orientation.rotation, nullptr, this->orientation.flip);
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
