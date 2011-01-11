#include "ModelEffect.h"

using namespace echtzeitlu;

ModelEffect::ModelEffect(Shader * shader)
{
	this->bbumpMap = this->bcolorList = this->btexture = false;
	this->shader = shader;
}
