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
#include <dom/domImage.h>
#include "Model.h"
#include "Effect.h"
#include "ModelObject.h"
#include "ModelEffect.h"
#include "ModelImage.h"
#include "ModelMaterial.h"

namespace echtzeitlu{


class ModelLoader
{
private:
	std::map<std::string, ModelEffect*> effects;
	std::map<std::string, ModelImage*> images;
	std::map<std::string, ModelMaterial*> materials;

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
	ModelEffect *ReadEffect( domEffectRef lib );
	ModelMaterial *ReadMaterial( domMaterialRef lib );
};

};
