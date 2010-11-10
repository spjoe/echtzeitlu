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
#include "Effect.h"

namespace echtzeitlu{

class ModelObject
{
private:
	std::string ID;
public:
	std::string getID(){
		return ID;
	}
	void setID(std::string ID){
		this->ID = ID;
	}
};

class ModelImage : public ModelObject{
	
};
class ModelEffect : public ModelObject{
	std::vector<ModelImage*> images;
};
class ModelMaterial : public ModelObject{
	std::vector<ModelEffect*> effects;
};

class ModelLoader
{
private:
	std::map<std::string, ModelEffect> effects;
	std::map<std::string, ModelImage> images;
	std::map<std::string, ModelMaterial> material;

public:
	ModelLoader();
	~ModelLoader(void);
	SceneObject* loadScene(const std::string path);
private:
	SceneObject* ReadScene(domVisual_scene *scene);
	void travers(domNode *node, SceneObject* sceneObject);

	void ReadImageLibrary( domLibrary_imagesRef lib );
	void ReadEffectLibrary( domLibrary_effectsRef lib );
	void ReadMaterialLibrary( domLibrary_materialsRef lib );
	void ReadAnimationLibrary( domLibrary_animationsRef lib );

	ModelImage* ReadImage(domImageRef lib);
};

};
