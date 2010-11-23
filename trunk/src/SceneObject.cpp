
#include "SceneObject.h"

using namespace echtzeitlu;

SceneObject::~SceneObject()
{
	std::vector<SceneObject*>::iterator it;
	
	while(it != children.end()){
		delete(*it);
		it++;
	}
}