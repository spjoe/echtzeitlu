/**
* @file ModelLoader.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Intermediate representation of a material
* @namespace echtzeitlu
*/

#pragma once

#include "ModelObject.h"
#include "ModelEffect.h"

namespace echtzeitlu{
class ModelMaterial : public ModelObject{
private:
	ModelEffect* effect;
public:
	ModelMaterial(ModelEffect* effect){
		this->effect = effect;
	}
	ModelEffect* getEffect(){ return effect; }
};
}
