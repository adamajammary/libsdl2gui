#include "main.h"

#ifndef LSG_IMAGE_H
#define LSG_IMAGE_H

class LSG_Image : public LSG_Component
{
public:
	LSG_Image(const std::string& id, int layer, LibXml::xmlDoc* xmlDoc, LibXml::xmlNode* xmlNode, const std::string& xmlNodeName, LSG_Component* parent);
	~LSG_Image() {}

private:
	std::string file;
	bool        fill;

public:
	virtual void Render(SDL_Renderer* renderer) override;
	void         SetImage(const std::string& file, bool fill = false);
	void         SetImage();

};

#endif
