#include "LSG_Modal.h"

LSG_Modal::LSG_Modal(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->components        = {};
	this->componentsByLayer = {};
	this->visible           = false;

	this->padding = LSG_Window::GetDPIScaled(LSG_Modal::Padding);

	this->textures.resize(NR_OF_MODAL_TEXTURES);

	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	this->hideCloseIcon = (xmlAttributes.contains("hide-close-icon") && xmlAttributes["hide-close-icon"] == "true");
}

/**
 * @throws invalid_argument
 */
LSG_Component* LSG_Modal::addNode(LibXml::xmlNode* node, LSG_Component* parent)
{
	if (!node)
		throw std::invalid_argument("XML node cannot be null.");

	if (node->type != LibXml::XML_ELEMENT_NODE)
		return nullptr;

	auto name  = std::string(reinterpret_cast<const char*>(node->name));
	auto xmlID = LSG_XML::GetAttribute(node, "id");

	auto nodeId = LSG_UI::GetNextId();
	auto layer  = (LSG_Component::LayerOffset + nodeId);

	if (parent && parent->IsScrollablePanel(true))
		layer = nodeId;

	auto id = (!xmlID.empty() ? xmlID : std::format("{}_{}", name, layer));

	if (this->components.contains(id))
		throw std::invalid_argument(std::format("Duplicate XML ID '{}' already exists.", id));

	auto component = LSG_UI::GetComponent(id, layer, node, name, parent);

	if (!component)
		return nullptr;

	this->components[id]           = component;
	this->componentsByLayer[layer] = component;

	return component;
}

/**
 * @throws invalid_argument
 */
void LSG_Modal::addNodes(LibXml::xmlNode* parentNode, LSG_Component* parent)
{
	if (!parentNode)
		throw std::invalid_argument("Parent node cannot be null.");

	auto childNodes = LSG_XML::GetChildNodes(parentNode);

	for (auto childNode : childNodes)
	{
		auto component = this->addNode(childNode, parent);

		if (component)
			this->addNodes(childNode, component);
	}
}

void LSG_Modal::Close()
{
	this->visible = false;

	for (auto child : this->children)
		delete child;

	this->children.clear();
	this->components.clear();
	this->componentsByLayer.clear();

	this->destroyTextures();
}

SDL_Rect LSG_Modal::getCloseIcon() const
{
	auto texture = this->textures[LSG_MODAL_TEXTURE_ICON_CLOSE];

	if (this->hideCloseIcon || !texture)
		return {};

	auto textureSize = LSG_Graphics::GetTextureSize(this->textures[LSG_MODAL_TEXTURE_ICON_CLOSE]);

	SDL_Rect closeIcon = {
		(this->background.x + this->background.w - textureSize.width - this->padding),
		(this->background.y + this->padding),
		textureSize.width,
		textureSize.height
	};

	return closeIcon;
}

LSG_Component* LSG_Modal::GetComponent(const std::string& id)
{
	return (this->components.contains(id) ? this->components[id] : nullptr);
}

LSG_Component* LSG_Modal::getComponent(const SDL_Point& mousePosition) const
{
	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	for (auto i = this->componentsByLayer.rbegin(); i != this->componentsByLayer.rend(); i++)
	{
		auto component = (*i).second;

		if (!component->visible)
			continue;

		component->background.y += headerHeight;

		auto background  = LSG_UI::GetScrolledBackground(component);
		bool isMouseOver = SDL_PointInRect(&mousePosition, &background);

		component->background.y -= headerHeight;

		if (isMouseOver && component->IsScrollablePanel())
		{
			if (static_cast<LSG_Panel*>(component)->IsMouseOverScrollbar(mousePosition))
				return component;

			for (auto child : component->GetChildren())
			{
				if (!child->visible)
					continue;

				auto child2 = this->getComponentInScrollablePanel(mousePosition, child);

				if (child2 && child2->visible)
					return child2;
			}
		}

		if (isMouseOver && (component->GetLayer() >= LSG_Component::LayerOffset))
			return component;
	}

	return nullptr;
}

LSG_Component* LSG_Modal::getComponentInScrollablePanel(const SDL_Point& mousePosition, LSG_Component* component) const
{
	if (!component->visible)
		return nullptr;

	if (component->IsPanel())
	{
		for (auto child : component->GetChildren())
		{
			if (!child->visible)
				continue;

			auto child2 = this->getComponentInScrollablePanel(mousePosition, child);

			if (child2 && child2->visible)
				return child2;
		}
	}

	auto position   = LSG_UI::GetScrolledPosition(mousePosition, component);
	auto background = SDL_Rect(component->background);

	background.y += LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	if (SDL_PointInRect(&position, &background))
		return component;

	return nullptr;
}

int LSG_Modal::getSizeFromXmlAttribute(const std::string& size, int maxSize) const
{
	if (!size.empty() && size.ends_with('%'))
		return (int)((double)maxSize * std::atof(size.c_str()) * 0.01);
	else if (!size.empty())
		return LSG_Window::GetDPIScaled(std::atoi(size.c_str()));

	return (maxSize / 2);
}

int LSG_Modal::getSizeFromXmlAttribute(const std::string& maxSize, const std::string& minSize, int size) const
{
	int newSize = size;

	if (!maxSize.empty())
		newSize = std::min(LSG_Window::GetDPIScaled(std::atoi(maxSize.c_str())), newSize);

	if (!minSize.empty())
		newSize = std::max(LSG_Window::GetDPIScaled(std::atoi(minSize.c_str())), newSize);

	return newSize;
}

SDL_Cursor* LSG_Modal::Highlight(const SDL_Point& mousePosition)
{
	this->highlighted = this->isMouseOverCloseIcon(mousePosition);

	for (const auto& component : this->componentsByLayer)
		component.second->highlighted = false;

	if (this->highlighted)
		return SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);

	auto component = this->getComponent(mousePosition);

	if (!component)
		return nullptr;

	component->highlighted = true;

	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	component->background.y += headerHeight;

	if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OffsetBackgroundY(headerHeight);

	auto cursor = LSG_UI::GetCursor(component, mousePosition);

	if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OffsetBackgroundY(-headerHeight);

	component->background.y -= headerHeight;

	return cursor;
}

bool LSG_Modal::isMouseOverCloseIcon(const SDL_Point& mousePosition) const
{
	if (!this->visible || this->hideCloseIcon)
		return false;

	auto closeIcon = this->getCloseIcon();

	return SDL_PointInRect(&mousePosition, &closeIcon);
}

bool LSG_Modal::isMouseOverHeader(const SDL_Point& mousePosition) const
{
	if (!this->visible)
		return false;

	auto header = SDL_Rect(this->background);

	header.h = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	return SDL_PointInRect(&mousePosition, &header);
}

void LSG_Modal::OnKeyDown(const SDL_KeyboardEvent& event)
{
	if (!this->visible)
		return;

	if (!this->hideCloseIcon && (event.keysym.sym == SDLK_ESCAPE)) {
		this->Close();
		return;
	}

	auto component = this->getComponent(LSG_Window::GetMousePosition());

	if (!component) {
		LSG_Events::HandleKeyDownEvent(event, nullptr);
		return;
	}

	if (component->IsModal() || component->IsMenu())
		return;

	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	component->background.y += headerHeight;

	LSG_Events::HandleKeyDownEvent(event, component);

	component->background.y -= headerHeight;
}


void LSG_Modal::OnMouseDown(const SDL_Event& event, const SDL_Point& mousePosition)
{
	if (!this->visible)
		return;

	if (!this->hideCloseIcon && (this->isMouseOverCloseIcon(mousePosition) || !SDL_PointInRect(&mousePosition, &this->background))) {
		this->Close();
		return;
	}

	auto component = this->getComponent(mousePosition);

	if (!component || component->IsModal() || component->IsMenu())
		return;

	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	component->background.y += headerHeight;

	LSG_Events::HandleMouseDownEvent(event, component);

	component->background.y -= headerHeight;
}

void LSG_Modal::OnMouseScroll(const SDL_MouseWheelEvent& event, const SDL_Point& mousePosition) const
{
	if (!this->visible || this->isMouseOverHeader(mousePosition) || !SDL_PointInRect(&mousePosition, &this->background))
		return;

	auto component = this->getComponent(mousePosition);

	if (!component || component->IsModal() || component->IsMenu())
		return;

	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	component->background.y += headerHeight;

	LSG_Events::HandleMouseScrollEvent(event, component);

	component->background.y -= headerHeight;
}

void LSG_Modal::OnMouseUp(const SDL_Event& event, const SDL_Point& mousePosition) const
{
	if (!this->visible)
		return;

	auto component = this->getComponent(mousePosition);

	if (!component || component->IsModal() || component->IsMenu())
		return;

	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	component->background.y += headerHeight;

	if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OffsetBackgroundY(headerHeight);

	LSG_Events::HandleMouseUpEvent(event, component);

	if (component->IsTiles())
		static_cast<LSG_Tiles*>(component)->OffsetBackgroundY(-headerHeight);

	component->background.y -= headerHeight;
}

void LSG_Modal::Open()
{
	LSG_UI::CloseMenu();
	LSG_UI::CloseModals();

	this->visible = true;

	this->addNodes(this->xmlNode, this);

	for (auto child : this->children)
		child->SetColors();

	LSG_UI::LayoutModal(this);
	LSG_UI::SetModal(this);
}

void LSG_Modal::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	this->renderBackdrop(renderer);

	if (this->borderRadius > 0) {
		this->renderFillWithRoundedBorder(renderer, std::format("{}_background", this->id));
	} else {
		this->renderFill(renderer);
		this->renderBorder(renderer);
	}

	auto headerHeight = LSG_Window::GetDPIScaled(LSG_Modal::HeaderHeight);

	this->renderHeader(renderer, headerHeight);

	this->background.y += headerHeight;

	for (auto child : this->children)
	{
		child->background.y += headerHeight;

		if (child->IsPanel())
			static_cast<LSG_Panel*>(child)->OffsetBackgroundY(headerHeight);

		if (child->IsButton())
			static_cast<LSG_Button*>(child)->Render(renderer);
		else if (child->IsCards())
			static_cast<LSG_Cards*>(child)->Render(renderer);
		else if (child->IsImage())
			static_cast<LSG_Image*>(child)->Render(renderer);
		else if (child->IsLine())
			static_cast<LSG_Line*>(child)->Render(renderer);
		else if (child->IsList())
			static_cast<LSG_List*>(child)->Render(renderer);
		else if (child->IsNavigation())
			static_cast<LSG_Navigation*>(child)->Render(renderer);
		else if (child->IsPanel())
			static_cast<LSG_Panel*>(child)->Render(renderer);
		else if (child->IsProgressBar())
			static_cast<LSG_ProgressBar*>(child)->Render(renderer);
		else if (child->IsSlider())
			static_cast<LSG_Slider*>(child)->Render(renderer);
		else if (child->IsTable())
			static_cast<LSG_Table*>(child)->Render(renderer);
		else if (child->IsTextInput())
			static_cast<LSG_TextInput*>(child)->Render(renderer);
		else if (child->IsTextLabel())
			static_cast<LSG_TextLabel*>(child)->Render(renderer);
		else if (child->IsTiles())
			static_cast<LSG_Tiles*>(child)->Render(renderer);
		else if (child->IsToggle())
			static_cast<LSG_Toggle*>(child)->Render(renderer);

		if (child->IsPanel())
			static_cast<LSG_Panel*>(child)->OffsetBackgroundY(-headerHeight);

		child->background.y -= headerHeight;
	}

	this->background.y -= headerHeight;
}

void LSG_Modal::renderBackdrop(SDL_Renderer* renderer) const
{
	auto      background      = LSG_UI::GetBackgroundArea();
	SDL_Color backgroundColor = { 0, 0, 0, 128 };

	LSG_Graphics::RenderFill(renderer, 0, backgroundColor, background);
}

void LSG_Modal::renderHeader(SDL_Renderer* renderer, int headerHeight) const
{
	this->renderHeaderTitle(renderer, headerHeight);
	this->renderHeaderCloseIcon(renderer);
	this->renderHeaderLine(renderer, headerHeight);
}

void LSG_Modal::renderHeaderCloseIcon(SDL_Renderer* renderer) const
{
	auto texture = this->textures[LSG_MODAL_TEXTURE_ICON_CLOSE];

	if (this->hideCloseIcon || !texture)
		return;

	auto destination = this->getCloseIcon();

	SDL_RenderCopy(renderer, texture, nullptr, &destination);

	if (this->highlighted)
		this->renderHighlight(renderer, destination);
}

void LSG_Modal::renderHeaderLine(SDL_Renderer* renderer, int headerHeight) const
{
	auto positionY = (this->background.y + headerHeight);

	SDL_Rect line = {
		(this->background.x + this->padding),
		positionY,
		(this->background.x + this->background.w - this->padding - 1),
		positionY
	};

	auto color = LSG_Graphics::GetThumbColor(this->backgroundColor);

	LSG_Graphics::RenderLine(renderer, color, line.x, line.y, line.w, line.h);
}

void LSG_Modal::renderHeaderTitle(SDL_Renderer* renderer, int headerHeight) const
{
	auto texture = this->textures[LSG_MODAL_TEXTURE_TITLE];

	if (!texture)
		return;

	auto textureSize = LSG_Graphics::GetTextureSize(texture);

	auto iconSize = (!this->hideCloseIcon ? headerHeight : 0);
	auto maxWidth = (this->background.w - iconSize);

	SDL_Rect clip = {
		0,
		0,
		std::min(textureSize.width,  maxWidth),
		std::min(textureSize.height, headerHeight)
	};

	SDL_Rect destination = {
		(this->background.x + ((maxWidth - clip.w) / 2)),
		(this->background.y + ((headerHeight - clip.h) / 2)),
		clip.w,
		clip.h
	};

	SDL_RenderCopy(renderer, texture, &clip, &destination);
}

void LSG_Modal::Set()
{
	this->destroyTextures();

	this->textures.resize(NR_OF_MODAL_TEXTURES);

	if (!this->visible)
		return;

	if (!this->hideCloseIcon)
	{
		auto closeIconSize = LSG_Window::GetDPIScaled(LSG_Modal::CloseIconSize);

		this->textures[LSG_MODAL_TEXTURE_ICON_CLOSE] = LSG_Graphics::GetVector(
			LSG_VECTOR_CLOSE,
			this->textColor,
			{ closeIconSize, closeIconSize }
		);
	}

	auto title = LSG_XML::GetAttribute(this->xmlNode, "title");

	if (!title.empty())
		this->textures[LSG_MODAL_TEXTURE_TITLE] = this->getTexture(title, LSG_Modal::TitleFontSize);
}

void LSG_Modal::SetBackground()
{
	if (!this->visible)
		return;

	auto background    = LSG_UI::GetBackgroundArea();
	auto xmlAttributes = LSG_XML::GetAttributes(this->xmlNode);

	auto width  = (xmlAttributes.contains("width")  ? xmlAttributes.at("width")  : "");
	auto height = (xmlAttributes.contains("height") ? xmlAttributes.at("height") : "");

	this->background.w = this->getSizeFromXmlAttribute(width,  background.w);
	this->background.h = this->getSizeFromXmlAttribute(height, background.h);

	if (width == "height")
		this->background.w = this->background.h;

	if (height == "width")
		this->background.h = this->background.w;

	auto maxWidth = (xmlAttributes.contains("max-width") ? xmlAttributes.at("max-width") : "");
	auto minWidth = (xmlAttributes.contains("min-width") ? xmlAttributes.at("min-width") : "");

	this->background.w = this->getSizeFromXmlAttribute(maxWidth, minWidth, this->background.w);

	auto maxHeight = (xmlAttributes.contains("max-height") ? xmlAttributes.at("max-height") : "");
	auto minHeight = (xmlAttributes.contains("min-height") ? xmlAttributes.at("min-height") : "");

	this->background.h = this->getSizeFromXmlAttribute(maxHeight, minHeight, this->background.h);

	this->background.x = ((background.w - this->background.w) / 2);
	this->background.y = ((background.h - this->background.h) / 2);
}
