/**
* @file Model.h
* @author Thomas Mörwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Dynamic renderable geometric model in scene
* @namespace echtzeitlu
*/

#include "SceneObject.h"
#include "common.hpp"

namespace echtzeitlu{
  
class Model : public SceneObject
{
private:
	GLuint vbo_id[3];	// Vertex Buffer Object (array not save!)
	GLuint vao_id;		// Vertex Array Object
	
public:
	
	/** @brief draws the model */
	virtual void draw();
	
	/** @brief updates the model state 
	*	@param fTime, time elapsed since last frame */
	virtual void update(float fTime);
  
};
  
}; // namespace echtzeitlu

