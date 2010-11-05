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
  private:
	std::vector<SceneObject*> children;
	
  public:
	  SceneObject() : children(){};
	~SceneObject();
	
	virtual void draw()=0;
	virtual void update(float fTime)=0;

	void add(SceneObject * child){
		children.push_back(child);
	};
  protected:
	void drawAll(){
		std::vector<SceneObject*>::iterator childrenIterator;
		for(childrenIterator = children.begin(); 
				childrenIterator != children.end();
				childrenIterator++)
		{
			(*childrenIterator)->draw();
		}
	};
	void updateAll(float ftime){
		std::vector<SceneObject*>::iterator childrenIterator;
		for(childrenIterator = children.begin(); 
				childrenIterator != children.end();
				childrenIterator++)
		{
			(*childrenIterator)->update(ftime);
		}
	};
};

}; // namespace echtzeitlu


#endif
