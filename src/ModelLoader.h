#pragma once

#include <dae.h>
#include <dom.h>
#include <dom/domVisual_scene.h>

class ModelLoader
{
public:
	ModelLoader(void);
	~ModelLoader(void);
private:
	void ReadScene(domVisual_scene *scene);
};

