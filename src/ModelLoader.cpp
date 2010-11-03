#include "ModelLoader.h"
#include <dae.h>
#include <dom.h>
#include <dom/domCOLLADA.h>
#include <dae/daeElement.h>
#include <dom/domVisual_scene.h>
#include <dom/domConstants.h>

#include <glm/glm.hpp>

#include "common.hpp"


ModelLoader::ModelLoader(const std::string path)
{
	DAE *dae = new DAE;
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

void travers(domNode *node)
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
				glm::vec3 * points = new glm::vec3[numPoints];
				for(unsigned int i=0;i< numPoints;i++){
					points[i].x = position_floats->get(i*3+0);
					points[i].y = position_floats->get(i*3+1);
					points[i].z = position_floats->get(i*3+2);
				}
				int * pointIndices;
				pointIndices = new int[dom_triangles->getCount() * 3];
				for(unsigned int i=0;i < dom_triangles->getCount() * 3;i++){
					pointIndices[i] = P[i*max_offset + position_offset];
				}

				unsigned int numNormals = normal_floats->getCount()/3;
				glm::vec3 * normals = new glm::vec3[numNormals];
				for(unsigned int i=0;i< numNormals;i++){
						normals[i].x = normal_floats->get(i*3+0);
						normals[i].y = normal_floats->get(i*3+1);
						normals[i].z = normal_floats->get(i*3+2);
				}
				int * normalIndices = new int[dom_triangles->getCount() * 3];
				for(unsigned int i=0;i < dom_triangles->getCount() * 3;i++){
						normalIndices[i] = P[i*max_offset + normal_offset];
				}

				unsigned int numTexture = texture1_floats->getCount()/2;
				glm::vec2 * texture = new glm::vec2[numTexture];
				for(unsigned int i=0;i< numTexture;i++){
						texture[i].x = texture1_floats->get(i*2+0);
						texture[i].y = texture1_floats->get(i*2+1);
				}
				int * textureIndices = new int[dom_triangles->getCount() * 3];
				for(unsigned int i=0;i < dom_triangles->getCount() * 3;i++){
						textureIndices[i] = P[i*max_offset + texture1_offset];
				}

				//VBO's erstellen

				int pointsVBO = GenerateVBO(1);

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