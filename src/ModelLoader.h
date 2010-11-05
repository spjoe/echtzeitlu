/**
* @file ModelLoader.h
* @author Thomas Mörwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Loads the dae int internal fomrat
* @namespace echtzeitlu
*/

#pragma once

#include <dae.h>
#include <dom.h>
#include <dom/domVisual_scene.h>

namespace echtzeitlu{

class ModelLoader
{
private:
	daeElement* defaultScene;
public:
	ModelLoader(const std::string path);
	~ModelLoader(void);
	void loadScene();
private:
	void ReadScene(domVisual_scene *scene);
	void travers(domNode *node);
};

};