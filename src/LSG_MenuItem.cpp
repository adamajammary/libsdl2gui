#include "LSG_MenuItem.h"

LSG_MenuItem::LSG_MenuItem(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->closed          = true;
	this->iconOrientation = {};
	this->selected        = false;
}

void LSG_MenuItem::Close()
{
	this->closed = true;

	this->destroyTextures();
}

SDL_Texture* LSG_MenuItem::getIcon(const std::string& imageFile)
{
	auto icon    = LSG_Window::ToTexture(imageFile);
	auto size    = LSG_Graphics::GetTextureSize(icon);
	auto maxSize = this->getMaxHeightIcon();

	if ((size.width <= maxSize) && (size.height <= maxSize))
		return icon;

	SDL_DestroyTexture(icon);

	auto downscaledSize = LSG_Graphics::GetDownscaledSize(size, { maxSize, maxSize });

	return LSG_Graphics::GetTextureDownScaled(imageFile, downscaledSize);
}

int LSG_MenuItem::getMaxHeightIcon() const
{
	auto padding = LSG_Graphics::GetDPIScaled(LSG_MenuItem::PaddingIcon2x);

	return (this->background.h - padding);
}

bool LSG_MenuItem::IsClosed() const
{
	return this->closed;
}

bool LSG_MenuItem::IsSelected() const
{
	return this->selected;
}

bool LSG_MenuItem::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || !this->visible)
		return false;

	this->sendEvent(LSG_EVENT_MENU_ITEM_SELECTED);

	return true;
}

void LSG_MenuItem::Open()
{
	this->closed = false;
}

void LSG_MenuItem::Render(SDL_Renderer* renderer)
{
	if (!this->visible || (this->textures.size() < NR_OF_MENU_ITEM_TEXTURES))
		return;

	if (!this->enabled)
		this->renderDisabled(renderer);

	this->renderIcon(renderer);
	this->renderText(renderer, this->textures[LSG_MENU_ITEM_TEXTURE_TEXT]);
	this->renderKey(renderer);

	if (this->selected)
		this->renderSelected(renderer);

	if (this->enabled && this->highlighted)
		this->renderHighlight(renderer, this->background);
}

void LSG_MenuItem::renderIcon(SDL_Renderer* renderer)
{
	auto texture = this->textures[LSG_MENU_ITEM_TEXTURE_ICON];

	if (!texture)
		return;

	auto maxSize = this->getMaxHeightIcon();
	auto size    = LSG_Graphics::GetTextureSize(texture);

	SDL_Rect destination = {
		this->background.x,
		this->background.y,
		std::min(size.width,  maxSize),
		std::min(size.height, maxSize)
	};

	destination.x += ((this->background.h - destination.w) / 2);
	destination.y += ((this->background.h - destination.h) / 2);

	SDL_Point center = { (destination.w / 2), (destination.h / 2) };

	SDL_RenderCopyEx(
		renderer,
		texture,
		nullptr,
		&destination,
		this->iconOrientation.rotation,
		&center,
		this->iconOrientation.flip
	);
}

void LSG_MenuItem::renderSelected(SDL_Renderer* renderer)
{
	auto texture = this->textures[LSG_MENU_ITEM_TEXTURE_SELECTED];

	if (!texture)
		return;

	auto size    = LSG_Graphics::GetTextureSize(texture);
	auto padding = LSG_Graphics::GetDPIScaled(LSG_MenuItem::PaddingIcon);

	SDL_Rect destination = {
		(this->background.x + this->background.w - size.width - padding),
		(this->background.y + ((this->background.h - size.height) / 2)),
		size.width,
		size.height
	};

	SDL_RenderCopy(renderer, texture, nullptr, &destination);

	auto border      = LSG_Graphics::GetDPIScaled(1);
	auto borderColor = LSG_Graphics::GetThumbColor(this->backgroundColor);

	this->renderBorder(renderer, border, borderColor, this->background);

	this->renderHighlight(renderer, this->background);
}

void LSG_MenuItem::renderKey(SDL_Renderer* renderer)
{
	auto texture = this->textures[LSG_MENU_ITEM_TEXTURE_KEY];

	if (!texture)
		return;

	auto textSize       = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_ITEM_TEXTURE_TEXT]);
	auto remainingWidth = (this->background.w - this->background.h - this->background.h - textSize.width);

	if (remainingWidth < 1)
		return;

	auto size = LSG_Graphics::GetTextureSize(texture);

	SDL_Rect clip = {
		0,
		0,
		std::min(size.width,  remainingWidth),
		std::min(size.height, this->background.h)
	};

	SDL_Rect destination = {
		(this->background.x + this->background.w - this->background.h - clip.w),
		(this->background.y + ((this->background.h - size.height) / 2)),
		clip.w,
		clip.h
	};

	SDL_RenderCopy(renderer, texture, &clip, &destination);
}

void LSG_MenuItem::renderText(SDL_Renderer* renderer, SDL_Texture* texture)
{
	if (!texture)
		return;

	auto size = LSG_Graphics::GetTextureSize(texture);

	SDL_Rect clip = {
		0,
		0,
		std::min(size.width,  (this->background.w - (this->background.h + this->background.h))),
		std::min(size.height, this->background.h)
	};

	SDL_Rect destination = {
		(this->background.x + this->background.h),
		(this->background.y + ((this->background.h - size.height) / 2)),
		clip.w,
		clip.h
	};

	SDL_RenderCopy(renderer, texture, &clip, &destination);
}

void LSG_MenuItem::sendEvent(LSG_EventType type)
{
	if (!this->enabled)
		return;

	SDL_Event menuEvent = {};

	menuEvent.type       = SDL_RegisterEvents(1);
	menuEvent.user.code  = (int)type;
	menuEvent.user.data1 = (void*)strdup(this->id.c_str());

	SDL_PushEvent(&menuEvent);
}

void LSG_MenuItem::SetMenuItem(const SDL_Rect& background)
{
	this->background = background;

	this->destroyTextures();

	this->textures.resize(NR_OF_MENU_ITEM_TEXTURES);

	auto xmlIcon = LSG_XML::GetAttribute(this->xmlNode, "icon");

	if (!xmlIcon.empty())
	{
		this->iconOrientation = LSG_Graphics::GetImageOrientation(xmlIcon);

		this->textures[LSG_MENU_ITEM_TEXTURE_ICON] = this->getIcon(xmlIcon);
	}

	if (!this->text.empty())
		this->textures[LSG_MENU_ITEM_TEXTURE_TEXT] = this->getTexture(this->text);

	auto xmlKey = LSG_XML::GetAttribute(this->xmlNode, "key");

	if (!xmlKey.empty())
		this->textures[LSG_MENU_ITEM_TEXTURE_KEY] = this->getTexture(xmlKey);

	if (this->selected)
		this->textures[LSG_MENU_ITEM_TEXTURE_SELECTED] = this->getTexture(LSG_ConstUnicodeCharacter::Checkmark);
}

void LSG_MenuItem::SetSelected(bool selected)
{
	if (!this->parent || (this->selected == selected))
		return;

	auto items = this->parent->GetChildren();

	for (auto item : items) {
		if (item->IsMenuItem())
			static_cast<LSG_MenuItem*>(item)->selected = false;
	}

	this->selected = selected;

	this->SetMenuItem(this->background);
}
