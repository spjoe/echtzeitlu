
#include "Model.h"
#include "camera.h"
#include "Lighting.h"


#include <stdio.h>
#include <string.h>
#include <glm/gtx/transform.hpp>

#ifndef PI
#define PI 3.141592654
#endif

using namespace echtzeitlu;

extern Camera m_camera_1;
extern Lighting *m_lighting;
//extern glm::mat4 model;
// extern Shader* defaultShader;
extern Shader* simpleShader;
bool animation = true;

char errmsg[128];

Model::Model( 	std::vector<glm::vec4> &pointlist, std::vector<glm::vec3> &normallist,  std::vector<glm::vec2> &texturelist,
			std::vector<GLuint> &indexlist, std::string name, ModelEffect *effect, glm::mat4 model)
{
	if(pointlist.size() != normallist.size()){
		printf("[Model::Model] Warning: pointlist.size() != normallist.size()\n");
		return;
	}
	
	this->pointlist = pointlist;
	this->normallist = normallist;
	this->indexlist = indexlist;
	this->texlist = texturelist;
	this->model = model;
	this->model_orig = model;
	//this->colorlist.assign(pointlist.size(), glm::vec4(0.5, 0.5, 0.5, 1));
	//this->texidlist.push_back(40000);
	this->name = name;
	this->effect = effect;
	printf("%s\n", name.c_str());
}

void Model::Init(){
	get_errors("Model::Model() A");
	PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
	my_glGenVertexArrays(1, &vao_id);
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors("Model::Model() B");
	GLuint* tmp_vbo_id = GenerateVBO(3);
	vbo_id[0] = tmp_vbo_id[0];
	vbo_id[1] = tmp_vbo_id[1];
	vbo_id[2] = tmp_vbo_id[2];
	
	get_errors("Model::Model() C");
	
	
	bindVertex(&pointlist[0], pointlist.size() * 4 * sizeof(GLfloat));
	//bindVertex(&pointlist[0], pointlist.size() * 4 * sizeof(GLfloat),simpleShader);
	if(effect->hasColorList()){
		bindColor(&(*(effect->getColorList()))[0], effect->getColorList()->size() * 4 * sizeof(GLfloat));
	}else{
		bindTexture(&texlist[0], texlist.size() * 2 * sizeof(GLfloat));
	}
	get_errors("Model::Model() D");
	
	bindNormals(&normallist[0], normallist.size() * 3 * sizeof(GLfloat));
	sprintf(errmsg, "Model::Model() E %d %d %d", pointlist.size(), texlist.size(), normallist.size());
	get_errors(errmsg);
	
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors("Model::Model() F");
	
	angle = 0.0f;
	
	if( name.compare("bulb1") == 0){
		glm::vec4 pos = model * glm::vec4(0.0f,0.0f,0.0f,1.0f);
		m_lighting->addLight(glm::vec3(pos[0], pos[1], pos[2]), glm::vec4(1.0f,1.0f,1.0f,1.0f));
	}
	if( name.compare("bulb2") == 0){
		glm::vec4 pos = model * glm::vec4(0.0f,0.0f,0.0f,1.0f);
		m_lighting->addLight(glm::vec3(pos[0], pos[1], pos[2]), glm::vec4(1.0f,1.0f,1.0f,1.0f));
	}

	my_glGenVertexArrays(1, &simple_vao_id);
	my_glBindVertexArray(simple_vao_id);
	tmp_vbo_id = GenerateVBO(1);
	simple_vbo_id = *tmp_vbo_id;
	bindVBO(simple_vbo_id, &pointlist[0], pointlist.size() * 4 * sizeof(GLfloat));
	GLint vertex_location = simpleShader->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	sprintf(errmsg, "Model::bindSimpleVertex() A %d", vertex_location);
	get_errors(errmsg);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors("Model::Model() G");
}

Model::Model()
{
	this->vbo_id[0] = this->vbo_id[1] = this->vbo_id[2] = 0;
	this->vao_id = 0;
	//this->shader = 0;
	
	angle = 0.0f;
}

void Model::print()
{
	for(unsigned i=0; i<pointlist.size(); i++){
		printf("%f %f %f, %f %f %f\n", pointlist[i].x, pointlist[i].y, pointlist[i].z, normallist[i].x, normallist[i].y, normallist[i].z);
	}// 	for(unsigned i=0; i<4; i++){
// 		for(unsigned j=0; j<4; j++){
// 		  printf(" %f", model[i][j]);
// 		}
// 		printf("\n");
// 	}
	for(unsigned i=0; i<indexlist.size(); i++){
		printf("%d ", indexlist[i]);
	}
	printf("\n");
	
	printf("Vertices: %d, Indices: %d\n", pointlist.size(), indexlist.size());
}

void Model::draw()
{
	if(indexlist.empty())
		return;
	
	if(	name.compare("bulb1")==0)
		return;
	
	if(	name.compare("bulb2")==0)
		return;
	
	Shader *shader = effect->getShader();
	get_errors("Model::draw() A");
	shader->bind();
	get_errors("Model::draw() B");
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors("Model::draw() C");
	if(effect->hasTexture()){ 
// 		printf("effect->hasTexture() %s\n", name.c_str());
		GLint texture_uniform = shader->get_uniform_location("colorMap");
		get_errors("Model::draw()::hasTexture A");
		glUniform1i(texture_uniform, 0); //soll erste textureinheit verwenden
		get_errors("Model::draw()::hasTexture B");
		glActiveTexture(GL_TEXTURE0);
		get_errors("Model::draw()::hasTexture C");
		glBindTexture(GL_TEXTURE_2D, effect->getTexture()->getTexId());
		get_errors("Model::draw()::hasTexture D");
	}
	if(effect->hasBumpMap()){
		GLint bumpmap_uniform = shader->get_uniform_location("bumpMap");
		get_errors("Model::draw()::hasBumpMap A");
		glUniform1i(bumpmap_uniform, 10); //soll 10. textureinheit verwenden
		get_errors("Model::draw()::hasBumpMap B");
		glActiveTexture(GL_TEXTURE10);
		get_errors("Model::draw()::hasBumpMap C");
		glBindTexture(GL_TEXTURE_2D, effect->getBumpMap()->getTexId());
		get_errors("Model::draw()::hasBumpMap D");
		GLint inv_trans_model_uniform = shader->get_uniform_location( "invTransModel");
		glUniformMatrix4fv(inv_trans_model_uniform, 1, GL_FALSE, glm::value_ptr(glm::inverse(model)));
		//GLint specular_uniform = shader->get_uniform_location( "specular");
		//glUniform1f(specular_uniform,effect->specular);
		//GLint powspecular_uniform = shader->get_uniform_location( "powspecular");
		//glUniform1i(powspecular_uniform,effect->powspecular);
		
		GLint ks_uniform = shader->get_uniform_location("ks");
		glUniform1f(ks_uniform, effect->specular);
// 		if( name.compare("floor") == 0){
// 			glUniform1f(ks_uniform, 0.0f);
// 		}else if(name.compare("walls") == 0 ||
// 				name.compare("shaft") == 0 ||
// 				name.compare("socket") == 0){
// 			glUniform1f(ks_uniform, 0.0f);
// 		}else if(name.compare("engine") == 0){
// 			glUniform1f(ks_uniform, 0.2f);
// 		}else{
// 			glUniform1f(ks_uniform, 0.1f);
// 		}
	}
	get_errors("Model::draw() D");

    // set matrix-uniforms
    GLint perspective_uniform = shader->get_uniform_location( "perspective");
    GLint view_uniform        = shader->get_uniform_location( "view");
    GLint model_uniform       = shader->get_uniform_location( "model");
	
	get_errors("Model::draw() E");
// 	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
// 	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));
	get_errors("Model::draw() F");
	glDrawElements(GL_TRIANGLES, indexlist.size(), GL_UNSIGNED_INT, &indexlist[0]);
	get_errors("Model::draw() G");
	my_glBindVertexArray(0);
	get_errors("Model::draw() H");
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors("Model::draw() I");
	shader->unbind();
	
	drawChildren();
}

void Model::drawSimple(){ // for deph map / shadow map
	// TODO should be much simpler (no lighting / shading at all)
	// 
	if(indexlist.empty())
		return;
	
	if(	name.compare("bulb1")==0 ||
		name.compare("wire1")==0)
		return;
	
	if(	name.compare("bulb2")==0 ||
		name.compare("wire2")==0)
		return;
	
	get_errors();
	
	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(simple_vao_id);
	get_errors();
	simpleShader->bind();
	get_errors();

    // set matrix-uniforms
    GLint perspective_uniform = simpleShader->get_uniform_location( "perspective");
    GLint view_uniform        = simpleShader->get_uniform_location( "view");
    GLint model_uniform       = simpleShader->get_uniform_location( "model");
	
	glUniformMatrix4fv(perspective_uniform, 1, GL_FALSE, glm::value_ptr(m_camera_1.intrinsic));
	glUniformMatrix4fv(view_uniform,        1, GL_FALSE, glm::value_ptr(m_camera_1.extrinsic));
	glUniformMatrix4fv(model_uniform,       1, GL_FALSE, glm::value_ptr(model));
	get_errors();
	glDrawElements(GL_TRIANGLES, indexlist.size(), GL_UNSIGNED_INT, &indexlist[0]);
	get_errors();
	simpleShader->unbind();
	my_glBindVertexArray(0);
	get_errors();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors();
	
	
	drawSimpleChildren();
}

void Model::update(float fTime)
{
	if(animation){
		float scale = 200.0f;
		angle = (angle + fTime * scale);
		if(angle > 360.0f)
			angle = angle - 360.0f;
		
		if( name.compare("flywheel") == 0 ||
			name.compare("crank") == 0){
			glm::mat4 rot = glm::rotate(-angle, 0.0f, 0.0f, 1.0f);
			model = model_orig * rot;
		}
		if( name.compare("shaft") == 0 ||
			name.compare("hammer") == 0){
			float start = 40.0f;
			float top = 135.0f;
			float fall = 200.0f;
			glm::mat4 rot;
			float alpha=0.0f;
			if(angle > 0.0f && angle < start){
				;
			}else if(angle > start && angle <= top){
				alpha = 0.3f*(angle-start);
				rot = glm::rotate(alpha, 0.0f, 1.0f, 0.0f);
			}else if(angle > top && angle <= fall){
				alpha = 0.3f*(top-start)-2.0f*(angle-top);
				if(alpha > 0.0f)
					rot = glm::rotate(alpha, 0.0f, 1.0f, 0.0f);
			}else if(angle > fall && angle <= 360.0f){
				;
			}
			model = model_orig * rot;
		}
		if( name.compare("piston") == 0 ){
			glm::mat4 rot;
			rot = glm::translate(0.0f, 0.0f, 0.7f * (float)sin(angle*PI/180.0f - PI*0.5f));
			model = model_orig * rot;
		}
		if( name.compare("bolt2") == 0 ){
			glm::mat4 rot;
			rot = glm::translate(0.7f * (float)sin(angle*PI/180.0f - PI*0.5f),0.0f,0.0f);
			model = model_orig * rot;
		}
		if( name.compare("connection") == 0 ){
			glm::mat4 rot;
			rot = glm::translate(0.7f * (float)sin(angle*PI/180.0f - PI*0.5f),0.0f,0.0f);
			rot = rot * glm::rotate((float)sin(angle*PI/180.0f)*15.0f,0.0f,1.0f,0.0f);
			model = model_orig * rot;
		}
		if( name.compare("piston2") == 0 ){
			glm::mat4 rot;
			rot = glm::translate(0.0f, 0.0f, 0.7f * (float)sin(angle*PI/180.0f + PI*0.5f));
			model = model_orig * rot;
		}
		if( name.compare("bolt3") == 0 ){
			glm::mat4 rot;
			rot = glm::translate(0.7f * (float)sin(angle*PI/180.0f + PI*0.5f),0.0f,0.0f);
			model = model_orig * rot;
		}
		if( name.compare("rod") == 0 ){
			glm::mat4 rot;
			rot = glm::translate(0.7f * (float)sin(angle*PI/180.0f + PI*0.5f),0.0f,0.0f);
			rot = rot * glm::rotate((float)sin(angle*PI/180.0f + PI)*13.5f,0.0f,1.0f,0.0f);
			model = model_orig * rot;
		}

		if( name.compare("regulator") == 0 ){
			glm::mat4 rot;
			rot = glm::rotate(4.0f*angle,0.0f,0.0f,1.0f);
			model = model_orig * rot;
		}
		
// 		if( name.compare("bulb1") == 0 ||
// 			name.compare("wire1") == 0){
// 			glm::mat4 rot;
// 			rot = glm::rotate(2.0f*(float)sin(angle*PI/180.0f),1.0f,0.0f,0.0f);
// 			rot = rot * glm::rotate(2.0f*(float)cos(angle*PI/180.0f),0.0f,1.0f,0.0f);
// 			model = model_orig * rot;
// 			if(name.compare("bulb1") == 0){
// 				glm::vec4 pos = model * glm::vec4(.0f,.0f,-3.0f,1.0f);
// 				m_lighting->changePosition(0,glm::vec3(pos[0], pos[1], pos[2]));
// 			}
// 		}
// 		if( name.compare("bulb2") == 0){
// 			glm::vec4 pos = model * glm::vec4(.0f,.0f,0.0f,1.0f);
// 			m_lighting->changePosition(1,glm::vec3(pos[0], pos[1], pos[2]));
// 		}
		
		updateChildren(fTime);
	}
}
void Model::bindVertex(void* data, size_t size){
	bindVBO(vbo_id[0], data, size);
	GLint vertex_location = effect->getShader()->get_attrib_location("vertex");
	glEnableVertexAttribArray(vertex_location);
	sprintf(errmsg, "Model::bindVertex() A %d", vertex_location);
	get_errors(errmsg);
	glVertexAttribPointer(	vertex_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}

void Model::bindNormals(void* data, size_t size){
	bindVBO(vbo_id[2], data, size);
	GLint normal_location = effect->getShader()->get_attrib_location("normal");
	glEnableVertexAttribArray(normal_location);
	sprintf(errmsg, "Model::bindNormals() A %d", normal_location);
	get_errors(errmsg);
	glVertexAttribPointer(	normal_location, 3, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}

void Model::bindColor(void* data, size_t size){
	bindVBO(vbo_id[1], data, size);
	GLint color_location = effect->getShader()->get_attrib_location("color");
	glEnableVertexAttribArray(color_location);
	sprintf(errmsg, "Model::bindColor() A %d", color_location);
	get_errors(errmsg);
	glVertexAttribPointer(	color_location, 4, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}
void Model::bindTexture(void* data, size_t size){
	bindVBO(vbo_id[1], data, size);
	GLint tex_location = effect->getShader()->get_attrib_location("texkoord");
	glEnableVertexAttribArray(tex_location);
	sprintf(errmsg, "Model::bindTexture() A %d", tex_location);
	get_errors(errmsg);
	glVertexAttribPointer(	tex_location, 2, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}

void Model::bindInvBinormal(void* data, size_t size){
	bindVBO(vbo_bump_id[0], data, size);
	GLint InvBinormal_location = effect->getShader()->get_attrib_location("InvBinormal");
	glEnableVertexAttribArray(InvBinormal_location);
	sprintf(errmsg, "Model::bindInvBinormal() A %d", InvBinormal_location);
	get_errors(errmsg);
	glVertexAttribPointer(	InvBinormal_location, 3, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}
void Model::bindInvNormal(void* data, size_t size){
	bindVBO(vbo_bump_id[1], data, size);
	GLint Normal_location = effect->getShader()->get_attrib_location("InvNormal");
	glEnableVertexAttribArray(Normal_location);
	sprintf(errmsg, "Model::bindInvNormal() A %d", Normal_location);
	get_errors(errmsg);
	glVertexAttribPointer(	Normal_location, 3, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}
void Model::bindInvTangent(void* data, size_t size){
	bindVBO(vbo_bump_id[2], data, size);
	GLint InvTangent_location = effect->getShader()->get_attrib_location("InvTangent");
	glEnableVertexAttribArray(InvTangent_location);
	sprintf(errmsg, "Model::bindInvTangent() A %d", InvTangent_location);
	get_errors(errmsg);
	glVertexAttribPointer(	InvTangent_location, 3, GL_FLOAT, 
							GL_FALSE, 0, NULL);
}
/*void Model::assignTextureId(GLuint texid)
{
	texidlist.clear(); // TODO very hacky ... will be removed when integrating prober multitexturing
	this->texidlist.push_back(texid);
}
void Model::assignBumpMapId(GLuint texid)
{
	this->texidlist.push_back(texid);
}*/
Model::~Model()
{
	//std::vector<SceneObject*>::iterator it = children.begin();
	
	//while(it != children.end()){
	//	delete(*it);
	//	it++;
	//}
	if(vao_id){
		// TODO this two commands cause Seg. faults in Linux
		// although the vao_id was generated and does still exist (tm)
// 		if(glIsVertexArrays(vao_id)){
// 			glDeleteVertexArrays(1, &vao_id);
// 		}
		vao_id = 0;
	}

	if(vbo_id[0] && vbo_id[1] && vbo_id[2]){
		glDeleteBuffers(3, vbo_id);
		vbo_id[0] = vbo_id[1] = vbo_id[2] = 0;
	}
}

void Model::initBumpMap()
{
	glm::vec3 *InvNormals = new glm::vec3[pointlist.size()];
	glm::vec3 *InvBinormals = new glm::vec3[pointlist.size()];
	glm::vec3 *InvTangents = new glm::vec3[pointlist.size()];


	for(unsigned i = 0; (i*3 + 2) < indexlist.size(); i++){
		glm::vec3 Vertices[3];
		glm::vec2 TexCoords[3];
		glm::vec3 Normals[3];
		glm::vec3 InvNormals_tmp[3];
		glm::vec3 InvBinormals_tmp[3];
		glm::vec3 InvTangents_tmp[3];

		Vertices[0] = (glm::vec3)pointlist[indexlist[i*3+0]];
		Vertices[1] = (glm::vec3)pointlist[indexlist[i*3+1]];
		Vertices[2] = (glm::vec3)pointlist[indexlist[i*3+2]];

		TexCoords[0] = texlist[indexlist[i*3+0]];
		TexCoords[1] = texlist[indexlist[i*3+1]];
		TexCoords[2] = texlist[indexlist[i*3+2]];
		
		Normals[0] = normallist[indexlist[i*3+0]];
		Normals[1] = normallist[indexlist[i*3+1]];
		Normals[2] = normallist[indexlist[i*3+2]];

		glm::vec3 InvNormal;
		glm::vec3 InvBinormal;
		glm::vec3 InvTangent;
		FindInvTBN(Vertices,TexCoords,Normals, InvNormals_tmp,InvBinormals_tmp,InvTangents_tmp);

		for(int j = 0; j < 3; j++){
			InvNormals[indexlist[i*3+j]] = InvNormals_tmp[j];
			InvBinormals[indexlist[i*3+j]] = InvBinormals_tmp[j];
			InvTangents[indexlist[i*3+j]] = InvTangents_tmp[j];
		}
	}
	for(unsigned i = 0; i < pointlist.size(); i++){
		this->InvNormals.push_back(InvNormals[i]);
		this->InvBinormals.push_back(InvBinormals[i]);
		this->InvTangents.push_back(InvTangents[i]);
	}
	delete InvNormals;
	delete InvBinormals;
	delete InvTangents;

	PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
	my_glBindVertexArray(vao_id);
	get_errors();
	GLuint* tmp_vbo_id = GenerateVBO(3);
	vbo_bump_id[0] = tmp_vbo_id[0];
	vbo_bump_id[1] = tmp_vbo_id[1];
	vbo_bump_id[2] = tmp_vbo_id[2];
	get_errors("Model::initBumpMap() A");
	bindInvNormal(&this->InvNormals[0], this->InvNormals.size() * 3 * sizeof(GLfloat));
	get_errors("Model::initBumpMap() B");
	bindInvBinormal(&this->InvBinormals[0], this->InvBinormals.size() * 3 * sizeof(GLfloat));
	get_errors("Model::initBumpMap() C");
	bindInvTangent(&this->InvTangents[0], this->InvTangents.size() * 3 * sizeof(GLfloat));
	get_errors("Model::initBumpMap() D");
	my_glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	get_errors("Model::initBumpMap() E");


}

void Model::FindInvTBN(glm::vec3 Vertices[3], glm::vec2 TexCoords[3], glm::vec3 Normals[3], 
					   glm::vec3 InvNormals[3], glm::vec3 InvBinormals[3], glm::vec3 InvTangents[3])
{
	/* Calculate the vectors from the current vertex
        to the two other vertices in the triangle */
	
    glm::vec3 v2v1 = Vertices[1] - Vertices[0];
    glm::vec3 v3v1 = Vertices[2] - Vertices[0];
 
    //Calculate the “direction” of the triangle based on texture coordinates.
 
    // Calculate c2c1_T and c2c1_B
    float c2c1_T = TexCoords[1].x - TexCoords[0].x;
    float c2c1_B = TexCoords[1].y - TexCoords[0].y;
 
    // Calculate c3c1_T and c3c1_B
    float c3c1_T = TexCoords[2].x - TexCoords[0].x;
    float c3c1_B = TexCoords[2].y - TexCoords[0].y;
 
    //Look at the references for more explanation for this one.
    float fDenominator = c2c1_T * c3c1_B - c3c1_T * c2c1_B;  
 
    if ( fDenominator < 0.00001 && fDenominator > -0.00001) 
    {
            /* We won't risk a divide by zero, so set the tangent matrix to the
                identity matrix */
			InvTangents[0] = glm::vec3(1.0f, 0.0f, 0.0f);
			InvBinormals[0] = glm::vec3(0.0f, 1.0f, 0.0f);
			InvNormals[0] = glm::vec3(0.0f, 0.0f, 1.0f);
			InvTangents[1] = glm::vec3(1.0f, 0.0f, 0.0f);
			InvBinormals[1] = glm::vec3(0.0f, 1.0f, 0.0f);
			InvNormals[1] = glm::vec3(0.0f, 0.0f, 1.0f);
			InvTangents[2] = glm::vec3(1.0f, 0.0f, 0.0f);
			InvBinormals[2] = glm::vec3(0.0f, 1.0f, 0.0f);
			InvNormals[2] = glm::vec3(0.0f, 0.0f, 1.0f);
    }
    else
    {            
            // Calculate the reciprocal value once and for all (to achieve speed)
            float fScale1 = 1.0f / fDenominator;
 
            /* Time to calculate the tangent, binormal, and normal.
                Look at Søren’s article for more information. */
            glm::vec3 T, B, N;
            T = glm::vec3((c3c1_B * v2v1.x - c2c1_B * v3v1.x) * fScale1,
                            (c3c1_B * v2v1.y - c2c1_B * v3v1.y) * fScale1,
                            (c3c1_B * v2v1.z - c2c1_B * v3v1.z) * fScale1);

 
//             B = glm::vec3((-c3c1_T * v2v1.x + c2c1_T * v3v1.x) * fScale1,
//                             (-c3c1_T * v2v1.y + c2c1_T * v3v1.y) * fScale1,
//                             (-c3c1_T * v2v1.z + c2c1_T * v3v1.z) * fScale1);
 
            //N = T%B; //Cross product!
// 			N = glm::cross(T,B);
/*This is where programmers should break up the function to smooth the tangent, binormal and
normal values. */

			T = glm::normalize(T);
			N = -glm::normalize(Normals[0]);
			B = glm::cross(N,T);
			T = glm::cross(B,N);
			InvTrans(T,B,N,InvTangents[0],InvBinormals[0],InvNormals[0]);

			N = -glm::normalize(Normals[1]);
			B = glm::cross(N,T);
			T = glm::cross(B,N);
			InvTrans(T,B,N,InvTangents[1],InvBinormals[1],InvNormals[1]);

			N = -glm::normalize(Normals[2]);
			B = glm::cross(N,T);
			T = glm::cross(B,N);
			InvTrans(T,B,N,InvTangents[2],InvBinormals[2],InvNormals[2]);
	}
}

void Model::InvTrans(glm::vec3 T, glm::vec3 B, glm::vec3 N,
					 glm::vec3 &InvTangent, glm::vec3 &InvBinormal, glm::vec3 &InvNormal)
{

	glm::mat3 tbn = glm::mat3(T,B,N);
	tbn = glm::inverse(tbn);
	InvTangent.x = tbn[0][0];
	InvTangent.y = tbn[0][1];
	InvTangent.z = tbn[0][2];
	InvBinormal.x = tbn[1][0];
	InvBinormal.y = tbn[1][1];
	InvBinormal.z = tbn[1][2];
	InvNormal.x = tbn[2][0];
	InvNormal.y = tbn[2][1];
	InvNormal.z = tbn[2][2];
// 	InvTangent.x = tbn[0][0];
// 	InvTangent.y = tbn[1][0];
// 	InvTangent.z = tbn[2][0];
// 	InvBinormal.x = tbn[0][1];
// 	InvBinormal.y = tbn[1][1];
// 	InvBinormal.z = tbn[2][1];
// 	InvNormal.x = tbn[0][2];
// 	InvNormal.y = tbn[1][2];
// 	InvNormal.z = tbn[2][2];
	return;

	//Look at “Derivation of the Tangent Space Matrix” for more information.

	float fScale2 = 1.0f / ((T.x * B.y * N.z - T.z * B.y * N.x) + 
							(B.x * N.y * T.z - B.z * N.y * T.x) + 
							(N.x * T.y * B.z - N.z * T.y * B.x));

	/*InvTangent.set((B%N).x() * fScale2,
				((-1.0f * N)%T).x() * fScale2,
				(T%B).x() * fScale2);*/

	InvTangent.x = glm::cross(B,N).x *  fScale2;
	InvTangent.y = glm::cross((-1.0f * N),T).x * fScale2;
	InvTangent.z = glm::cross(T,B).x * fScale2;
	InvTangent = glm::normalize(InvTangent);

	/*InvBinormal.set(((-1.0f *B)%N).y() * fScale2,
				(N%T).y() * fScale2,
				((-1.0f * T)%B).y() * fScale2);*/
	InvBinormal.x = glm::cross((-1.0f *B),N).y * fScale2;
	InvBinormal.y = glm::cross(N,T).y * fScale2;
	InvBinormal.z = glm::cross((-1.0f * T),B).y * fScale2;
	InvBinormal = glm::normalize(InvBinormal);

	InvNormal.x = glm::cross(B,N).z * fScale2;
	InvNormal.y = glm::cross((-1.0f * N),T).z * fScale2;
	InvNormal.z = glm::cross(T,B).z * fScale2;
	/*InvNormal.set((B%N).z() * fScale2,
				((-1.0f * N)%T).z() * fScale2,
				(T%B).z() * fScale2);*/	
	InvNormal = glm::normalize(InvNormal);
}