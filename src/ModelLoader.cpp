#include "ModelLoader.h"
#include <dae.h>
#include <dom.h>
#include <dom/domCOLLADA.h>
#include <dae/daeElement.h>
#include <dom/domVisual_scene.h>
#include <dom/domConstants.h>

#include <glm/glm.hpp>
#include <glm/gtx/comparison.hpp>

#include "common.hpp"
#include "Model.h"
#include "SceneObject.h"

using namespace echtzeitlu;

extern SceneObject* rootScene;

ModelLoader::ModelLoader(const std::string path)
{
	DAE *dae = new DAE;	// TODO delete
	domCOLLADA* domRoot = dae->open(path);
	domCOLLADA::domSceneRef domScene = domRoot->getScene();
	defaultScene = NULL;
	if (domScene)
		if (domScene->getInstance_visual_scene())
			if (domScene->getInstance_visual_scene())
				defaultScene = domScene->getInstance_visual_scene()->getUrl().getElement();
	
	//int geometrieCount = root->getDatabase()->getElementCount(NULL, COLLADA_TYPE_IMAGE, NULL);

}


ModelLoader::~ModelLoader(void)
{
}

void ModelLoader::travers(domNode *node)
{
	for(unsigned int i = 0; i < node->getNode_array().getCount(); i++)
	{
		domNode * currNode = node->getNode_array()[i];
		daeElement * daeElem = (daeElement *) currNode;
		if(daeElem->getElementType()== COLLADA_TYPE::NODE)
			travers(currNode);
	}
	
	for (int i = 0; i < node->getInstance_geometry_array().getCount(); i++)
	{
		//Suche geometrie im dokument
		domInstance_geometryRef lib = node->getInstance_geometry_array()[i];
		xsAnyURI & urltype  = lib->getUrl();
		urltype.resolveElement();
		domElement * element = (domElement* ) urltype.getElement();
		if (element==NULL) // this instance geometry is not found skip to the next one
			continue;
		domGeometry * geo = (domGeometry *)element;
		domMesh *meshElement = geo->getMesh();
		if(meshElement == NULL)// kein mesh andere geometrie information
			continue;
		//not sure if we should get primitives by groups or by whatever comes first, I think it shouldn't matter, let's confirm later.
		unsigned int numPolylistGroups = meshElement->getPolylist_array().getCount();
		for (unsigned int i=0; i< numPolylistGroups; i++)
		{
			
		}

		unsigned int numPolygonGroups = meshElement->getPolygons_array().getCount();
		for (unsigned int i=0; i< numPolygonGroups; i++)
		{
			
		}

		unsigned int numTriangleGroups = meshElement->getTriangles_array().getCount();
		for (unsigned int i=0; i< numTriangleGroups; i++)
		{
			domTriangles *dom_triangles = meshElement->getTriangles_array()[i];
			domInputLocalOffset_Array & inputs = dom_triangles->getInput_array();
			domListOfUInts P = dom_triangles->getP()->getValue();
			unsigned int max_offset = 0;
			unsigned position_offset, normal_offset, texture1_offset;
			domListOfFloats * position_floats;
			domListOfFloats * normal_floats;
			domListOfFloats * texture1_floats;
			domListOfUInts * position_indices;

			for(unsigned int i=0; i<inputs.getCount(); i++)
			{
				unsigned int thisoffset  = inputs[i]->getOffset();
				if (max_offset < thisoffset) max_offset++;

				domSource * source = (domSource*) (domElement*) inputs[i]->getSource().getElement();
				if(strcmp("VERTEX", inputs[i]->getSemantic()) == 0) {
					position_offset = thisoffset;
					//position_floats = & source->getFloat_array()->getValue();
				} else if(strcmp("NORMAL", inputs[i]->getSemantic()) == 0) {
					normal_offset = thisoffset;
					normal_floats = & source->getFloat_array()->getValue();
				} else if(strcmp("TEXCOORD", inputs[i]->getSemantic()) == 0) {
					texture1_offset = thisoffset;
					texture1_floats = & source->getFloat_array()->getValue();
				}
			}
			max_offset++;

			// inputs without offsets in vertices
			domMesh * mesh = (domMesh*) inputs[0]->getParentElement()->getParentElement();
			domVertices * vertices = mesh->getVertices();
			domInputLocal_Array& vertices_inputs = vertices->getInput_array();
			for(unsigned int i=0; i<vertices_inputs.getCount(); i++)
			{
				domSource * source = (domSource*) (domElement*) vertices_inputs[i]->getSource().getElement();
				if(strcmp("POSITION", vertices_inputs[i]->getSemantic()) == 0) {
					position_floats = & source->getFloat_array()->getValue();
				} else if(strcmp("NORMAL", vertices_inputs[i]->getSemantic()) == 0) {
					normal_floats = & source->getFloat_array()->getValue();
					normal_offset = position_offset;
				} else if(strcmp("TEXCOORD", vertices_inputs[i]->getSemantic()) == 0) {
					texture1_floats = & source->getFloat_array()->getValue();
					texture1_offset = position_offset;
				}
			}
			unsigned int numPoints = position_floats->getCount()/3;
			glm::vec4 * points = new glm::vec4[numPoints];	// TODO delete
			for(unsigned int i=0;i< numPoints;i++){
				points[i].x = position_floats->get(i*3+0);
				points[i].y = position_floats->get(i*3+1);
				points[i].z = position_floats->get(i*3+2);
				points[i].w = 1.f;
			}
			int * pointIndices;
			pointIndices = new int[dom_triangles->getCount() * 3];	// TODO delete
			for(unsigned int i=0;i < dom_triangles->getCount() * 3;i++){
				pointIndices[i] = P[i*max_offset + position_offset];
			}

			unsigned int numNormals = normal_floats->getCount()/3;
			glm::vec3 * normals = new glm::vec3[numNormals];	// TODO delete
			for(unsigned int i=0;i< numNormals;i++){
					normals[i].x = normal_floats->get(i*3+0);
					normals[i].y = normal_floats->get(i*3+1);
					normals[i].z = normal_floats->get(i*3+2);
			}
			int * normalIndices = new int[dom_triangles->getCount() * 3];	// TODO delete
			for(unsigned int i=0;i < dom_triangles->getCount() * 3;i++){
					normalIndices[i] = P[i*max_offset + normal_offset];
			}
			
			//
			printf("re-arranging vertexlist\n");
			std::vector<glm::vec4> pointlist;
			std::vector<glm::vec3> normallist;
			std::vector<unsigned> indexlist;
			for(unsigned i=0; i<dom_triangles->getCount()*3; i++){
				unsigned pI = pointIndices[i];
				unsigned nI = normalIndices[i];
				
				if(pointlist.empty()){
					pointlist.push_back(points[pI]);
					normallist.push_back(normals[nI]);
					indexlist.push_back(pointlist.size()-1);
				}else{
					bool is_in = false; 
					unsigned j;
					for(j=0; j<pointlist.size(); j++){
						if( points[pI] == pointlist[j] && normals[nI] == normallist[j] ){
							is_in = true;
							break;
						}
					}
					if(is_in){
						indexlist.push_back(j);
					}else{
						pointlist.push_back(points[pI]);
						normallist.push_back(normals[nI]);
						indexlist.push_back(pointlist.size()-1);
					}
				}
			}
			
			printf("%d %d %d\n", pointlist.size(), normallist.size(), indexlist.size());
			
			

			// TODO causes segmentation fault when no texture used
// 				unsigned int numTexture = texture1_floats->getCount()/2;
// 				glm::vec2 * texture = new glm::vec2[numTexture];
// 				for(unsigned int i=0;i< numTexture;i++){
// 						texture[i].x = texture1_floats->get(i*2+0);
// 						texture[i].y = texture1_floats->get(i*2+1);
// 				}
// 				int * textureIndices = new int[dom_triangles->getCount() * 3];
// 				for(unsigned int i=0;i < dom_triangles->getCount() * 3;i++){
// 						textureIndices[i] = P[i*max_offset + texture1_offset];
// 				}

			//VBO's erstellen
			GLuint *vbo_id;
			GLuint vao_id;
			PFNGLGENVERTEXARRAYSPROC my_glGenVertexArrays = (PFNGLGENVERTEXARRAYSPROC)glfwGetProcAddress("glGenVertexArrays");
			my_glGenVertexArrays(1, &vao_id);
			PFNGLBINDVERTEXARRAYPROC my_glBindVertexArray = (PFNGLBINDVERTEXARRAYPROC)glfwGetProcAddress("glBindVertexArray");
			my_glBindVertexArray(vao_id);

			//komischer vector error, reihnfolge bind und shader binds müssen noch angepasst werden (verlagerung in model klasse)
			vbo_id = GenerateVBO(3);
			Model *model=new Model(vbo_id,vao_id,&indexlist[0],indexlist.size());
			model->bindVertex(&pointlist[0],pointlist.size() * 4 * sizeof(GLfloat));
			model->bindNormals(&normallist[0], normallist.size() * 3 * sizeof(GLfloat));

			std::vector<glm::vec4> colorlist;
			for(size_t i = 0; i < indexlist.size();i++){
				colorlist.push_back(glm::vec4(1, 0, 0.5, 1));
			}
			model->bindColor(&colorlist[0], colorlist.size() * 4 * sizeof(GLfloat));

			my_glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);


			rootScene->add((SceneObject*)model);
			
			

		}

		unsigned int numTriStripsGroups = meshElement->getTristrips_array().getCount();
		for (unsigned int i=0; i< numTriStripsGroups ; i++)
		{
			
		}

		unsigned int numTriFansGroups = meshElement->getTrifans_array().getCount();
		for (unsigned int i=0; i< numTriFansGroups ; i++)
		{
			
		}

		unsigned int numLinesGroups = meshElement->getLines_array().getCount();
		for (unsigned int i=0; i< numLinesGroups ; i++)
		{
			
		}

		unsigned int numLineStripsGroups = meshElement->getLinestrips_array().getCount();
		for (unsigned int i=0; i< numLineStripsGroups ; i++)
		{
			
		}
		for (unsigned int i=0; i< 0; i++)
		{

		}
	}
}


void ModelLoader::ReadScene(domVisual_scene *scene)
{
	for ( int i = 0; i < scene->getNode_array().getCount(); i++){
		domNode* currNode = scene->getNode_array()[i];
		travers(currNode);
	}
}

void ModelLoader::loadScene()
{
	if(defaultScene)
		ReadScene( (domVisual_scene *)defaultScene );
}