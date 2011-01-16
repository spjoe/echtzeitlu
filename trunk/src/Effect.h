/**
* @file Effect.h
* @author Thomas Mörwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief An effect in the scene (Not used, Effect is now ModelEffect a Object in the Model Class)
* @namespace echtzeitlu
*/

#ifndef _EFFECT_H_
#define _EFFECT_H_

#include "SceneObject.h"

namespace echtzeitlu{

class Effect : public SceneObject
{
  private:
	
  public:
	
	/** @brief draws the effect */
	virtual void draw();
	
	/** @brief updates the effect state 
	*	@param fTime, time elapsed since last frame */
	virtual void update(float fTime);
	
};
  
}; // namespace echtzeitlu

#endif