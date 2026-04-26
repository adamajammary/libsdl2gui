#include "LSG_TextLabel.h"

LSG_TextLabel::LSG_TextLabel(const std::string& id, int layer, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent)
	: LSG_Text(id, layer, xmlNode, xmlNodeName, parent)
{}

SDL_Size LSG_TextLabel::GetSize()
{
	if (!this->texture)
		this->setTexture();

	return this->getTextureSize();
}

void LSG_TextLabel::Render(SDL_Renderer* renderer, const SDL_Point& position)
{
	if (!this->visible)
		return;

	this->background.x = position.x;
	this->background.y = position.y;

	this->render(renderer);
}

void LSG_TextLabel::Render(SDL_Renderer* renderer)
{
	if (this->visible)
		this->render(renderer);
}

void LSG_TextLabel::render(SDL_Renderer* renderer)
{
	if (!this->texture)
		return;

	if (this->parent->IsScrollablePanel(true)) {
		SDL_RenderCopy(renderer, this->texture, nullptr, &this->background);
		return;
	}

	if (this->textOverflow == LSG_TEXT_OVERFLOW_ELLIPSIS)
		this->renderEllipsis(renderer);
	else
		this->renderClip(renderer);
}

void LSG_TextLabel::renderClip(SDL_Renderer* renderer)
{
	SDL_Rect clip = { 0, 0, this->background.w, this->background.h };

	SDL_RenderCopy(renderer, this->texture, &clip, &this->background);
}

void LSG_TextLabel::renderEllipsis(SDL_Renderer* renderer)
{
	auto textSize = this->getTextureSize();

	if (textSize.width <= this->background.w) {
		this->renderClip(renderer);
		return;
	}

	this->renderTextOverflowEllipse(renderer, this->texture, this->background, this->background.w);
}

void LSG_TextLabel::Set(const std::string &text)
{
	if (text == this->text)
		return;

	this->text = text;

	this->Set();
}

void LSG_TextLabel::Set()
{
	this->destroyTextures();

	if (this->ellipsisTexture) {
		SDL_DestroyTexture(this->ellipsisTexture);
		this->ellipsisTexture = nullptr;
	}

	this->setTexture();
}

void LSG_TextLabel::setTexture()
{
	if (this->text.empty())
		return;

	this->texture = this->getTexture(this->text);

	if (this->textOverflow == LSG_TEXT_OVERFLOW_ELLIPSIS)
		this->ellipsisTexture = this->getTexture("...");
}
