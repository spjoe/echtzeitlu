
#include <glm/glm.hpp>

#include "shader.hpp"

#pragma once;


namespace echtzeitlu{
  
class Camera{
  public:
	glm::mat4 intrinsic;
	glm::mat4 extrinsic;
	
	glm::vec3 f;	// front
	glm::vec3 s;	// side
	glm::vec3 u;	// up
	glm::vec3 p;	// position

	void perspective(float fovy, float aspect, float near, float far);
	void lookat(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up);
	
	void translate(const glm::vec3 &v);
	void translateF(float d);
	void translateS(float d);
	void translateU(float d);
	
	void rotateF(float a);	
	void rotateS(float a);
	void rotateU(float a);
	
	void rotateX(float a);
	void rotateY(float a);
	void rotateZ(float a);
	
	void orbit(const glm::vec3 &origin, const glm::vec3 &normal, float a);
	
	void apply(Shader* shader);
	
	
  private:
	void fsu2extrinsic();
	void pvu2fsu(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up);
};
  
  
}; // namespace