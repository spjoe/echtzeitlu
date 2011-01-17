#include "CameraMotion.h"

using namespace echtzeitlu;

CameraMotion::CameraMotion(Camera* c)
{
	camera = c;
}

void CameraMotion::update(float dTime)
{
	switch(flyState){
	case movetoIaround:
		{
		glm::vec3 epsi(0.05,0.05,0.05);
		glm::detail::tvec3<bool> a = glm::gtx::epsilon::equalEpsilon(camera->p,movetoPoint,epsi);
		if(a.x && a.y && a.z)
			flyState = around;
		else
			moveto(dTime);
		break;
		}
	case around:
		{
		flyaround(dTime);
		break;
		}
	case movetoState:
		{
		moveto(dTime);
		break;
		}
	default:
		break;
	}
}

void CameraMotion::flyaround(glm::vec3 startPoint, glm::vec3 center, glm::vec3 pivotPoint, float speed, bool ccw)
{
	flyState = movetoIaround;
	movetoPoint  = startPoint;
	this->pivotPoint = pivotPoint;
	this->center = center;
	this->speedrad = speed;
	this->speedunits = 10;
	this->ccw = ccw;
}

void CameraMotion::moveto(glm::vec3 Point, float speed)
{
	flyState = movetoState;
	movetoPoint  = Point;
	this->speedunits = speed;
}

void CameraMotion::moveto(float dTime)
{ 
	glm::vec3 direction = movetoPoint - camera->p;
	direction = glm::normalize(direction);
	direction = speedunits * dTime * direction;
	camera->translate(direction);
}
void CameraMotion::flyaround(float dTime)
{
	float r = glm::distance(camera->p,center);
	float alpha = speedrad * dTime;
	glm::vec3 p1 = camera->p;
	glm::vec3 c = center;
	glm::vec3 p2(10,0,10); //todo
	p2 = r * glm::normalize(p2);

	glm::vec3 cp1 = p1 - c; 
	glm::vec3 cp2 = p2 - c;

	glm::vec3 nepsilon = glm::cross(cp1,cp2);
	glm::vec3 nh = cp1;
	glm::vec3 ag = glm::cross(nepsilon,nh);
        float tmp  = r * cos(alpha);
        float tmp2 = r * sin(alpha);
        glm::vec3 Y = c + tmp * glm::normalize(cp1);
	glm::vec3 X;
	if(!ccw){
                X = Y - tmp2 * glm::normalize(ag);
		glm::mat3 m(c,Y,X);
		if(glm::determinant(m) > 0)
                        X = Y + tmp2 * glm::normalize(ag);
	}else{
                X = Y + tmp2 * glm::normalize(ag);
		glm::mat3 m(c,Y,X);
		if(glm::determinant(m) < 0)
                        X = Y - tmp2 * glm::normalize(ag);
	}

	//camera->p = X;
	//camera->translate(glm::vec3(0,0,0));
	camera->lookat(X,pivotPoint,glm::vec3(0,0,1));
	//camera->orbit(c,glm::normalize(ag),glm::degrees(alpha));
}
void CameraMotion::save()
{
	save_camera = *camera;
}
void CameraMotion::restore()
{
	camera->p = save_camera.p;
	camera->f = save_camera.f;
	camera->s = save_camera.s;
	camera->extrinsic = save_camera.extrinsic;
	camera->intrinsic = save_camera.intrinsic;
}