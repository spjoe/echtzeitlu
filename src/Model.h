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
#include "shader.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

namespace echtzeitlu{
  
class Model : public SceneObject
{
private:
	GLuint vbo_id[3];	// Vertex Buffer Object (array not save!)
	GLuint vao_id;		// Vertex Array Object
	
	std::vector<GLuint> indices;
	size_t numVertecies;  // 
	Shader *shader;
	glm::mat4 model;
	
public:
	Model(GLuint[3], GLuint, std::vector<GLuint> &, size_t numVertecies, Shader*);
	Model();
	/** @brief draws the model */
	virtual void draw();
	
	/** @brief updates the model state 
	*	@param fTime, time elapsed since last frame */
	virtual void update(float fTime);

	void bindVertex(void*, size_t size);
	void bindColor(void*, size_t size);
    void bindNormals(void*, size_t size);
};
  
}; // namespace echtzeitlu

