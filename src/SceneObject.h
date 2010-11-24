 /**
 * @file SceneObject.h
 * @author Thomas Mörwald, Camillo Dell'mour
 * @date November 2010
 * @version unstable
 * @brief Dynamic renderable generic object in scene
 * @namespace echtzeitlu
 */

#ifndef _SCENE_OBJECT_H_
#define _SCENE_OBJECT_H_

#include <vector>

namespace echtzeitlu{
  
class SceneObject
{
  protected:
	std::vector<SceneObject*> children;
	
  public:
	SceneObject() : children(){};
	virtual ~SceneObject();
	
	virtual void draw(){ drawChildren(); };
	virtual void update(float fTime){ updateChildren(fTime); };

	void add(SceneObject * child){
		children.push_back(child);
	};
  protected:
	void drawChildren(){
		std::vector<SceneObject*>::iterator childrenIterator;
		for(childrenIterator = children.begin(); 
				childrenIterator != children.end();
				childrenIterator++)
		{
			(*childrenIterator)->draw();
			(*childrenIterator)->drawChildren();
		}
	};
	void updateChildren(float ftime){
		std::vector<SceneObject*>::iterator childrenIterator;
		for(childrenIterator = children.begin(); 
				childrenIterator != children.end();
				childrenIterator++)
		{
			(*childrenIterator)->update(ftime);
			(*childrenIterator)->updateChildren(ftime);
		}
	};
};

}; // namespace echtzeitlu


#endif
