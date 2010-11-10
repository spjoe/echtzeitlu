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
private:
	GLuint _id;
public:
	ModelImage(domImage* img){
		domImage* imageElement = img;
	
		if ( !imageElement )
			return;

		imageElement->getInit_from()->getValue().str();
			
		const std::string file = cdom::uriToNativePath(imageElement->getInit_from()->getValue().str());


		GLFWimage image;
		if (glfwReadImage(file.c_str(), &image, GLFW_ORIGIN_UL_BIT) != GL_TRUE)
			return;
 
		glGenTextures(1, &_id);
		glBindTexture(GL_TEXTURE_2D, _id);
 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
 
		glTexImage2D(GL_TEXTURE_2D, 0, image.Format, image.Width, image.Height,
					 0, image.Format, GL_UNSIGNED_BYTE,
					 reinterpret_cast<void*>(image.Data));
		glfwFreeImage(&image);
	}

	unsigned getTexId(){
		return _id;
	}
};
class ModelEffect : public ModelObject{
	std::vector<ModelImage*> images;
public:
	void addImage(ModelImage* img){images.push_back(img);}
};
class ModelMaterial : public ModelObject{
private:
	ModelEffect* effect;
public:
	ModelMaterial(ModelEffect* effect){
		this->effect = effect;
	}
	ModelEffect* getEffect(){ return effect; }
};

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
