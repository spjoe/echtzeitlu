/**
* @file ModelLoader.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Superclass of an intermediate representation
* @namespace echtzeitlu
*/

#pragma once

#include <dae.h>
#include <dom.h>
#include <dom/domVisual_scene.h>
#include <dom/domImage.h>

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
}
