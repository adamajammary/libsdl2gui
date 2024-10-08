#include "LSG_Modal.h"

LSG_Modal::LSG_Modal(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{
	this->hideCloseIcon        = false;
	this->highlightedChild     = false;
	this->highlightedCloseIcon = false;
	this->visible              = false;

	this->Layout();
}

bool LSG_Modal::CloseOnMouseClick(const SDL_Point& mousePosition)
{
	if (this->hideCloseIcon)
		return false;

	if (!this->visible)
		return true;

	auto closeIcon        = this->getCloseIcon();
	bool clickedCloseIcon = SDL_PointInRect(&mousePosition,  &closeIcon);
	bool clickedOutside   = !SDL_PointInRect(&mousePosition, &this->background);
	bool close            = (clickedCloseIcon || clickedOutside);

	if (close)
		this->SetVisible(false);

	return close;
}

SDL_Rect LSG_Modal::getCloseIcon()
{
	auto textureSize = LSG_Graphics::GetTextureSize(this->textures[LSG_MODAL_TEXTURE_ICON_CLOSE]);
	auto padding     = LSG_Graphics::GetDPIScaled(LSG_Modal::Padding);

	SDL_Rect closeIcon = {
		(this->background.x + this->background.w - textureSize.width - this->padding - padding),
		(this->background.y + this->padding + padding),
		textureSize.width,
		textureSize.height
	};

	return closeIcon;
}

bool LSG_Modal::Highlight(const SDL_Point& mousePosition)
{
	this->highlightedCloseIcon = this->isMouseOverIconClose(mousePosition);
	this->highlightedChild     = false;

	this->highlight(this, mousePosition);

	return (this->highlightedCloseIcon || this->highlightedChild);
}

void LSG_Modal::highlight(LSG_Component* component, const SDL_Point& mousePosition)
{
	if (!component)
		return;

	if (component->visible && component->enabled)
	{
		component->highlighted = SDL_PointInRect(&mousePosition, &component->background);

		if (component->highlighted && component->IsButton())
			this->highlightedChild = true;
	}

	for (auto child : component->GetChildren())
		this->highlight(child, mousePosition);
}

bool LSG_Modal::IsModalChild(LSG_Component* component)
{
	if (!component)
		return false;

	if (component->IsModal())
		return true;

	return LSG_Modal::IsModalChild(component->GetParent());
}

bool LSG_Modal::isMouseOverIconClose(const SDL_Point& mousePosition)
{
	if (!this->enabled || !this->visible || this->hideCloseIcon)
		return false;

	auto iconClose = this->getCloseIcon();

	return SDL_PointInRect(&mousePosition, &iconClose);
}

void LSG_Modal::Layout()
{
	auto parentBackground = LSG_UI::GetBackgroundArea();
	auto attributes       = LSG_XML::GetAttributes(this->xmlNode);

	this->setSize(attributes,     parentBackground);
	this->setPosition(attributes, parentBackground);

	this->setBorder(attributes);
	this->setHideCloseIcon(attributes);
	this->setMargin(attributes);
	this->setPadding(attributes);

	this->setTextures(attributes.contains("title") ? attributes["title"] : "");
}

bool LSG_Modal::OnKeyDown(const SDL_KeyboardEvent& event)
{
	if (!this->visible || this->hideCloseIcon || (event.keysym.sym != SDLK_ESCAPE))
		return false;

	this->SetVisible(false);

	return true;
}

void LSG_Modal::Render(SDL_Renderer* renderer)
{
	if (!this->visible)
		return;

	auto      background      = LSG_UI::GetBackgroundArea();
	SDL_Color backgroundColor = { 0, 0, 0, 128 };

	this->renderFill(renderer, 0, backgroundColor, background);

	LSG_Component::Render(renderer);

	this->renderHeader(renderer);
}

void LSG_Modal::renderHeader(SDL_Renderer* renderer)
{
	this->renderHeaderTitle(renderer);
	this->renderHeaderCloseIcon(renderer);
	this->renderHeaderLine(renderer);
}

void LSG_Modal::renderHeaderCloseIcon(SDL_Renderer* renderer)
{
	if (this->hideCloseIcon)
		return;

	auto closeIcon = this->getCloseIcon();

	SDL_RenderCopy(renderer, this->textures[LSG_MODAL_TEXTURE_ICON_CLOSE], nullptr, &closeIcon);

	if (this->highlightedCloseIcon)
		this->renderHighlight(renderer, closeIcon);
}

void LSG_Modal::renderHeaderLine(SDL_Renderer* renderer)
{
	auto height      = LSG_Graphics::GetDPIScaled(LSG_Modal::Height);
	auto padding     = LSG_Graphics::GetDPIScaled(LSG_Modal::Padding);
	auto borderY     = (this->background.y + this->padding + height);
	auto borderColor = LSG_Graphics::GetThumbColor(this->backgroundColor);

	SDL_Rect border = {
		(this->background.x + this->padding + padding),
		borderY,
		(this->background.x + this->background.w - this->padding - padding - 1),
		borderY
	};

	SDL_SetRenderDrawColor(renderer, borderColor.r, borderColor.g, borderColor.b, 255);
	SDL_RenderDrawLine(renderer, border.x, border.y, border.w, border.h);
}

void LSG_Modal::renderHeaderTitle(SDL_Renderer* renderer)
{
	auto texture = this->textures[LSG_MODAL_TEXTURE_TITLE];

	if (!texture)
		return;

	auto height      = LSG_Graphics::GetDPIScaled(LSG_Modal::Height);
	auto textureSize = LSG_Graphics::GetTextureSize(texture);
	auto maxWidth    = (this->background.w - this->padding - (!this->hideCloseIcon ? height : 0) - this->padding);

	SDL_Rect clip = {
		0,
		0,
		std::min(textureSize.width,  maxWidth),
		std::min(textureSize.height, height)
	};

	SDL_Rect destination = {
		(this->background.x + ((maxWidth - textureSize.width)  / 2)),
		(this->background.y + (((this->padding + height + this->padding) - textureSize.height) / 2)),
		textureSize.width,
		textureSize.height
	};

	SDL_RenderCopy(renderer, texture, &clip, &destination);
}

void LSG_Modal::setBorder(const LSG_UMapStrStr& attributes)
{
	auto border = (attributes.contains("border") ? attributes.at("border") : "");

	if (!border.empty())
		this->border = LSG_Graphics::GetDPIScaled(std::atoi(border.c_str()));
}

void LSG_Modal::setHideCloseIcon(const LSG_UMapStrStr& attributes)
{
	auto hideCloseIcon = (attributes.contains("hide-close-icon") ? attributes.at("hide-close-icon") : "");

	this->hideCloseIcon = (hideCloseIcon == "true");
}

void LSG_Modal::setMargin(const LSG_UMapStrStr& attributes)
{
	auto margin = (attributes.contains("margin") ? attributes.at("margin") : "");

	if (!margin.empty())
		this->margin = LSG_Graphics::GetDPIScaled(std::atoi(margin.c_str()));
}

void LSG_Modal::setPadding(const LSG_UMapStrStr& attributes)
{
	auto padding = (attributes.contains("padding") ? attributes.at("padding") : "");

	if (!padding.empty())
		this->padding = LSG_Graphics::GetDPIScaled(std::atoi(padding.c_str()));
}

void LSG_Modal::setPosition(const LSG_UMapStrStr& attributes, const SDL_Rect& parentBackground)
{
	auto positionX = (attributes.contains("x") ? attributes.at("x") : "");
	auto positionY = (attributes.contains("y") ? attributes.at("y") : "");

	if (!positionX.empty())
		this->background.x = std::atoi(positionX.c_str());
	else
		this->background.x = ((parentBackground.w - this->background.w) / 2);

	if (!positionY.empty())
		this->background.y = std::atoi(positionY.c_str());
	else
		this->background.y = ((parentBackground.h - this->background.h) / 2);
}

void LSG_Modal::setSize(const LSG_UMapStrStr& attributes, const SDL_Rect& parentBackground)
{
	auto width  = (attributes.contains("width")  ? attributes.at("width")  : "");
	auto height = (attributes.contains("height") ? attributes.at("height") : "");

	bool isWidthPercent  = (!width.empty()  && (width[width.length() - 1]   == '%'));
	bool isHeightPercent = (!height.empty() && (height[height.length() - 1] == '%'));

	if (isWidthPercent)
		this->background.w = (int)((double)parentBackground.w * std::atof(width.c_str()) * 0.01);
	else if (!width.empty())
		this->background.w = LSG_Graphics::GetDPIScaled(std::atoi(width.c_str()));
	else
		this->background.w = (int)(parentBackground.w / 2);

	if (isHeightPercent)
		this->background.h = (int)((double)parentBackground.h * std::atof(height.c_str()) * 0.01);
	else if (!height.empty())
		this->background.h = LSG_Graphics::GetDPIScaled(std::atoi(height.c_str()));
	else
		this->background.h = (int)(parentBackground.h / 2);

	if (width == "height")
		this->background.w = this->background.h;

	if (height == "width")
		this->background.h = this->background.w;

	auto maxWidth  = (attributes.contains("max-width")  ? attributes.at("max-width")  : "");
	auto maxHeight = (attributes.contains("max-height") ? attributes.at("max-height") : "");
	auto minWidth  = (attributes.contains("min-width")  ? attributes.at("min-width")  : "");
	auto minHeight = (attributes.contains("min-height") ? attributes.at("min-height") : "");

	if (!maxWidth.empty())
		this->background.w = std::min(LSG_Graphics::GetDPIScaled(std::atoi(maxWidth.c_str())), this->background.w);

	if (!maxHeight.empty())
		this->background.h = std::min(LSG_Graphics::GetDPIScaled(std::atoi(maxHeight.c_str())), this->background.h);

	if (!minWidth.empty())
		this->background.w = std::max(LSG_Graphics::GetDPIScaled(std::atoi(minWidth.c_str())), this->background.w);

	if (!minHeight.empty())
		this->background.h = std::max(LSG_Graphics::GetDPIScaled(std::atoi(minHeight.c_str())), this->background.h);
}

void LSG_Modal::setTextures(const std::string& title)
{
	this->destroyTextures();

	this->textures.resize(NR_OF_MODAL_TEXTURES);

	auto     iconSizeScaled = LSG_Graphics::GetDPIScaled(LSG_Modal::CloseIconSize);
	SDL_Size closeIconSize  = { iconSizeScaled, iconSizeScaled };

	this->textures[LSG_MODAL_TEXTURE_ICON_CLOSE] = LSG_Graphics::GetVectorClose(this->textColor, closeIconSize);

	if (!title.empty())
		this->textures[LSG_MODAL_TEXTURE_TITLE] = this->getTexture(title, LSG_Modal::TitleFontSize);
}

void LSG_Modal::Update()
{
	this->setTextures(LSG_XML::GetAttribute(this->xmlNode, "title"));
}
