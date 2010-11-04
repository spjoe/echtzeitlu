
#include "Model.h"

using namespace echtzeitlu;

Model::Model(GLuint vbo_id[3], GLuint vao_id)
{
	memcpy (this->vbo_id,vbo_id, 3 * sizeof(GLuint));
	this->vao_id = vao_id;
}

void Model::draw()
{
  
}

void Model::update(float fTime)
{
  
}