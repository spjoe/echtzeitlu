/**
* @file ModelLoader.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Intermediate representation of an image
* @namespace echtzeitlu
*/

#pragma once

#include <dae.h>
#include <dom.h>
#include <dom/domVisual_scene.h>
#include <dom/domImage.h>
#include "Model.h"
#include "Effect.h"
#include "ModelObject.h"

namespace echtzeitlu{

class ModelImage : public ModelObject{
private:
	GLuint _id;
public:
	ModelImage(domImage* img);

	unsigned getTexId();
};
}
