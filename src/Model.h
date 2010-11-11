/**
* @file Model.h
* @author Thomas Mörwald, Camillo Dell'mour
* @date November 2010
* @version unstable
* @brief Dynamic renderable geometric model in scene
* @namespace echtzeitlu
*/

#ifndef _MODEL_H_
#define _MODEL_H_

#include "SceneObject.h"
#include "common.hpp"
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

namespace echtzeitlu{
  
class Model : public SceneObject
{
protected:
	GLuint vbo_id[3];	// Vertex Buffer Object (array not save!)
	GLuint vao_id;		// Vertex Array Object
	Shader *shader;
	
	// Die Vertexliste sollten wir erst mal im Speicher lassen (debugging, animation, ...)
	std::vector<glm::vec4> pointlist;
	std::vector<glm::vec3> normallist;
	std::vector<glm::vec2> texlist;
	std::vector<glm::vec4> colorlist;
	std::vector<unsigned> indexlist;
	glm::mat4 model;
	GLuint texid;
	
public:
	Model( 	std::vector<glm::vec4> &pointlist, std::vector<glm::vec3> &normallist, 
			std::vector<unsigned> &indexlist, Shader* shader);
	Model( 	std::vector<glm::vec4> &pointlist, std::vector<glm::vec3> &normallist, std::vector<glm::vec2> &texturelist,
			std::vector<unsigned> &indexlist, GLuint _texid, Shader* shader);
	Model();
	void print();
	
	/** @brief draws the model */
	virtual void draw();
	
	/** @brief updates the model state 
	*	@param fTime, time elapsed since last frame */
	virtual void update(float fTime);
private:
	void bindVertex(void*, size_t size);
	void bindColor(void*, size_t size);
    void bindNormals(void*, size_t size);
	void bindTexture(void*, size_t size);
};
  
}; // namespace echtzeitlu

#endif

