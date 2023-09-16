#include "LSG_MenuSub.h"

LSG_MenuSub::LSG_MenuSub(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Menu(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
{
	this->enableScroll    = false;
	this->items           = {};
	this->orientation     = LSG_VERTICAL;
	this->selectedSubMenu = -1;
	this->visible         = false;
	this->wrap            = true;

	auto border = this->GetXmlAttribute("border");

	if (border.empty())
		this->border = 1;
}

void LSG_MenuSub::AddItem(const std::string& item, const std::string& itemId)
{
	if (item.empty())
		return;

	LSG_UI::AddSubMenuItem(this, item, itemId);

	this->SetItems();
}

void LSG_MenuSub::Close()
{
	this->selectedSubMenu = -1;
	this->visible         = false;

	for (auto child : this->children)
		child->background = {};

	this->destroyTextures();
}

LSG_Strings LSG_MenuSub::GetItems()
{
	return this->items;
}

int LSG_MenuSub::getSelectedIndex()
{
	if (!this->parent)
		return -1;

	auto children = this->parent->GetChildren();

	for (int i = 0; i < (int)children.size(); i++) {
		if (children[i]->GetID() == this->id)
			return i;
	}

	return -1;
}

bool LSG_MenuSub::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->visible || !this->parent)
		return false;

	if (!this->parent->IsSubMenu())
		return true;

	SDL_Point mousePosition = { event.x, event.y };
	auto      index         = this->getSelectedIndex();

	return this->open(mousePosition, index);
}

bool LSG_MenuSub::open(const SDL_Point& mousePosition, int index)
{
	auto currentSelectedSubMenu = this->selectedSubMenu;
	auto parentChildren         = this->parent->GetChildren();

	for (auto child : parentChildren) {
		LSG_UI::SetSubMenuVisible(child, false);
		child->visible = true;
	}

	if ((index < 0) || (index >= (int)parentChildren.size()) || (index == currentSelectedSubMenu) || this->children.empty())
		return true;

	this->selectedSubMenu = index;

	this->SetItems();

	auto textureSize = this->GetTextureSize();
	auto subMenuArea = SDL_Rect(this->background);

	subMenuArea.x += (this->background.w + 1);
	subMenuArea.w  = (textureSize.width  + LSG_MENU_SUB_PADDING_X_2X);

	auto windowSize = LSG_Window::GetSize();

	subMenuArea.x = std::min(windowSize.width  - subMenuArea.w, subMenuArea.x);
	subMenuArea.y = std::min(windowSize.height - subMenuArea.h, subMenuArea.y);

	int  offsetY   = LSG_MENU_SUB_PADDING_Y;
	auto rowHeight = (textureSize.height / (int)this->children.size());

	for (auto child : this->children)
	{
		child->background    = subMenuArea;
		child->background.y += offsetY;
		child->background.h  = rowHeight;

		offsetY += rowHeight;
	}

	LSG_UI::SetSubMenuVisible(this);

	return true;
}

void LSG_MenuSub::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	if (this->selectedSubMenu >= 0)
		this->renderHighlight(renderer, this->background, this->backgroundColor);
	
	if (!this->enabled)
		this->renderDisabledOverlay(renderer, this->background);
	else if (this->highlighted)
		this->renderHighlight(renderer, this->background, this->backgroundColor);

	if (!this->texture)
		return;

	auto backgroundArea = SDL_Rect(this->background);

	if (this->parent->IsSubMenu() && !this->children.empty())
	{
		backgroundArea.x = this->children[0]->background.x;
		backgroundArea.w = this->children[0]->background.w;
		backgroundArea.h = ((this->children[0]->background.h * (int)this->children.size()) + LSG_MENU_SUB_PADDING_Y_2X);
	}

	backgroundArea.x -= LSG_MENU_SPACING_HALF;

	auto destination = SDL_Rect(backgroundArea);

	destination.x += LSG_MENU_SUB_PADDING_X;
	destination.y += LSG_MENU_SUB_PADDING_Y;

	this->renderFill(renderer, this->border, this->backgroundColor, backgroundArea);
	this->renderTexture(renderer, destination);
	this->renderBorder(renderer, this->border, this->borderColor, backgroundArea);

	for (auto child : this->children)
	{
		if (!child->visible || !child->IsMenuItem())
			continue;

		if (static_cast<LSG_MenuItem*>(child)->IsSelected())
			this->renderHighlight(renderer, child->background, child->backgroundColor);

		if (!child->enabled)
			this->renderDisabledOverlay(renderer, child->background);
		else if (child->highlighted)
			this->renderHighlight(renderer, child->background, child->backgroundColor);
	}
}

void LSG_MenuSub::renderDisabledOverlay(SDL_Renderer* renderer, const SDL_Rect& background)
{
	SDL_Rect backgroundArea = background;

	backgroundArea.x -= LSG_MENU_SPACING_HALF;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);

	SDL_RenderFillRect(renderer, &backgroundArea);
}

void LSG_MenuSub::renderHighlight(SDL_Renderer* renderer, const SDL_Rect& background, const SDL_Color& backgroundColor)
{
	SDL_Rect backgroundArea = background;

	backgroundArea.x -= LSG_MENU_SPACING_HALF;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - backgroundColor.r, 255 - backgroundColor.g, 255 - backgroundColor.b, 64);

	SDL_RenderFillRect(renderer, &backgroundArea);
}

void LSG_MenuSub::sendEvent(LSG_EventType type)
{
}

void LSG_MenuSub::SetItems()
{
	this->items.clear();
	this->text = "";

	int maxLength1 = 0;
	int maxLength2 = 0;

	for (auto child : this->children)
	{
		bool isSubMenu = child->IsSubMenu();
		auto item      = (isSubMenu ? child->GetXmlAttribute("label") : child->GetXmlValue());
		
		if (item.empty())
			continue;

		auto tabPos   = item.find("\\t");
		auto accelPos = (tabPos + 2);

		if (tabPos != std::string::npos) {
			maxLength1 = std::max(maxLength1, ((int)tabPos + LSG_MENU_LABEL_SPACING));
			maxLength2 = std::max(maxLength2, (int)(item.size() - accelPos));
		} else {
			maxLength1 = std::max(maxLength1, ((int)item.size() + LSG_MENU_LABEL_SPACING));
		}

		if (isSubMenu)
			item = item.append(">");

		this->items.push_back(item);
	}

	for (const auto& item : this->items)
	{
		auto tabPos   = item.find("\\t");
		auto accelPos = (tabPos + 2);
		auto subPos   = item.rfind(">");

		std::string itemText;

		if ((tabPos != std::string::npos) && (subPos != std::string::npos))
		{
			auto format = LSG_Text::Format("%%-%ds%%%ds%%3s\n", maxLength1, maxLength2);
			auto label  = item.substr(0, tabPos);
			auto accel  = item.substr(accelPos, (item.size() - accelPos - 1));

			itemText = LSG_Text::Format(format.c_str(), label.c_str(), accel.c_str(), ">");
		}
		else if (tabPos != std::string::npos)
		{
			auto format = LSG_Text::Format("%%-%ds%%%ds\n", maxLength1, maxLength2);
			auto label  = item.substr(0, tabPos);
			auto accel  = item.substr(accelPos);

			itemText = LSG_Text::Format(format.c_str(), label.c_str(), accel.c_str());
		}
		else if (subPos != std::string::npos)
		{
			auto format = LSG_Text::Format("%%-%ds%%%ds%%3s\n", maxLength1, maxLength2);
			auto label  = item.substr(0, subPos);

			itemText = LSG_Text::Format(format.c_str(), label.c_str(), "", ">");
		} else {
			itemText = LSG_Text::Format("%s\n", item.c_str());
		}

		this->text.append(itemText);
	}

	if (!this->items.empty() && !this->text.empty())
		this->SetText();
}
