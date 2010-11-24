
#include "SceneObject.h"

using namespace echtzeitlu;

SceneObject::~SceneObject()
{
	std::vector<SceneObject*>::iterator it = children.begin();
	
	while(it != children.end()){
		delete(*it);
		it++;
	}
}