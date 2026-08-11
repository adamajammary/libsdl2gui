#include "LSG_Menu.h"

LSG_Menu::LSG_Menu(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->borderRadius         = 0;
	this->borderWidth          = 0;
	this->highlightedIconClose = false;
	this->highlightedNavBack   = false;
	this->isOpen               = false;
	this->renderTarget         = nullptr;
	this->subMenu              = this;

	this->textures.resize(NR_OF_MENU_TEXTURES);

	auto padding = LSG_XML::GetAttribute(this->xmlNode, "padding");

	if (padding.empty())
		this->padding = LSG_Window::GetDPIScaled(LSG_Menu::DefaultPadding);
}

LSG_Menu::~LSG_Menu()
{
	if (this->renderTarget)
		SDL_DestroyTexture(this->renderTarget);
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

	this->scrollVertical.offset = 0;
}

SDL_Rect LSG_Menu::getIconClose(const SDL_Rect& menu) const
{
	auto size = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_ICON_CLOSE]);

	SDL_Rect icon = {
		(menu.x + menu.w - size.width - this->padding),
		(menu.y + this->padding),
		size.width,
		size.height
	};

	return icon;
}

SDL_Rect LSG_Menu::getIconOpen() const
{
	auto size = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_ICON_OPEN]);
	auto icon = LSG_Graphics::GetDestinationAligned(this->background, size, this->getAlignment());

	return icon;
}

SDL_Rect LSG_Menu::getMenu(const SDL_Rect& window) const
{
	auto width     = LSG_XML::GetAttribute(this->xmlNode, "width");
	bool isPercent = (!width.empty() && width.ends_with('%'));

	SDL_Rect menu = window;

	if (isPercent)
		menu.w = (int)((double)window.w * std::atof(width.c_str()) * 0.01);
	else if (!width.empty())
		menu.w = LSG_Window::GetDPIScaled(std::atoi(width.c_str()));
	else
		menu.w = LSG_Window::GetDPIScaled(LSG_Menu::DefaultWidth);

	return menu;
}

std::vector<SDL_Rect> LSG_Menu::getMenuItems() const
{
	auto background = LSG_UI::GetBackgroundArea();
	auto menu       = this->getMenu(background);
	auto maxWidth   = (menu.w - this->padding - this->padding);

	auto padding    = LSG_Window::GetDPIScaled(LSG_Menu::DefaultPadding);
	auto itemHeight = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);
	auto lineHeight = (padding + padding + 1);

	auto offsetY = (menu.y + this->padding + itemHeight + lineHeight);

	std::vector<SDL_Rect> items;

	for (auto& child : this->subMenu->GetChildren())
	{
		SDL_Rect background = {
			(menu.x + this->padding),
			offsetY,
			maxWidth,
			(child->IsLine() ? lineHeight : itemHeight)
		};

		items.push_back(background);

		if (child->visible)
			offsetY += background.h;
	}

	return items;
}

SDL_Rect LSG_Menu::getNavBackHighlight(const SDL_Rect& menu) const
{
	auto maxHeight = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);

	SDL_Rect highlight = {
		(menu.x + this->padding),
		(menu.y + this->padding),
		maxHeight,
		maxHeight
	};

	return highlight;
}

int LSG_Menu::getTextureHeight(const SDL_Rect& background) const
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

bool LSG_Menu::IsHighlightedIconClose() const
{
	return this->highlightedIconClose;
}

bool LSG_Menu::IsHighlightedNavBack() const
{
	return this->highlightedNavBack;
}

bool LSG_Menu::isMouseOverIconClose(const SDL_Point& mousePosition) const
{
	if (!this->IsVisible(true) || !this->isOpen)
		return false;

	auto window = LSG_UI::GetBackgroundArea();
	auto menu   = this->getMenu(window);
	auto icon   = this->getIconClose(menu);

	return SDL_PointInRect(&mousePosition, &icon);
}

bool LSG_Menu::IsMouseOverIconOpen(const SDL_Point& mousePosition) const
{
	if (!this->IsVisible(true) || this->isOpen)
		return false;

	auto icon = this->getIconOpen();

	return SDL_PointInRect(&mousePosition, &icon);
}

bool LSG_Menu::isMouseOverMenu(const SDL_Point& mousePosition) const
{
	if (!this->IsVisible(true) || !this->isOpen)
		return false;

	auto window = LSG_UI::GetBackgroundArea();
	auto menu   = this->getMenu(window);

	return SDL_PointInRect(&mousePosition, &menu);
}

bool LSG_Menu::isMouseOverNavBack(const SDL_Point& mousePosition) const
{
	if (!this->IsVisible(true) || !this->subMenu->IsSubMenu())
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

void LSG_Menu::Navigate(LSG_Component* component)
{
	if (!component || (!component->IsSubMenu() && !component->IsMenu()))
		return;

	this->Close();

	this->subMenu = component;

	this->Open();
}

void LSG_Menu::OnMouseClick(const SDL_Point& mousePosition)
{
	if (!this->enabled || !this->IsVisible(true))
		return;

	if (!this->isOpen && this->IsMouseOverIconOpen(mousePosition)) {
		this->Open();
		return;
	}

	if (!this->isOpen)
		return;

	if (this->isMouseOverNavBack(mousePosition)) {
		this->Navigate(this->subMenu->GetParent());
		return;
	}

	if (this->isMouseOverIconClose(mousePosition) || !this->isMouseOverMenu(mousePosition)) {
		this->Close();
		return;
	}

	for (const auto& child : this->subMenu->GetChildren())
	{
		auto background = SDL_Rect(child->background);
		background.y   -= this->scrollVertical.offset;

		if (!child->enabled || !child->visible || !SDL_PointInRect(&mousePosition, &background))
			continue;

		if (child->IsSubMenu()) {
			this->Navigate(child);
		} else if (child->IsMenuItem()) {
			if (static_cast<LSG_MenuItem*>(child)->OnMouseDown(mousePosition))
				this->Close();
		}

		break;
	}
}

void LSG_Menu::Open()
{
	if (!this->enabled || !this->IsVisible(true))
		return;

	this->scrollVertical.offset = 0;

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
	this->scrollHorizontal.show = false;
	this->scrollVertical.show   = false;

	if (!this->IsVisible(true))
		return;

	if (!this->isOpen)
		this->renderIconOpen(renderer);
	else
		this->renderMenu(renderer);
}

void LSG_Menu::renderHeaderLine(SDL_Renderer* renderer, const SDL_Rect& menu) const
{
	auto maxHeight = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);
	auto color     = LSG_Graphics::GetThumbColor(this->backgroundColor);
	auto padding   = LSG_Window::GetDPIScaled(LSG_Menu::DefaultPadding);
	auto positionY = (menu.y + this->padding + maxHeight + padding);

	SDL_Rect border = {
		(menu.x + this->padding),
		positionY,
		(menu.x + menu.w - this->padding - 1),
		positionY
	};

	LSG_Graphics::RenderLine(renderer, color, border.x, border.y, border.w, border.h);
}

void LSG_Menu::renderHighlightIconOpen(SDL_Renderer* renderer, const SDL_Rect& background) const
{
	auto highlightSize    = LSG_Window::GetDPIScaled(LSG_MenuItem::Height + 8);
	auto maxHighlightSize = std::min(this->background.h, highlightSize);
	auto borderRadius     = LSG_Window::GetDPIScaled(maxHighlightSize / 2);
	auto diffSize         = (borderRadius - (background.h / 2));

	if (diffSize <= 0) {
		this->renderHighlight(renderer, background, borderRadius);
		return;
	}

	auto offset   = LSG_Window::GetDPIScaled(diffSize);
	auto offset2x = (offset + offset);

	SDL_Rect icon = {
		(background.x - offset),
		(background.y - offset),
		(background.w + offset2x),
		(background.h + offset2x)
	};

	this->renderHighlight(renderer, icon, borderRadius);
}

void LSG_Menu::renderIconClose(SDL_Renderer* renderer, const SDL_Rect& menu) const
{
	if (!this->textures[LSG_MENU_TEXTURE_ICON_CLOSE])
		return;

	auto icon = this->getIconClose(menu);

	LSG_Graphics::RenderTexture(renderer, this->textures[LSG_MENU_TEXTURE_ICON_CLOSE], nullptr, &icon);

	if (this->enabled && this->highlightedIconClose)
		this->renderHighlight(renderer, icon, (icon.h / 2));
}

void LSG_Menu::renderIconOpen(SDL_Renderer* renderer) const
{
	if (!this->textures[LSG_MENU_TEXTURE_ICON_OPEN])
		return;

	auto icon = this->getIconOpen();

	LSG_Graphics::RenderTexture(renderer, this->textures[LSG_MENU_TEXTURE_ICON_OPEN], nullptr, &icon);

	if (this->enabled && this->highlighted)
		this->renderHighlightIconOpen(renderer, icon);
}

void LSG_Menu::renderMenu(SDL_Renderer* renderer)
{
	auto background = LSG_UI::GetBackgroundArea();
	auto menu       = this->getMenu(background);

	LSG_Graphics::RenderFill(renderer, 0, { 0, 0, 0, 128 },      background);
	LSG_Graphics::RenderFill(renderer, 0, this->backgroundColor, menu);

	this->renderNavBack(renderer,    menu);
	this->renderTitle(renderer,      menu);
	this->renderIconClose(renderer,  menu);
	this->renderHeaderLine(renderer, menu);

	bool hasChildren = (this->subMenu->GetChildCount() > 0);
	auto offsetY     = (hasChildren ? this->subMenu->GetChild(0)->background.y : 0);

	menu.y += offsetY;
	menu.h -= offsetY;
	menu.w += LSG_ScrollBar::GetSize();

	auto textureHeight = this->getTextureHeight(menu);

	if ((textureHeight <= menu.h) || !hasChildren) {
		this->renderMenuItems(renderer);
		return;
	}

	this->scrollVertical.show = true;

	this->renderMenuContentToTexture(renderer, offsetY, { menu.w, (textureHeight + offsetY) });

	auto clip = this->getClipWithOffset({ 0, offsetY, menu.w, menu.h }, { menu.w, textureHeight });

	LSG_Graphics::RenderTexture(renderer, this->renderTarget, &clip, &menu);

	this->renderScrollBarVertical(renderer, menu, textureHeight, this->backgroundColor, true, this);
}

/**
 * @throws runtime_error
 */
void LSG_Menu::renderMenuContentToTexture(SDL_Renderer* renderer, int offsetY, const SDL_Size& textureSize)
{
	LSG_Window::InitRenderTarget(this->renderTarget, textureSize);

	if (!SDL_SetRenderTarget(renderer, this->renderTarget))
		throw std::runtime_error(std::format("Failed to set render target: {}", SDL_GetError()));

	SDL_Rect background = {
		0,
		offsetY,
		textureSize.width,
		textureSize.height
	};

	LSG_Graphics::RenderFill(renderer, 0, this->backgroundColor, background);

	this->renderMenuItems(renderer);

	SDL_SetRenderTarget(renderer, nullptr);
}

void LSG_Menu::renderMenuItems(SDL_Renderer* renderer) const
{
	auto children = this->subMenu->GetChildren();
	auto items    = this->getMenuItems();

	for (size_t i = 0; i < items.size(); i++)
	{
		children[i]->background = items[i];

		if (children[i]->IsMenuItem())
			static_cast<LSG_MenuItem*>(children[i])->Render(renderer);
		else if (children[i]->IsSubMenu())
			static_cast<LSG_MenuSub*>(children[i])->Render(renderer);
		else if (children[i]->IsLine())
			static_cast<LSG_Line*>(children[i])->Render(renderer);
	}
}

void LSG_Menu::renderNavBack(SDL_Renderer* renderer, const SDL_Rect& menu) const
{
	if (!this->textures[LSG_MENU_TEXTURE_NAV_BACK])
		return;

	auto maxHeight = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);
	auto size      = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_NAV_BACK]);

	SDL_Rect destination = {
		(menu.x + this->padding + ((maxHeight - size.width)  / 2)),
		(menu.y + this->padding + ((maxHeight - size.height) / 2)),
		size.width,
		size.height
	};

	LSG_Graphics::RenderTexture(renderer, this->textures[LSG_MENU_TEXTURE_NAV_BACK], nullptr, &destination);

	if (this->enabled && this->highlightedNavBack)
		this->renderHighlight(renderer, this->getNavBackHighlight(menu), (maxHeight / 2));
}

void LSG_Menu::renderTitle(SDL_Renderer* renderer, const SDL_Rect& menu) const
{
	if (!this->textures[LSG_MENU_TEXTURE_TITLE])
		return;

	auto maxHeight = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);
	auto closeIcon = maxHeight;
	auto navBack   = (this->textures[LSG_MENU_TEXTURE_NAV_BACK] ? maxHeight : 0);
	auto title     = LSG_Graphics::GetTextureSize(this->textures[LSG_MENU_TEXTURE_TITLE]);
	auto padding2x = (this->padding + this->padding);
	auto maxWidth  = (menu.w - padding2x - closeIcon - navBack);

	SDL_Rect clip = {
		0,
		0,
		std::min(title.width,  maxWidth),
		std::min(title.height, maxHeight)
	};

	SDL_Rect destination = {
		(menu.x + this->padding + ((maxWidth  - clip.w) / 2) + navBack),
		(menu.y + this->padding + ((maxHeight - clip.h) / 2)),
		clip.w,
		clip.h
	};

	LSG_Graphics::RenderTexture(renderer, this->textures[LSG_MENU_TEXTURE_TITLE], &clip, &destination);
}

void LSG_Menu::setMenuClosed()
{
	if (this->background.h == 0)
		return;

	this->destroyTextures();

	this->textures.resize(NR_OF_MENU_TEXTURES);

	auto iconSize = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);

	SDL_Size maxIconSize = {
		std::min(this->background.h, iconSize),
		std::min(this->background.h, iconSize)
	};

	this->textures[LSG_MENU_TEXTURE_ICON_OPEN] = LSG_Graphics::GetVector(LSG_VECTOR_MENU, this->textColor, maxIconSize);
}

void LSG_Menu::setMenuOpened()
{
	if (this->background.h == 0)
		return;

	this->destroyTextures();

	this->textures.resize(NR_OF_MENU_TEXTURES);

	auto iconSize = LSG_Window::GetDPIScaled(LSG_MenuItem::Height);

	std::string navTitle = "";

	if (this->subMenu->IsSubMenu())
	{
		auto     padding2x = LSG_Window::GetDPIScaled(LSG_MenuSub::PaddingArrow2x);
		auto     size      = (iconSize - padding2x);
		SDL_Size maxSize   = { size, size };

		this->textures[LSG_MENU_TEXTURE_NAV_BACK] = LSG_Graphics::GetVector(LSG_VECTOR_BACK, this->textColor, maxSize);

		navTitle = LSG_XML::GetAttribute(this->subMenu->GetXmlNode(), "title");
	} else if (this->IsMenu()) {
		navTitle = LSG_XML::GetAttribute(this->xmlNode, "title");
	}

	if (!navTitle.empty())
		this->textures[LSG_MENU_TEXTURE_TITLE] = LSG_Text::GetTexture(navTitle, this->getFontSize(), TTF_STYLE_BOLD, this->textColor, this->wrap);

	SDL_Size maxIconSize = {
		std::min(this->background.h, iconSize),
		std::min(this->background.h, iconSize)
	};

	this->textures[LSG_MENU_TEXTURE_ICON_CLOSE] = LSG_Graphics::GetVector(LSG_VECTOR_CLOSE, this->textColor, maxIconSize);

	for (auto child : this->subMenu->GetChildren())
	{
		if (child->IsMenuItem())
			static_cast<LSG_MenuItem*>(child)->Set();
		else if (child->IsSubMenu())
			static_cast<LSG_MenuSub*>(child)->Set();
	}
}

void LSG_Menu::SetMenu()
{
	if (this->IsOpen())
		this->setMenuOpened();
	else
		this->setMenuClosed();
}
