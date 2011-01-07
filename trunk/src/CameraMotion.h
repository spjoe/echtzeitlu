/**
* @file CameraMotion.h
* @author Thomas Moerwald, Camillo Dell'mour
* @date Januar 2010
* @version unstable
* @brief Controll the movement of a camera
* @namespace echtzeitlu
*/

#pragma once

#include "camera.h";
//#include "glm/gtc/double_float.hpp"
//#include "glm/gtc/half_float.hpp"
//#include <glm/gtx/epsilon.hpp>;
#include <glm/gtx/epsilon.inl>;

namespace echtzeitlu{
class CameraMotion{
	Camera* camera;
	enum FlyState{
		around,
		movetoState,
		movetoIaround
	};
	FlyState flyState;

	glm::vec3 movetoPoint;
	glm::vec3 pivotPoint;
	float speed;
	bool ccw;

public:
	CameraMotion(Camera* camera);
	void flyaround(glm::vec3 startPoint, glm::vec3 pivotPoint, float speed, bool ccw);
	void moveto(glm::vec3 Point, float speed);
	void update(float dTime);
private:
	void moveto(float dTime);
	void flyaround(float dTime);
};
}