#include "main.h"

#ifndef LSG_IRENDERABLE_H
#define LSG_IRENDERABLE_H

class LSG_IRenderable
{
public:
	LSG_IRenderable() {}
	virtual ~LSG_IRenderable() {}

public:
	virtual void Render(SDL_Renderer* renderer) = 0;

};

#endif
