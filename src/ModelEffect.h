/**
* @file ModelLoader.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Intermediate representation of an Effect
* @namespace echtzeitlu
*/

#pragma once
#include <glm/glm.hpp>
#include "shader.hpp"
#include "ModelObject.h"
#include "ModelImage.h"


namespace echtzeitlu{

class ModelEffect : public ModelObject{
	//std::vector<ModelImage*> images;
	bool bbumpMap;
	bool btexture;
	bool bcolorList;
	ModelImage* bumpMap;
	ModelImage* texture;
	std::vector<glm::vec4>* colorList;
	Shader* shader;
public:
	float specular;
	unsigned powspecular;
public:
	//void addImage(ModelImage* img){images.push_back(img);}
	//std::vector<ModelImage*> getImages(){return images;}
	ModelEffect(Shader* shader);

	bool hasTexture(void){return btexture;}
	bool hasBumpMap(void){return bbumpMap;}
	bool hasColorList(void){return bcolorList;}

	ModelImage* getTexture(void){return texture;}
	void setTexture(ModelImage* mi){texture = mi; btexture=true;}
	ModelImage* getBumpMap(void){return bumpMap;}
	void setBumpMap(ModelImage* mi){bumpMap = mi; bbumpMap=true;}

	std::vector<glm::vec4>* getColorList(void){return colorList;}
	void setColorList(std::vector<glm::vec4> *cl){colorList = cl; bcolorList = true;}

	Shader * getShader(void){return shader;}
	void setShader(Shader* shader){this->shader = shader;}
};
}
