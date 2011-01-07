#include "CameraMotion.h"

using namespace echtzeitlu;

CameraMotion::CameraMotion(Camera* c)
{
	camera = c;
}

void CameraMotion::update(float dTime)
{
	switch(flyState){
	case movetoIaround:{
		glm::vec3 epsi(0.01,0.01,0.01);
		glm::detail::tvec3<bool> a = glm::gtx::epsilon::equalEpsilon(camera->p,movetoPoint,epsi);
		if(a.x && a.y && a.z)
			flyState = around;
		else
			moveto(dTime);
					   }
	case around:
		flyaround(dTime);
	case movetoState:
		moveto(dTime);
	default:
		;
	}
}

void CameraMotion::flyaround(glm::vec3 startPoint, glm::vec3 pivotPoint, float speed, bool ccw)
{
	flyState = movetoIaround;
	movetoPoint  = startPoint;
	this->pivotPoint = pivotPoint; 
	this->speed = speed;
	this->ccw = ccw;
}

void CameraMotion::moveto(glm::vec3 Point, float speed)
{
	flyState = movetoState;
	movetoPoint  = Point;
	this->speed = speed;
}

void CameraMotion::moveto(float dTime)
{ 
	glm::vec3 direction = movetoPoint - camera->p;
	direction = glm::normalize(direction);
	direction = speed * dTime * direction;
	camera->translate(direction);
}
void CameraMotion::flyaround(float dTime)
{
	glm::vec3 direction = movetoPoint - camera->p;
	direction = glm::normalize(direction);
	direction = speed * dTime * direction;
	camera->translate(direction);
	//camera->lookat(pivotPoint,
}