#include "LSG_Button.h"

LSG_Button::LSG_Button(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	auto attributes = LSG_XML::GetAttributes(this->xmlNode);

	this->iconPath = (attributes.contains("icon") ? attributes["icon"] : "");
	this->text     = (attributes.contains("text") ? attributes["text"] : "");

	this->textures.resize(NR_OF_BUTTON_TEXTURES);
}

void LSG_Button::downscaleTextureIcon(int maxSize)
{
	auto textureSize     = LSG_Graphics::GetTextureSize(this->textures[LSG_BUTTON_TEXTURE_ICON]);
	auto downscaleFactor = LSG_Graphics::GetDownscaleFactor(textureSize, { maxSize, maxSize });

	if (!this->scaleDown(downscaleFactor) && !this->scaleUp(textureSize, maxSize))
		return;

	if (this->textures[LSG_BUTTON_TEXTURE_ICON])
		SDL_DestroyTexture(this->textures[LSG_BUTTON_TEXTURE_ICON]);

	this->textures[LSG_BUTTON_TEXTURE_ICON] = LSG_Graphics::GetDownScaledTexture(this->iconPath, downscaleFactor);
}

SDL_Rect LSG_Button::getIconDestination(int iconSize)
{
	SDL_Rect destination = this->background;

	destination.w = iconSize;
	destination.h = iconSize;

	destination.x += ((this->background.w - iconSize) / 2);
	destination.y += ((this->background.h - iconSize) / 2);

	return destination;
}

SDL_Rect LSG_Button::getTextClip()
{
	SDL_Rect clip = {};

	auto textureSize = LSG_Graphics::GetTextureSize(this->textures[LSG_BUTTON_TEXTURE_TEXT]);

	clip.w = std::min(textureSize.width,  this->background.w);
	clip.h = std::min(textureSize.height, this->background.h);

	return clip;
}

SDL_Rect LSG_Button::getTextDestination(const SDL_Rect& clip)
{
	SDL_Rect destination = this->background;

	destination.w = clip.w;
	destination.h = clip.h;

	destination.x += ((this->background.w - clip.w) / 2);
	destination.y += ((this->background.h - clip.h) / 2);

	return destination;
}

bool LSG_Button::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled)
		return false;

	this->sendEvent(LSG_EVENT_BUTTON_CLICKED);

	return true;
}

void LSG_Button::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_Button::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_Button::render(SDL_Renderer* renderer)
{
	LSG_Graphics::RenderFill(renderer,   this->border, this->backgroundColor, this->background);
	LSG_Graphics::RenderBorder(renderer, this->border, this->borderColor,     this->background);

	auto iconTexture = this->textures[LSG_BUTTON_TEXTURE_ICON];
	auto textTexture = this->textures[LSG_BUTTON_TEXTURE_TEXT];

	if (!textTexture && !iconTexture)
		return;

	SDL_Rect iconDestination, textClip, textDestination;

	if (iconTexture)
	{
		int iconSize;
		
		if (this->IsVertical() && textTexture)
			iconSize = (this->background.h / 4);
		else
			iconSize = (std::min(this->background.w, this->background.h) / 2);

		this->downscaleTextureIcon(iconSize);

		iconDestination = this->getIconDestination(iconSize);
	}

	if (textTexture)
	{
		textClip        = this->getTextClip();
		textDestination = this->getTextDestination(textClip);
	}

	if (textTexture && iconTexture)
	{
		if (this->IsVertical())
			this->setLayoutVertical(iconDestination, textClip, textDestination);
		else
			this->setLayoutHorizontal(iconDestination, textClip, textDestination);
	}

	if (iconTexture)
		SDL_RenderCopy(renderer, iconTexture, nullptr, &iconDestination);

	if (textTexture)
		SDL_RenderCopy(renderer, textTexture, &textClip, &textDestination);

	if (!this->enabled)
		this->renderDisabled(renderer);

	if (this->enabled && this->highlighted)
		this->renderHighlight(renderer);
}

bool LSG_Button::scaleDown(const SDL_Point& downscaleFactor) const
{
	return ((downscaleFactor.x > 1) || (downscaleFactor.y > 1));
}

bool LSG_Button::scaleUp(const SDL_Size& textureSize, int maxSize) const
{
	return (
		((textureSize.width < maxSize) || (textureSize.height < maxSize)) &&
		((this->iconSize.width > textureSize.width) || (this->iconSize.height > textureSize.height))
	);
}

void LSG_Button::sendEvent(LSG_EventType type) const
{
	if (!this->enabled)
		return;

	SDL_Event clickEvent = {};

	clickEvent.type       = SDL_RegisterEvents(1);
	clickEvent.user.code  = (int)type;
	clickEvent.user.data1 = (void*)strdup(this->id.c_str());

	SDL_PushEvent(&clickEvent);
}

void LSG_Button::Set(const std::string& text, const std::string& iconPath)
{
	if ((text == this->text) && (iconPath == this->iconPath))
		return;

	this->iconPath = iconPath;
	this->text     = text;

	this->Set();
}

void LSG_Button::Set()
{
	if (SDL_RectEmpty(&this->background))
		return;

	this->destroyTextures();

	this->textures.resize(NR_OF_BUTTON_TEXTURES);

	if (!this->iconPath.empty())
	{
		this->textures[LSG_BUTTON_TEXTURE_ICON] = LSG_Window::ToTexture(this->iconPath);

		if (this->textures[LSG_BUTTON_TEXTURE_ICON])
			this->iconSize = LSG_Graphics::GetTextureSize(this->textures[LSG_BUTTON_TEXTURE_ICON]);
	}

	if (!this->text.empty())
		this->textures[LSG_BUTTON_TEXTURE_TEXT] = this->getTexture(this->text);
}

void LSG_Button::setLayoutHorizontal(SDL_Rect& iconDestination, SDL_Rect& textClip, SDL_Rect& textDestination)
{
	auto padding = LSG_Window::GetDPIScaled(LSG_Button::DefaultPadding);
	auto spacing = LSG_Window::GetDPIScaled(LSG_Button::DefaultSpacingX);

	auto padding2x  = (padding + padding);
	auto totalWidth = (iconDestination.w + spacing + textDestination.w);
	auto diffWidth  = (this->background.w - totalWidth - padding2x);

	if (diffWidth >= 0)
	{
		iconDestination.x = (this->background.x + ((this->background.w - totalWidth) / 2));
		textDestination.x = (iconDestination.x + iconDestination.w + spacing);
	}
	else
	{
		iconDestination.x = (this->background.x + padding);
		textDestination.x = (iconDestination.x + iconDestination.w + spacing);

		textClip.w       -= std::abs(diffWidth);
		textDestination.w = textClip.w;
	}
}

void LSG_Button::setLayoutVertical(SDL_Rect& iconDestination, SDL_Rect& textClip, SDL_Rect& textDestination)
{
	auto padding = LSG_Window::GetDPIScaled(LSG_Button::DefaultPadding);
	auto spacing = LSG_Window::GetDPIScaled(LSG_Button::DefaultSpacingY);

	iconDestination.y -= ((iconDestination.h + spacing) / 2);
	textDestination.y += ((textDestination.h + spacing) / 2);

	auto minPositionX = (this->background.x + padding);

	if (textDestination.x < minPositionX)
	{
		textDestination.x = minPositionX;

		textClip.w        -= (padding + padding);
		textDestination.w = textClip.w;
	}

	auto padding2x   = (padding + padding);
	auto totalHeight = (iconDestination.h + spacing + textDestination.h);
	auto diffHeight  = (this->background.h - totalHeight - padding2x);

	if (diffHeight < 0)
	{
		iconDestination.y = (this->background.y + padding);
		textDestination.y = (iconDestination.y + iconDestination.h + spacing);

		textClip.h       -= std::abs(diffHeight);
		textDestination.h = textClip.h;
	}
}
