
#include "camera.h"
#include <glm/gtx/transform.hpp>
#include <glm/gtx/transform2.hpp>
#include <glm/gtc/matrix_projection.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <stdio.h>

using namespace echtzeitlu;

void Camera::perspective(float fovy, float aspect, float near, float far)
{
	intrinsic = glm::perspective(fovy, aspect, near, far);
}

void Camera::lookat(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up)
{
	pvu2fsu(pos, view, up);
}

void Camera::translate(const glm::vec3 &v)
{
	p = p + v;
	fsu2extrinsic();
}

void Camera::translateF(float d){
	p = p + f * d;
	fsu2extrinsic();
}

void Camera::translateS(float d){
	p = p + s * d;
	fsu2extrinsic();
}

void Camera::translateU(float d){
	p = p + u * d;
	fsu2extrinsic();
}

void Camera::rotateF(float a){
	s = glm::rotate(s, a, f);
	u = glm::rotate(u, a, f);
	fsu2extrinsic();
}

void Camera::rotateS(float a){
	f = glm::rotate(f, a, s);
	u = glm::rotate(u, a, s);
	fsu2extrinsic();
}

void Camera::rotateU(float a){
	f = glm::rotate(f, a, u);
	s = glm::rotate(s, a, u);
	fsu2extrinsic();
}

void Camera::rotateX(float a){
	f = glm::rotateX(f,a);
	s = glm::rotateX(s,a);
	u = glm::rotateX(u,a);
	fsu2extrinsic();
}

void Camera::rotateY(float a){
	f = glm::rotateY(f,a);
	s = glm::rotateY(s,a);
	u = glm::rotateY(u,a);
	fsu2extrinsic();
}

void Camera::rotateZ(float a){
	f = glm::rotateZ(f,a);
	s = glm::rotateZ(s,a);
	u = glm::rotateZ(u,a);
	fsu2extrinsic();
}

void Camera::orbit(const glm::vec3 &origin, const glm::vec3 &normal, float a)
{
	glm::vec3 r = p - origin;
	r = glm::rotate(r, a, normal);
	p = origin + r;
	
	f = glm::rotate(f, a, normal);
	s = glm::rotate(s, a, normal);
	u = glm::rotate(u, a, normal);
	fsu2extrinsic();
}

void Camera::pvu2fsu(const glm::vec3 &pos, const glm::vec3 &view, const glm::vec3 &up)
{
	f = view - pos; 
	f = glm::normalize(f);
	
	s = glm::cross(f,up);
	s = glm::normalize(s);
	
	u = glm::cross(s,f);
	u = glm::normalize(u);
	
	p = pos;
	
	fsu2extrinsic();
}

void Camera::fsu2extrinsic()
{
	glm::mat4 R(	s.x,  u.x, -f.x,  0.0,
					s.y,  u.y, -f.y,  0.0,
					s.z,  u.z, -f.z,  0.0,
					0.0,	0.0,	0.0,  1.0);
									
	glm::mat4 t(	1.0,  0.0, 0.0, 0.0,
					0.0,  1.0, 0.0, 0.0,
					0.0,  0.0, 1.0, 0.0,
					-p.x,	-p.y, -p.z, 1.0);

	extrinsic = R * t;
}