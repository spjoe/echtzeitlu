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
#include "shader.hpp"
#include "Timer.h"

using namespace echtzeitlu;

extern Shader* defaultShader;		// TODO der hat hier nix verloren!

ModelLoader::ModelLoader()
{

}


ModelLoader::~ModelLoader(void)
{
}

void ModelLoader::travers(domNode *node, SceneObject* sceneObject)
{
	for(size_t i = 0; i < node->getNode_array().getCount(); i++)
	{
		domNode * currNode = node->getNode_array()[i];
		daeElement * daeElem = (daeElement *) currNode;
		if(daeElem->getElementType()== COLLADA_TYPE::NODE)
			travers(currNode, sceneObject);
	}
	
	for (size_t i = 0; i < node->getInstance_geometry_array().getCount(); i++)
	{
		SceneObject* model = NULL;
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
				domUint thisoffset  = inputs[i]->getOffset();
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
			std::multimap<unsigned,std::pair<unsigned,unsigned>> pointtonormalmap;

			for(unsigned i=0; i<dom_triangles->getCount()*3; i++){
				unsigned pI = pointIndices[i];
				unsigned nI = normalIndices[i];
				
				if(pointlist.empty()){
					pointlist.push_back(points[pI]);
					pointtonormalmap.insert(std::pair<unsigned,std::pair<unsigned,unsigned>>(pI,std::pair<unsigned,unsigned>(nI,0)));
					normallist.push_back(normals[nI]);
					indexlist.push_back(pointlist.size()-1);
				}else{
					bool is_in = false; 
					if(pointtonormalmap.count(pI)){
						std::pair<std::multimap<unsigned, std::pair<unsigned,unsigned>>::iterator,std::multimap<unsigned, std::pair<unsigned,unsigned>>::iterator> pairnIs = pointtonormalmap.equal_range( pI );
						std::multimap<unsigned, std::pair<unsigned,unsigned>>::iterator nIs = pairnIs.first;
						for(; nIs != pairnIs.second ;nIs++)
						{
							if((*nIs).second.first == nI){ //element schon vorhanden
								indexlist.push_back((*nIs).second.second);
								is_in = true;
								break;
							}
						}
						
					}
					
					if(is_in == false){
						pointlist.push_back(points[pI]);
						normallist.push_back(normals[nI]);
						indexlist.push_back(pointlist.size()-1);
						pointtonormalmap.insert(std::pair<unsigned,std::pair<unsigned,unsigned>>(pI,std::pair<unsigned,unsigned>(nI,pointlist.size()-1)));
					}
					/*
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
						pointmap.insert(std::pair<glm::vec4,unsigned>(normals[nI],pointlist.size()-1));
						normalmap.insert(std::pair<glm::vec4,unsigned>(normals[nI],pointlist.size()-1));
					}
					*/
				}
			}
			
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
			if(model == NULL){
				model = new Model(pointlist, normallist, indexlist, defaultShader);
				sceneObject->add( (SceneObject*)model );
			}else{
				SceneObject *submodel = new Model(pointlist, normallist, indexlist, defaultShader);
				model->add(submodel);
			}
			printf("[ModelLoader::travers] added a Scene Object\n");
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

		domBind_material *bindMaterial =  lib->getBind_material();
		if(bindMaterial)
		{
			// Get the <technique_common>
			domBind_material::domTechnique_common *techniqueCommon = bindMaterial->getTechnique_common();
			if(techniqueCommon)
			{
				// Get the <instance_material>s
				domInstance_material_Array &instanceMaterialArray = techniqueCommon->getInstance_material_array();
				for(unsigned j = 0; j < instanceMaterialArray.getCount(); j++)
				{

					domElement * element = instanceMaterialArray[j]->getTarget().getElement();
					if (element)
					{
						domMaterial * material = (domMaterial *) element;
	
						domMaterial * MaterialElement = (domMaterial*)(domElement*)element; 

						if ( MaterialElement ) 
						{
							if ( !MaterialElement->getInstance_effect() )
								return; 

							domElement * element = (domElement *) MaterialElement->getInstance_effect()->getUrl().getElement(); 
							if (element==NULL)
								return;

	
							// Get a pointer to the effect element
							domEffect * EffectElement = (domEffect*)(domElement*)element; 

							if ( EffectElement )
							{
								for (unsigned i = 0; i < EffectElement->getImage_array().getCount(); i++ )
								{
									domImage* imageElement = (domImage*)(domElement*)lib;
	
									if ( imageElement )
									{
										const char* FileName = imageElement->getInit_from()->getValue().str().c_str();
									}
								}
							}
						}
					}
				}
			}
		}
	}
}


SceneObject* ModelLoader::ReadScene(domVisual_scene *scene)
{
	SceneObject* rootScene = new SceneObject();
	for ( int i = 0; i < scene->getNode_array().getCount(); i++){
		domNode* currNode = scene->getNode_array()[i];
		travers(currNode, rootScene);
	}
	return rootScene;
}

SceneObject* ModelLoader::loadScene(const std::string path)
{
	DAE *dae = new DAE;	// TODO delete
	domCOLLADA* domRoot = dae->open(path);
	domCOLLADA::domSceneRef domScene = domRoot->getScene();
	daeElement* defaultScene = NULL;
	if (domScene)
		if (domScene->getInstance_visual_scene())
			if (domScene->getInstance_visual_scene())
				defaultScene = domScene->getInstance_visual_scene()->getUrl().getElement();

	for ( unsigned  i = 0; i < domRoot->getLibrary_images_array().getCount(); i++)
	{
		ReadImageLibrary( domRoot->getLibrary_images_array()[i] );			
	}
	for ( unsigned i = 0; i < domRoot->getLibrary_effects_array().getCount(); i++)
	{
		ReadEffectLibrary( domRoot->getLibrary_effects_array()[i] );			
	}
	for ( unsigned i = 0; i < domRoot->getLibrary_materials_array().getCount(); i++)
	{
		ReadMaterialLibrary( domRoot->getLibrary_materials_array()[i] );			
	}
	for ( unsigned i = 0; i < domRoot->getLibrary_animations_array().getCount(); i++)
	{
		ReadAnimationLibrary( domRoot->getLibrary_animations_array()[i] );			
	}
			
	if(defaultScene)
		return ReadScene( (domVisual_scene *)defaultScene );
}


void ModelLoader::ReadImageLibrary( domLibrary_imagesRef lib )
{
	printf(" ModelLoader::Reading Image Library \n" );	
	for ( unsigned i = 0; i < lib->getImage_array().getCount(); i++)
	{
		ModelImage *n = ReadImage( lib->getImage_array()[i] );
		images.insert(std::pair<std::string,ModelImage>(n->getID(),n));
	}	
}
void ModelLoader::ReadEffectLibrary( domLibrary_effectsRef lib )
{
	printf(" ModelLoader::Reading Effect Library \n" );	
	for ( unsigned i = 0; i < lib->getEffect_array().getCount(); i++)
	{
		//ReadEffect( lib->getEffect_array()[i] ); 
	}	
}
void ModelLoader::ReadMaterialLibrary( domLibrary_materialsRef lib )
{
	printf(" ModelLoader::Reading Material Library \n" );	
	for ( unsigned  i = 0; i < lib->getMaterial_array().getCount(); i++)
	{
		//ReadMaterial( lib->getMaterial_array()[i] ); 
	}
}
void ModelLoader::ReadAnimationLibrary( domLibrary_animationsRef lib )
{//no used

}

ModelImage* ModelLoader::ReadImage(domImageRef lib){
	ModelImage *n = new ModelImage();
	n->setID(lib->getID());
}