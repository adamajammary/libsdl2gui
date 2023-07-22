#include "LSG_MenuSub.h"

LSG_MenuSub::LSG_MenuSub(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlDoc, xmlNode, xmlNodeName, parent)
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

int LSG_MenuSub::getRowHeight(LSG_Component* component)
{
	if (!component)
		return 0;

	auto textureSize = component->GetTextureSize();
	auto rowCount    = (int)component->GetChildren().size();
	auto rowHeight   = (textureSize.height / rowCount);

	return rowHeight;
}

int LSG_MenuSub::getSelectedSubMenu(const SDL_Point& mousePosition, LSG_Component* component)
{
	if (!component)
		return -1;

	auto subMenuRowCount  = (int)component->GetChildren().size();
	auto subMenuRowHeight = this->getRowHeight(component);
	auto subMenuRow       = SDL_Rect(component->background);

	subMenuRow.y += LSG_MENU_SUB_PADDING_Y;
	subMenuRow.h  = subMenuRowHeight;

	for (int i = 0; i < subMenuRowCount; i++)
	{
		if (SDL_PointInRect(&mousePosition, &subMenuRow))
			return i;

		subMenuRow.y += subMenuRowHeight;
	}

	return -1;
}

bool LSG_MenuSub::MouseClick(const SDL_MouseButtonEvent& event)
{
	if (!this->visible || !this->parent)
		return false;

	if (!this->parent->IsSubMenu())
		return true;

	auto mousePosition = SDL_Point(event.x, event.y);
	auto index         = this->getSelectedSubMenu(mousePosition, this->parent);

	return this->open(mousePosition, index);
}

bool LSG_MenuSub::open(const SDL_Point& mousePosition, int index)
{
	auto currentSelectedSubMenu = this->selectedSubMenu;

	auto parentChildren = this->parent->GetChildren();

	for (auto child : parentChildren) {
		LSG_UI::SetSubMenuVisible(child, false);
		child->visible = true;
	}

	if (this->children.empty())
		return true;

	if ((index < 0) || (index >= (int)parentChildren.size()) || (index == currentSelectedSubMenu))
		return true;

	this->selectedSubMenu = index;

	this->SetItems();

	auto textureSize = this->GetTextureSize();
	auto subMenuArea = SDL_Rect(this->background);

	subMenuArea.x += (this->background.w + 1);
	subMenuArea.w  = (textureSize.width  + LSG_MENU_SUB_PADDING_X_2X);

	auto windowSize = LSG_Window::GetSize();

	subMenuArea.x = min(windowSize.width  - subMenuArea.w, subMenuArea.x);
	subMenuArea.y = min(windowSize.height - subMenuArea.h, subMenuArea.y);

	int  offsetY   = LSG_MENU_SUB_PADDING_Y;
	auto rowHeight = (textureSize.height / (int)this->children.size());

	for (auto child : this->children)
	{
		child->background    = SDL_Rect(subMenuArea);
		child->background.y += offsetY;
		child->background.h  = rowHeight;

		offsetY += rowHeight;
	}

	LSG_UI::SetSubMenuVisible(this);

	return true;
}

void LSG_MenuSub::Render(SDL_Renderer* renderer)
{
	if (!this->enabled)
	{
		this->renderDisabledOverlay(renderer);
	}
	else if (this->highlighted)
	{
		this->renderHighlight(renderer);

		SDL_Point mousePosition = {};
		SDL_GetMouseState(&mousePosition.x, &mousePosition.y);

		auto index = this->getSelectedSubMenu(mousePosition, this->parent);

		if ((index >= 0) && (index != this->selectedSubMenu))
			this->open(mousePosition, index);
	}

	if (!this->visible || !this->texture)
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

	if (this->selectedSubMenu >= 0)
		this->renderHighlightSelection(renderer, this->selectedSubMenu, this->parent);
}

void LSG_MenuSub::renderDisabledOverlay(SDL_Renderer* renderer)
{
	auto backgroundArea = SDL_Rect(this->background);

	backgroundArea.x -= LSG_MENU_SPACING_HALF;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 64);

	SDL_RenderFillRect(renderer, &backgroundArea);
}

void LSG_MenuSub::renderHighlight(SDL_Renderer* renderer)
{
	auto backgroundArea = SDL_Rect(this->background);

	backgroundArea.x -= LSG_MENU_SPACING_HALF;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 64);

	SDL_RenderFillRect(renderer, &backgroundArea);
}

void LSG_MenuSub::renderHighlightSelection(SDL_Renderer* renderer, int index, LSG_Component* component)
{
	if (!component)
		return;

	auto subMenuRowCount = (int)component->GetChildren().size();

	if ((index < 0) || (index >= subMenuRowCount))
		return;

	auto subMenuRowHeight = this->getRowHeight(component);
	auto subMenuRow       = SDL_Rect(component->background);

	subMenuRow.y += LSG_MENU_SUB_PADDING_Y;
	subMenuRow.x -= LSG_MENU_SPACING_HALF;

	subMenuRow.y += (index * subMenuRowHeight);
	subMenuRow.h  = subMenuRowHeight;

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
	SDL_SetRenderDrawColor(renderer, 255 - this->backgroundColor.r, 255 - this->backgroundColor.g, 255 - this->backgroundColor.b, 64);

	SDL_RenderFillRect(renderer, &subMenuRow);
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
			maxLength1 = max(maxLength1, ((int)tabPos + LSG_MENU_LABEL_SPACING));
			maxLength2 = max(maxLength2, (int)(item.size() - accelPos));
		} else {
			maxLength1 = max(maxLength1, ((int)item.size() + LSG_MENU_LABEL_SPACING));
		}

		if (isSubMenu)
			item = item.append(">");

		this->items.push_back(item);
	}

	char buffer[1024];

	for (const auto& item : this->items)
	{
		auto tabPos   = item.find("\\t");
		auto accelPos = (tabPos + 2);
		auto subPos   = item.rfind(">");

		if ((tabPos != std::string::npos) && (subPos != std::string::npos))
		{
			auto format = std::format("%-{}s%{}s%3s\n", maxLength1, maxLength2);
			auto label  = item.substr(0, tabPos);
			auto accel  = item.substr(accelPos, (item.size() - accelPos - 1));

			std::sprintf(buffer, format.c_str(), label.c_str(), accel.c_str(), ">");

			this->text.append(buffer);
		}
		else if (tabPos != std::string::npos)
		{
			auto format = std::format("%-{}s%{}s\n", maxLength1, maxLength2);
			auto label  = item.substr(0, tabPos);
			auto accel  = item.substr(accelPos);

			std::sprintf(buffer, format.c_str(), label.c_str(), accel.c_str());

			this->text.append(buffer);
		}
		else if (subPos != std::string::npos)
		{
			auto format = std::format("%-{}s%{}s%3s\n", maxLength1, maxLength2);
			auto label  = item.substr(0, subPos);

			std::sprintf(buffer, format.c_str(), label.c_str(), "", ">");

			this->text.append(buffer);
		} else {
			this->text.append(item + "\n");
		}
	}

	if (!this->items.empty() && !this->text.empty())
		this->SetText();
}
