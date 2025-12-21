#include "LSG_Image.h"

LSG_Image::LSG_Image(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Component(id, layer, xmlNode, xmlNodeName, parent)
{
	this->aspectRatio = {};
	this->orientation = {};
	this->scaleFactor = {};

	auto attributes = LSG_XML::GetAttributes(this->xmlNode);

	this->file = (attributes.contains("file") ? attributes["file"] : "");
	this->fill = (attributes.contains("fill") && attributes["fill"] == "true");
}

LSG_Image::~LSG_Image()
{
	this->destroyTextures();
}

void LSG_Image::downscaleTextureIcon(SDL_Size maxSize)
{
	auto textureSize     = this->getTextureSize();
	auto downscaleFactor = LSG_Graphics::GetDownscaleFactor(textureSize, maxSize);

	if (!this->scaleDown(downscaleFactor) && !this->scaleUp(textureSize, maxSize))
		return;

	if (this->texture)
		SDL_DestroyTexture(this->texture);

	this->texture = LSG_Graphics::GetDownScaledTexture(this->file, downscaleFactor);

	if (this->texture)
		this->rotate();
}

SDL_Size LSG_Image::getMaxSize(const SDL_Rect& background) const
{
	SDL_Size maxSize     = {};
	auto     textureSize = this->getTextureSize();

	if (background.w >= background.h) {
		maxSize.height = std::min(textureSize.height, background.h);
		maxSize.width  = (int)(this->scaleFactor.x * (float)maxSize.height);
	} else {
		maxSize.width  = std::min(textureSize.width, background.w);
		maxSize.height = (int)(this->scaleFactor.y * (float)maxSize.width);
	}

    if (maxSize.width > background.w) {
        maxSize.width  = background.w;
        maxSize.height = (int)(scaleFactor.y * (float)background.w);
    } else if (maxSize.height > background.h) {
        maxSize.height = background.h;
        maxSize.width  = (int)(scaleFactor.x * (float)background.h);
    }

	return maxSize;
}

SDL_Size LSG_Image::GetSize()
{
	auto attributes = this->GetXmlAttributes();

	auto width  = (attributes.contains("width")  ? attributes["width"]  : "");
	auto height = (attributes.contains("height") ? attributes["height"] : "");

	if (!width.empty() && !height.empty())
		return { this->background.w, this->background.h };

	if (!width.empty() && height.empty())
		return { this->background.w, (int)(this->scaleFactor.y * (float)this->background.w) };

	if (!height.empty() && width.empty())
		return { (int)(this->scaleFactor.x * (float)this->background.h), this->background.h };

	if (!this->texture)
		this->setTexture();

	return this->getTextureSize();
}

void LSG_Image::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

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

	auto fillArea = LSG_Graphics::GetFillArea(this->background, this->border);

	if (this->fill) {
		SDL_RenderCopy(renderer, this->texture, nullptr, &fillArea);
		return;
	}

	auto maxSize = this->getMaxSize(fillArea);

	this->downscaleTextureIcon(maxSize);

	if (this->aspectRatio.x < this->aspectRatio.y)
		maxSize.width = (int)(this->aspectRatio.x * (float)maxSize.width);
	else if (this->aspectRatio.y < this->aspectRatio.x)
		maxSize.height = (int)(this->aspectRatio.y * (float)maxSize.height);

	auto destination = LSG_Graphics::GetDestinationAligned(fillArea, maxSize, this->getAlignment());

	SDL_RenderCopy(renderer, this->texture, nullptr, &destination);
}

void LSG_Image::rotate()
{
	this->imageSize = LSG_Graphics::GetTextureSize(this->texture);

	this->aspectRatio = {
		((float)this->imageSize.width  / (float)this->imageSize.height),
		((float)this->imageSize.height / (float)this->imageSize.width)
	};

	if (this->orientation.rotation > 0.0)
	{
		uint32_t format;
		int      width, height;

		SDL_QueryTexture(this->texture, &format, nullptr, &width, &height);

		auto maxSize = std::max(width, height);

		this->texture = LSG_Window::RotateTexture(this->texture, this->orientation, { maxSize, maxSize }, format);

		this->imageSize = { maxSize, maxSize };

		if ((this->orientation.rotation == 90.0) || (this->orientation.rotation == 270.0))
		{
			auto x = this->aspectRatio.x;

			this->aspectRatio.x  = this->aspectRatio.y;
			this->aspectRatio.y = x;
		}
	}

	this->scaleFactor = {
		((float)this->imageSize.width  / (float)this->imageSize.height),
		((float)this->imageSize.height / (float)this->imageSize.width)
	};
}

bool LSG_Image::scaleDown(const SDL_Point& downscaleFactor) const
{
	return ((downscaleFactor.x > 1) || (downscaleFactor.y > 1));
}

bool LSG_Image::scaleUp(const SDL_Size& textureSize, SDL_Size maxSize) const
{
	return (
		((textureSize.width < maxSize.width) || (textureSize.height < maxSize.height)) &&
		((this->imageSize.width > textureSize.width) || (this->imageSize.height > textureSize.height))
	);
}

void LSG_Image::Set(const std::string& file, bool fill)
{
	if ((file == this->file) && (fill == this->fill))
		return;

	this->file = file;
	this->fill = fill;

	this->set();
}

void LSG_Image::Set()
{
	this->set();
}

void LSG_Image::set()
{
	if (SDL_RectEmpty(&this->background))
		return;

	this->destroyTextures();
	this->setTexture();
}

void LSG_Image::setTexture()
{
	if (this->file.empty())
		return;

	this->orientation = LSG_Exif::GetOrientation(LSG_Exif::Get(this->file).tags);
	this->texture = LSG_Window::ToTexture(this->file);

	if (this->texture)
		this->rotate();
}
