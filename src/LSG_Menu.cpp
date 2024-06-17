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

void LSG_Menu::Close()
{
	this->isOpen = false;

	this->destroyTextures();

	for (auto& child : this->subMenu->GetChildren()) {
		if (child->IsMenuItem() || child->IsSubMenu())
			static_cast<LSG_MenuItem*>(child)->Close();
	}

	this->subMenu = this;

	this->setMenuClosed();

	this->scrollOffsetY = 0;
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
	auto size     = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_TITLE]);
	auto maxWidth = (menu.w - (this->padding + this->background.h + this->background.h + this->padding));

	SDL_Rect clip = {
		0,
		0,
		std::min(size.width,  maxWidth),
		std::min(size.height, this->background.h)
	};

	return clip;
}

int LSG_Menu::getTextureHeight(const SDL_Rect& background)
{
	int height = 0;

	for (auto child : this->subMenu->GetChildren())
		height += child->background.h;

	return std::max(background.h, (height + this->padding));
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
	this->Close();
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
			this->Close();
			return true;
		}

		auto scrollY = this->GetScrollY();

		for (const auto& child : this->subMenu->GetChildren())
		{
			auto background = SDL_Rect(child->background);
			background.y   -= scrollY;

			if (!child->enabled || !child->visible || !SDL_PointInRect(&mousePosition, &background))
				continue;

			if (child->IsSubMenu()) {
				this->navigate(child);
			} else if (child->IsMenuItem()) {
				if (static_cast<LSG_MenuItem*>(child)->OnMouseClick(mousePosition))
					this->Close();
			}

			break;
		}

		return true;
	}

	return false;
}

void LSG_Menu::open()
{
	this->scrollOffsetY = 0;

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
	this->showScrollX = false;
	this->showScrollY = false;

	if (!this->visible)
		return;

	if (!this->isOpen)
		this->renderIconOpen(renderer);
	else
		this->renderMenu(renderer);
}

void LSG_Menu::renderBorderLine(SDL_Renderer* renderer, const SDL_Rect& menu)
{
	auto borderColor = LSG_Graphics::GetThumbColor(this->backgroundColor);
	auto border      = this->getMenuBorder(menu);

	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
	SDL_RenderDrawLine(renderer, border.x, border.y, border.w, border.h);
}

void LSG_Menu::renderIconClose(SDL_Renderer* renderer, const SDL_Rect& menu)
{
	if (!this->textures[LSG_MENU_TEXTURE_ICON_CLOSE])
		return;

	auto destination = this->getIconClose(menu);

	SDL_RenderCopy(renderer, this->textures[LSG_MENU_TEXTURE_ICON_CLOSE], nullptr, &destination);

	if (this->enabled && this->highlightedIconClose)
		this->renderHighlight(renderer, destination);
}

void LSG_Menu::renderIconOpen(SDL_Renderer* renderer)
{
	if (!this->textures[LSG_MENU_TEXTURE_ICON_OPEN])
		return;

	auto destination = this->getIconOpen();

	SDL_RenderCopy(renderer, this->textures[LSG_MENU_TEXTURE_ICON_OPEN], nullptr, &destination);

	if (this->enabled && this->highlighted)
		this->renderHighlight(renderer, destination);
}

void LSG_Menu::renderMenu(SDL_Renderer* renderer)
{
	auto background = LSG_UI::GetBackgroundArea();
	auto menu       = this->getMenu(background);

	this->renderFill(renderer, 0, { 0, 0, 0, 128 },      background);
	this->renderFill(renderer, 0, this->backgroundColor, menu);

	this->renderNavBack(renderer,    menu);
	this->renderTitle(renderer,      menu);
	this->renderIconClose(renderer,  menu);
	this->renderBorderLine(renderer, menu);

	bool hasChildren = (this->subMenu->GetChildCount() > 0);
	auto offsetY     = (hasChildren ? this->subMenu->GetChild(0)->background.y : 0);

	menu.y += offsetY;
	menu.h -= offsetY;
	menu.w += LSG_ScrollBar::GetSize();

	auto textureHeight = this->getTextureHeight(menu);

	if ((textureHeight <= menu.h) || !hasChildren)
	{
		for (auto child : this->subMenu->GetChildren())
			child->Render(renderer);

		return;
	}

	this->showScrollY = true;

	SDL_Size maxSize     = { menu.w, textureHeight };
	SDL_Size textureSize = { menu.w, (textureHeight + offsetY) };

	auto clip    = this->getClipWithOffset({ 0, offsetY, menu.w, menu.h }, maxSize);
	auto texture = this->renderMenuContentToTexture(renderer, offsetY, textureSize);

	SDL_RenderCopy(renderer, texture, &clip, &menu);

	this->renderScrollBarVertical(renderer, menu, maxSize.height, this->backgroundColor, true);

	SDL_DestroyTexture(texture);
}

SDL_Texture* LSG_Menu::renderMenuContentToTexture(SDL_Renderer* renderer, int offsetY, const SDL_Size& textureSize)
{
	auto format  = SDL_GetWindowPixelFormat(SDL_RenderGetWindow(renderer));
	auto texture = SDL_CreateTexture(renderer, format, SDL_TEXTUREACCESS_TARGET, textureSize.width, textureSize.height);

	SDL_SetRenderTarget(renderer, texture);

	SDL_Rect background = {
		0,
		offsetY,
		textureSize.width,
		textureSize.height
	};

	this->renderFill(renderer, 0, this->backgroundColor, background);

	for (auto child : this->subMenu->GetChildren())
		child->Render(renderer);

	SDL_SetRenderTarget(renderer, NULL);

	return texture;
}

void LSG_Menu::renderNavBack(SDL_Renderer* renderer, const SDL_Rect& menu)
{
	if (!this->textures[LSG_MENU_TEXTURE_NAV_BACK])
		return;

	auto destination = this->getNavBack(menu);

	SDL_RenderCopy(renderer, this->textures[LSG_MENU_TEXTURE_NAV_BACK], nullptr, &destination);

	if (this->enabled && this->highlightedNavBack)
		this->renderHighlight(renderer, this->getNavBackHighlight(menu));
}

void LSG_Menu::renderTitle(SDL_Renderer* renderer, const SDL_Rect& menu)
{
	if (!this->textures[LSG_MENU_TEXTURE_TITLE])
		return;

	auto clip        = this->getNavTitleClip(menu);
	auto destination = this->getNavTitle(menu, clip);

	SDL_RenderCopy(renderer, this->textures[LSG_MENU_TEXTURE_TITLE], &clip, &destination);
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

	std::string navTitle = "";

	if (this->subMenu->IsSubMenu())
	{
		auto     padding2x   = LSG_Graphics::GetDPIScaled(LSG_MenuSub::PaddingArrow2x);
		auto     itemHeight  = LSG_Graphics::GetDPIScaled(LSG_MenuItem::Height);
		auto     maxHeight   = (itemHeight - padding2x);
		SDL_Size navBackSize = { maxHeight, maxHeight };

		this->textures[LSG_MENU_TEXTURE_NAV_BACK] = LSG_Graphics::GetVectorBack(this->textColor, navBackSize);

		navTitle = LSG_XML::GetAttribute(this->subMenu->GetXmlNode(), "title");
	}
	else if (this->IsMenu())
	{
		navTitle = LSG_XML::GetAttribute(this->xmlNode, "title");
	}

	if (!navTitle.empty())
		this->textures[LSG_MENU_TEXTURE_TITLE] = this->getTexture(navTitle);

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
