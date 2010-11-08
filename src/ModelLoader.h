/**
* @file ModelLoader.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Loads the dae int internal fomrat
* @namespace echtzeitlu
*/

#pragma once

#include <dae.h>
#include <dom.h>
#include <dom/domVisual_scene.h>
#include "Model.h"

namespace echtzeitlu{

class ModelLoader
{
private:
	
public:
	ModelLoader();
	~ModelLoader(void);
	SceneObject* loadScene(const std::string path);
private:
	SceneObject* ReadScene(domVisual_scene *scene);
	void travers(domNode *node, SceneObject* sceneObject);
};

};
