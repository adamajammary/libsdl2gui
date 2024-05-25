#include "LSG_Menu.h"

LSG_Menu::LSG_Menu(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->highlightedIconClose = false;
	this->highlightedNavBack   = false;
	this->isOpen               = false;
	this->lastTextColor        = {};
	this->subMenu              = this;

	auto padding = LSG_XML::GetAttribute(this->xmlNode, "padding");

	if (padding.empty())
		this->padding = LSG_Graphics::GetDPIScaled(LSG_Menu::Padding);
}

void LSG_Menu::close()
{
	this->isOpen = false;

	this->destroyTextures();

	for (auto& child : this->subMenu->GetChildren()) {
		if (child->IsMenuItem() || child->IsSubMenu())
			static_cast<LSG_MenuItem*>(child)->Close();
	}

	this->subMenu = this;

	this->setMenuClosed();
}

SDL_Rect LSG_Menu::getIconClose(const SDL_Rect& menu)
{
	auto size = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_ICON_CLOSE]);

	SDL_Rect icon = {
		(menu.x + menu.w - (size.width + this->padding)),
		(menu.y + this->padding),
		size.width,
		size.height
	};

	return icon;
}

SDL_Rect LSG_Menu::getIconOpen()
{
	auto size = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_ICON_OPEN]);
	auto icon = LSG_Graphics::GetDestinationAligned(this->background, size, this->getAlignment());

	return icon;
}

SDL_Rect LSG_Menu::getMenu(const SDL_Rect& window)
{
	auto     width  = LSG_Graphics::GetDPIScaled(LSG_Menu::Width);
	SDL_Rect menu   = { window.x, window.y, width, window.h };

	return menu;
}

SDL_Rect LSG_Menu::getMenuBorder(const SDL_Rect& menu) const
{
	auto borderPosY = (menu.y + this->padding + this->background.h + this->padding);

	SDL_Rect border = {
		(menu.x + this->padding),
		borderPosY,
		(menu.x + menu.w - 1 - this->padding),
		borderPosY
	};

	return border;
}

std::vector<SDL_Rect> LSG_Menu::getMenuItems()
{
	auto padding2x     = (this->padding + this->padding);
	auto itemWidth     = LSG_Graphics::GetDPIScaled(LSG_Menu::Width - padding2x);
	auto itemHeight    = LSG_Graphics::GetDPIScaled(LSG_MenuItem::Height);
	auto dividerHeight = (padding2x + 1);
	auto window        = LSG_UI::GetBackgroundArea();
	auto offsetY       = (window.y + this->background.h + dividerHeight + this->padding);

	std::vector<SDL_Rect> items;

	for (auto& child : this->subMenu->GetChildren())
	{
		SDL_Rect background = {
			(window.x + this->padding),
			offsetY,
			itemWidth,
			(child->IsLine() ? dividerHeight : itemHeight)
		};

		items.push_back(background);

		offsetY += background.h;
	}

	return items;
}

SDL_Rect LSG_Menu::getNavBack(const SDL_Rect& menu)
{
	auto size = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_NAV_BACK]);

	SDL_Rect navBack = {
		(menu.x + this->padding + ((this->background.h - size.width)  / 2)),
		(menu.y + this->padding + ((this->background.h - size.height) / 2)),
		size.width,
		size.height
	};

	return navBack;
}

SDL_Rect LSG_Menu::getNavBackHighlight(const SDL_Rect& menu)
{
	SDL_Rect highlight = {
		(menu.x + this->padding),
		(menu.y + this->padding),
		this->background.h,
		this->background.h
	};

	return highlight;
}

SDL_Rect LSG_Menu::getNavTitle(const SDL_Rect& menu, const SDL_Rect& clip)
{
	SDL_Rect navTitle = {
		(menu.x + ((menu.w - clip.w) / 2)),
		(menu.y + this->padding + ((this->background.h - clip.h) / 2)),
		clip.w,
		clip.h
	};

	return navTitle;
}

SDL_Rect LSG_Menu::getNavTitleClip(const SDL_Rect& menu)
{
	auto size     = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_NAV_TITLE]);
	auto maxWidth = (menu.w - (this->padding + this->background.h + this->background.h + this->padding));

	SDL_Rect clip = {
		0,
		0,
		std::min(size.width,  maxWidth),
		std::min(size.height, this->background.h)
	};

	return clip;
}

void LSG_Menu::Highlight(const SDL_Point& mousePosition)
{
	this->highlightedIconClose = this->isMouseOverIconClose(mousePosition);
	this->highlightedNavBack   = this->isMouseOverNavBack(mousePosition);
}

bool LSG_Menu::isMouseOverIconClose(const SDL_Point& mousePosition)
{
	if (!this->visible || !this->isOpen)
		return false;

	auto window = LSG_UI::GetBackgroundArea();
	auto menu   = this->getMenu(window);
	auto icon   = this->getIconClose(menu);

	return SDL_PointInRect(&mousePosition, &icon);
}

bool LSG_Menu::IsMouseOverIconOpen(const SDL_Point& mousePosition)
{
	if (!this->visible || this->isOpen)
		return false;

	auto icon = this->getIconOpen();

	return SDL_PointInRect(&mousePosition, &icon);
}

bool LSG_Menu::isMouseOverMenu(const SDL_Point& mousePosition)
{
	if (!this->visible || !this->isOpen)
		return false;

	auto window = LSG_UI::GetBackgroundArea();
	auto menu   = this->getMenu(window);

	return SDL_PointInRect(&mousePosition, &menu);
}

bool LSG_Menu::isMouseOverNavBack(const SDL_Point& mousePosition)
{
	if (!this->visible || !this->subMenu->IsSubMenu())
		return false;

	auto window  = LSG_UI::GetBackgroundArea();
	auto menu    = this->getMenu(window);
	auto navBack = this->getNavBackHighlight(menu);

	return SDL_PointInRect(&mousePosition, &navBack);
}

bool LSG_Menu::IsOpen() const
{
	return this->isOpen;
}

void LSG_Menu::navigate(LSG_Component* component)
{
	this->close();
	this->subMenu = component;
	this->open();
}

bool LSG_Menu::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || !this->visible)
		return false;


	if (!this->isOpen && this->IsMouseOverIconOpen(mousePosition)) {
		this->open();
		return true;
	}
	else if (this->isOpen)
	{
		if (this->isMouseOverNavBack(mousePosition)) {
			this->navigate(this->subMenu->GetParent());
			return true;
		}

		if (this->isMouseOverIconClose(mousePosition) || !this->isMouseOverMenu(mousePosition)) {
			this->close();
			return true;
		}

		for (const auto& child : this->subMenu->GetChildren())
		{
			if (!child->enabled || !child->visible || !SDL_PointInRect(&mousePosition, &child->background))
				continue;

			if (child->IsSubMenu()) {
				this->navigate(child);
			} else if (child->IsMenuItem()) {
				if (static_cast<LSG_MenuItem*>(child)->OnMouseClick(mousePosition))
					this->close();
			}

			break;
		}

		return true;
	}

	return false;
}

void LSG_Menu::open()
{
	this->destroyTextures();

	this->setMenuOpened();

	for (auto& child : this->subMenu->GetChildren()) {
		if (child->IsMenuItem() || child->IsSubMenu())
			static_cast<LSG_MenuItem*>(child)->Open();
	}

	this->isOpen = true;
}

void LSG_Menu::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	if (this->isOpen)
		this->renderMenuOpened(renderer);
	else
		this->renderMenuClosed(renderer);
}

void LSG_Menu::renderMenuClosed(SDL_Renderer* renderer)
{
	auto iconOpen = this->textures[LSG_MENU_TEXTURE_ICON_OPEN];

	if (!iconOpen)
		return;

	auto destination = this->getIconOpen();

	SDL_RenderCopy(renderer, iconOpen, nullptr, &destination);

	if (this->enabled && this->highlighted)
		this->renderHighlight(renderer, destination);
}

void LSG_Menu::renderMenuOpened(SDL_Renderer* renderer)
{
	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);

	auto window = LSG_UI::GetBackgroundArea();

	SDL_RenderFillRect(renderer, &window);

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
	SDL_SetRenderDrawColor(renderer, this->backgroundColor.r, this->backgroundColor.g, this->backgroundColor.b, 255);

	auto menu = this->getMenu(window);

	SDL_RenderFillRect(renderer, &menu);

	auto navBack = this->textures[LSG_MENU_TEXTURE_NAV_BACK];

	if (navBack)
	{
		auto destination = this->getNavBack(menu);

		SDL_RenderCopy(renderer, navBack, nullptr, &destination);

		if (this->enabled && this->highlightedNavBack)
			this->renderHighlight(renderer, this->getNavBackHighlight(menu));
	}

	auto navTitle = this->textures[LSG_MENU_TEXTURE_NAV_TITLE];

	if (navTitle)
	{
		auto clip        = this->getNavTitleClip(menu);
		auto destination = this->getNavTitle(menu, clip);

		SDL_RenderCopy(renderer, navTitle, &clip, &destination);
	}

	auto iconClose = this->textures[LSG_MENU_TEXTURE_ICON_CLOSE];

	if (iconClose)
	{
		auto destination = this->getIconClose(menu);

		SDL_RenderCopy(renderer, iconClose, nullptr, &destination);

		if (this->enabled && this->highlightedIconClose)
			this->renderHighlight(renderer, destination);
	}

	auto borderColor = LSG_Graphics::GetThumbColor(this->backgroundColor);
	auto border      = this->getMenuBorder(menu);

	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
	SDL_RenderDrawLine(renderer, border.x, border.y, border.w, border.h);

	for (auto child : this->subMenu->GetChildren())
		child->Render(renderer);
}

void LSG_Menu::sendEvent(LSG_EventType type)
{
}

void LSG_Menu::setMenuClosed()
{
	if (this->background.h == 0)
		return;

	this->destroyTextures();

	this->textures.resize(NR_OF_MENU_TEXTURES);

	SDL_Size size = { this->background.h, this->background.h };

	this->textures[LSG_MENU_TEXTURE_ICON_OPEN] = LSG_Graphics::GetVectorMenu(this->textColor, size);

	if (this->textures[LSG_MENU_TEXTURE_ICON_OPEN])
		this->lastTextColor = SDL_Color(this->textColor);
}

void LSG_Menu::setMenuOpened()
{
	if (this->background.h == 0)
		return;

	this->destroyTextures();

	this->textures.resize(NR_OF_MENU_TEXTURES);

	if (this->subMenu->IsSubMenu())
	{
		auto     padding2x   = LSG_Graphics::GetDPIScaled(LSG_MenuSub::PaddingArrow2x);
		auto     itemHeight  = LSG_Graphics::GetDPIScaled(LSG_MenuItem::Height);
		auto     maxHeight   = (itemHeight - padding2x);
		SDL_Size navBackSize = { maxHeight, maxHeight };

		this->textures[LSG_MENU_TEXTURE_NAV_BACK] = LSG_Graphics::GetVectorBack(this->textColor, navBackSize);

		auto navTitle = LSG_XML::GetAttribute(this->subMenu->GetXmlNode(), "label");

		if (!navTitle.empty())
			this->textures[LSG_MENU_TEXTURE_NAV_TITLE] = this->getTexture(navTitle);
	}

	SDL_Size closeIconSize = { this->background.h, this->background.h };

	this->textures[LSG_MENU_TEXTURE_ICON_CLOSE] = LSG_Graphics::GetVectorClose(this->textColor, closeIconSize);

	if (this->textures[LSG_MENU_TEXTURE_ICON_CLOSE])
		this->lastTextColor = SDL_Color(this->textColor);

	auto children = this->subMenu->GetChildren();
	auto items    = this->getMenuItems();

	for (size_t i = 0; i < items.size(); i++)
	{
		if (children[i]->IsMenuItem())
			static_cast<LSG_MenuItem*>(children[i])->SetMenuItem(items[i]);
		else if (children[i]->IsSubMenu())
			static_cast<LSG_MenuSub*>(children[i])->SetSubMenu(items[i]);
		else
			children[i]->background = items[i];
	}
}

void LSG_Menu::SetMenu()
{
	if (this->IsOpen())
		this->setMenuOpened();
	else
		this->setMenuClosed();
}
