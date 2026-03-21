#include "LSG_MenuItem.h"

LSG_MenuItem::LSG_MenuItem(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->closed          = true;
	this->iconOrientation = {};
	this->selected        = false;

	this->textures.resize(NR_OF_MENU_ITEM_TEXTURES);
}

void LSG_MenuItem::Close()
{
	this->closed = true;

	this->destroyTextures();
}

SDL_Texture* LSG_MenuItem::getIcon(const std::string& imageFile) const
{
	auto icon    = LSG_Window::ToTexture(imageFile);
	auto size    = LSG_Graphics::GetTextureSize(icon);
	auto maxSize = this->getMaxHeightIcon();

	auto downscaleFactor = LSG_Graphics::GetDownscaleFactor(size, { maxSize, maxSize });

	if ((downscaleFactor.x > 1) || (downscaleFactor.y > 1))
	{
		SDL_DestroyTexture(icon);

		icon = LSG_Graphics::GetDownScaledTexture(imageFile, downscaleFactor);
	}

	return icon;
}

int LSG_MenuItem::getMaxHeightIcon() const
{
	auto maxHeight = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);
	auto padding   = LSG_Window::GetDPIScaled(LSG_MenuItem::PaddingIcon);

	return (maxHeight - padding);
}

bool LSG_MenuItem::IsClosed() const
{
	return this->closed;
}

bool LSG_MenuItem::IsSelected() const
{
	return this->selected;
}

bool LSG_MenuItem::OnMouseDown(const SDL_Point& mousePosition)
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

void LSG_MenuItem::Render(SDL_Renderer* renderer) const
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
		this->renderHighlight(renderer);
}

void LSG_MenuItem::renderIcon(SDL_Renderer* renderer) const
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

void LSG_MenuItem::renderSelected(SDL_Renderer* renderer) const
{
	auto texture = this->textures[LSG_MENU_ITEM_TEXTURE_SELECTED];

	if (!texture)
		return;

	auto size    = LSG_Graphics::GetTextureSize(texture);
	auto padding = LSG_Window::GetDPIScaled(LSG_MenuItem::PaddingIconSelected);

	SDL_Rect destination = {
		(this->background.x + this->background.w - size.width - padding),
		(this->background.y + ((this->background.h - size.height) / 2)),
		size.width,
		size.height
	};

	SDL_RenderCopy(renderer, texture, nullptr, &destination);
}

void LSG_MenuItem::renderKey(SDL_Renderer* renderer) const
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

void LSG_MenuItem::renderText(SDL_Renderer* renderer, SDL_Texture* texture) const
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

void LSG_MenuItem::sendEvent(LSG_EventType type) const
{
	if (!this->enabled)
		return;

	SDL_Event menuEvent = {};

	menuEvent.type       = SDL_RegisterEvents(1);
	menuEvent.user.code  = (int)type;
	menuEvent.user.data1 = (void*)strdup(this->id.c_str());

	SDL_PushEvent(&menuEvent);
}

void LSG_MenuItem::Set()
{
	if (!this->visible)
		return;

	this->destroyTextures();

	this->textures.resize(NR_OF_MENU_ITEM_TEXTURES);

	auto xmlIcon = LSG_XML::GetAttribute(this->xmlNode, "icon");

	if (!xmlIcon.empty())
	{
		auto exif = LSG_Exif::Get(xmlIcon);

		this->iconOrientation = LSG_Exif::GetOrientation(exif.tags);

		this->textures[LSG_MENU_ITEM_TEXTURE_ICON] = this->getIcon(xmlIcon);
	}

	if (!this->text.empty())
		this->textures[LSG_MENU_ITEM_TEXTURE_TEXT] = this->getTexture(this->text);

	auto xmlKey = LSG_XML::GetAttribute(this->xmlNode, "key");

	if (!xmlKey.empty())
		this->textures[LSG_MENU_ITEM_TEXTURE_KEY] = this->getTexture(xmlKey);

	if (this->selected)
	{
		auto size = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_ITEM_TEXTURE_TEXT]).height;

		this->textures[LSG_MENU_ITEM_TEXTURE_SELECTED] = LSG_Graphics::GetVector(LSG_VECTOR_CHECK, this->textColor, { size, size });
	}
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

	this->Set();
}
