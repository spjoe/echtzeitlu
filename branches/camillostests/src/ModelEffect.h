/**
* @file ModelLoader.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Intermediate representation of an Effect
* @namespace echtzeitlu
*/

#pragma once

#include "ModelObject.h"
#include "ModelImage.h"

namespace echtzeitlu{
class ModelEffect : public ModelObject{
	std::vector<ModelImage*> images;
public:
	void addImage(ModelImage* img){images.push_back(img);}
	std::vector<ModelImage*> getImages(){return images;}
};
}
