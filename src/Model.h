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
#include "ModelObject.h"
#include "ModelEffect.h"
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp> 

namespace echtzeitlu{
  
class Model : public SceneObject
{
protected:
	GLuint vbo_id[3];	// Vertex Buffer Object (array not save!)
	GLuint vao_id;		// Vertex Array Object
	//Shader *shader;
	float angle;
	
	// Die Vertexliste sollten wir erst mal im Speicher lassen (debugging, animation, ...)
	std::vector<glm::vec4> pointlist;
	std::vector<glm::vec3> normallist;
	std::vector<glm::vec2> texlist;

	//BumpMap
	std::vector<glm::vec3> InvNormals;
	std::vector<glm::vec3> InvBinormals;
	std::vector<glm::vec3> InvTangents;

	//std::vector<glm::vec4> colorlist;
	std::vector<unsigned> indexlist;
	glm::mat4 model;
	glm::mat4 model_orig;
	//std::vector<GLuint> texidlist;

	ModelEffect *effect;
	
public:
	//Model( 	std::vector<glm::vec4> &pointlist, std::vector<glm::vec3> &normallist, 
	//		std::vector<unsigned> &indexlist, Shader* shader, std::string name, glm::mat4 model = glm::mat4());
	Model( 	std::vector<glm::vec4> &pointlist, std::vector<glm::vec3> &normallist, std::vector<glm::vec2> &texturelist,
			std::vector<unsigned> &indexlist, std::string name, ModelEffect *effect,glm::mat4 model = glm::mat4());
	Model();
	virtual ~Model();
	void print();
	
	/** @brief draws the model */
	virtual void draw();
	virtual void drawSimple(); // for deph map / shadow map
	
	/** @brief updates the model state 
	*	@param fTime, time elapsed since last frame */
	virtual void update(float fTime);

	void initBumpMap();

	//void assignTextureId(GLuint);
	//void assignBumpMapId(GLuint);
private:
	void bindVertex(void*, size_t size);
	void bindColor(void*, size_t size);
    void bindNormals(void*, size_t size);
	void bindTexture(void*, size_t size);

	//adaptiert von http://gpwiki.org/index.php/OpenGL:Tutorials:GLSL_Bump_Mapping
	void FindInvTBN(glm::vec3 Vertices[3], glm::vec2 TexCoords[3], glm::vec3 & InvNormal,
                  glm::vec3 & InvBinormal, glm::vec3 & InvTangent);
};
  
}; // namespace echtzeitlu

#endif

