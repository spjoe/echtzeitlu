#include "ModelLoader.h"
#include <dae.h>
#include <dom.h>
#include <dom/domCOLLADA.h>
#include <dae/daeElement.h>
#include <dom/domVisual_scene.h>
#include <dom/domConstants.h>

#include <glm/glm.hpp>
#include <glm/gtx/comparison.hpp>
#include <dom/domProfile_COMMON.h>

#include "common.hpp"
#include "Model.h"
#include "SceneObject.h"
#include "shader.hpp"
#include "Timer.h"

using namespace echtzeitlu;

extern Shader* defaultShader;		// TODO der hat hier nix verloren!
extern Shader* defaultColorShader;		// TODO der hat hier nix verloren!

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
			domListOfFloats * texture1_floats = NULL;
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
			std::multimap<unsigned,std::pair<unsigned,unsigned> > pointtonormalmap;

			for(unsigned i=0; i<dom_triangles->getCount()*3; i++){
				unsigned pI = pointIndices[i];
				unsigned nI = normalIndices[i];
				
				if(pointlist.empty()){
					pointlist.push_back(points[pI]);
					pointtonormalmap.insert(std::pair<unsigned,std::pair<unsigned,unsigned> >(pI,std::pair<unsigned,unsigned>(nI,0)));
					normallist.push_back(normals[nI]);
					indexlist.push_back(pointlist.size()-1);
				}else{
					bool is_in = false; 
					if(pointtonormalmap.count(pI)){
						std::pair<std::multimap<unsigned, std::pair<unsigned,unsigned> >::iterator,std::multimap<unsigned, std::pair<unsigned,unsigned> >::iterator> pairnIs = pointtonormalmap.equal_range( pI );
						std::multimap<unsigned, std::pair<unsigned,unsigned> >::iterator nIs = pairnIs.first;
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
						pointtonormalmap.insert(std::pair<unsigned,std::pair<unsigned,unsigned> >(pI,std::pair<unsigned,unsigned>(nI,pointlist.size()-1)));
					}
				}
			}
			
			if(texture1_floats == NULL || true){ // oder true wegnehmen dann kann man texturen testen
				if(model == NULL){
					model = new Model(pointlist, normallist, indexlist, defaultColorShader);
					sceneObject->add( (SceneObject*)model );
				}else{
					SceneObject *submodel = new Model(pointlist, normallist, indexlist, defaultColorShader);
					model->add(submodel);
				}
				printf("[ModelLoader::travers] added a Scene Object\n");
				return;
			}
			//else

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


			printf("re-arranging vertexlist with texture\n");
			std::vector<glm::vec4> tpointlist;
			std::vector<glm::vec3> tnormallist;
			std::vector<glm::vec2> ttexturelist;
			std::vector<unsigned> tindexlist;
			std::multimap<unsigned,std::pair<unsigned,unsigned> > IndextoTextureIndexMap;

			for(unsigned i=0; i<dom_triangles->getCount()*3; i++){
				unsigned iI = indexlist[i];
				unsigned tI = textureIndices[i];
				
				if(tpointlist.empty()){
					tpointlist.push_back(pointlist[iI]);
					tnormallist.push_back(normallist[iI]);
					ttexturelist.push_back(texture[tI]);
					tindexlist.push_back(tpointlist.size()-1);
					IndextoTextureIndexMap.insert(std::pair<unsigned,std::pair<unsigned,unsigned> >(iI,std::pair<unsigned,unsigned>(tI,0)));
					
				}else{
					bool is_in = false; 
					if(IndextoTextureIndexMap.count(iI)){
						std::pair<std::multimap<unsigned, std::pair<unsigned,unsigned> >::iterator,std::multimap<unsigned, std::pair<unsigned,unsigned> >::iterator> pairnIs = IndextoTextureIndexMap.equal_range( iI );
						std::multimap<unsigned, std::pair<unsigned,unsigned> >::iterator nIs = pairnIs.first;
						for(; nIs != pairnIs.second ;nIs++)
						{
							if((*nIs).second.first == tI){ //element schon vorhanden
								tindexlist.push_back((*nIs).second.second);
								is_in = true;
								break;
							}
						}
						
					}
					
					if(is_in == false){
						tpointlist.push_back(pointlist[iI]);
						tnormallist.push_back(normallist[iI]);
						ttexturelist.push_back(texture[tI]);
						tindexlist.push_back(tpointlist.size()-1);
						IndextoTextureIndexMap.insert(std::pair<unsigned,std::pair<unsigned,unsigned> >(iI,std::pair<unsigned,unsigned>(tI,pointlist.size()-1)));
					}
				}
			}

			GLuint texid = (*images.begin()).second->getTexId();
			if(model == NULL){
				//model = new Model(pointlist, normallist, indexlist, defaultShader);
				model = new Model(tpointlist, tnormallist, ttexturelist, tindexlist, texid, defaultShader);
				sceneObject->add( (SceneObject*)model );
			}else{
				//SceneObject *submodel = new Model(pointlist, normallist, indexlist, defaultShader);
				SceneObject *submodel = new Model(tpointlist, tnormallist, ttexturelist, tindexlist, texid, defaultShader);
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
							ModelMaterial *mat = ReadMaterial(MaterialElement);
							mat = mat;
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
	}	
}
void ModelLoader::ReadEffectLibrary( domLibrary_effectsRef lib )
{
	printf(" ModelLoader::Reading Effect Library \n" );	
	for ( unsigned i = 0; i < lib->getEffect_array().getCount(); i++)
	{
		ReadEffect( lib->getEffect_array()[i] ); 
	}	
}
void ModelLoader::ReadMaterialLibrary( domLibrary_materialsRef lib )
{
	printf(" ModelLoader::Reading Material Library \n" );	
	for ( unsigned  i = 0; i < lib->getMaterial_array().getCount(); i++)
	{
		ReadMaterial( lib->getMaterial_array()[i] ); 
	}
}
void ModelLoader::ReadAnimationLibrary( domLibrary_animationsRef lib )
{//no used

}

ModelImage* ModelLoader::ReadImage(domImageRef lib){
	if(images.count(lib->getID()))
		return images[lib->getID()];


	ModelImage *n = new ModelImage(lib);
	n->setID(lib->getID());

	images.insert(std::pair<std::string,ModelImage*>(n->getID(),n));

	return n;
}
ModelEffect * ModelLoader::ReadEffect( domEffectRef lib ){
	if(effects.count(lib->getID()))
		return effects[lib->getID()];

	// Get a pointer to the effect element
	domEffect * EffectElement = (domEffect*)(domElement*)lib; 

	if ( EffectElement )
	{
		for (unsigned i = 0; i < EffectElement->getImage_array().getCount(); i++ )
		{
			ReadImage( EffectElement->getImage_array()[i] );
		}

		ModelEffect* newEffect = new ModelEffect();
		newEffect->setID(EffectElement->getID());
		
		// How many profiles are there
		unsigned numProfiles =  EffectElement->getFx_profile_abstract_array().getCount(); 

		// Scan the profiles to find the profile_COMMON
		for ( unsigned p = 0; p < numProfiles;  p ++)
		{
			std::string typeName = EffectElement->getFx_profile_abstract_array()[p]->getTypeName(); 
			
			if ( strcmp("profile_COMMON", typeName.c_str() ) == 0 )
			{
				// Found the common profile, get the technique from it as well
				domProfile_COMMON * common = (domProfile_COMMON *)(domFx_profile_abstract*)EffectElement->getFx_profile_abstract_array()[p]; 

				// Get all images in profile_COMMON
				for (unsigned i = 0; i < common->getImage_array().getCount(); i++ )
				{
					ModelImage *tmp = ReadImage( common->getImage_array()[i] );
					newEffect->addImage(tmp);
				}

				// Get all images in profile_COMMON
				domProfile_COMMON::domTechnique *technique = common->getTechnique(); 
				if ( technique == NULL )
					break; 

				for (unsigned i = 0; i < technique->getImage_array().getCount(); i++ )
				{
					ModelImage *tmp = ReadImage( technique->getImage_array()[i] );
					newEffect->addImage(tmp);
				}

				for (unsigned i = 0; i < common->getNewparam_array().getCount(); i++ )
				{
					domCommon_newparam_type *newparam = common->getNewparam_array()[i];
					domFx_surface_common *s = newparam->getSurface();
					if(s){
						domFx_surface_init_common* test = s->getFx_surface_init_common();
						//todo find image,...
					}
				}
				

			}
		}
		effects.insert(std::pair<std::string,ModelEffect*>(newEffect->getID(),newEffect));
		return newEffect;


	}
}
ModelMaterial * ModelLoader::ReadMaterial( domMaterialRef lib )
{
	if(materials.count(lib->getID()))
		return materials[lib->getID()];
	
	domMaterial * MaterialElement = (domMaterial*)(domElement*)lib; 

	if ( MaterialElement ) 
	{
		if ( !MaterialElement->getInstance_effect() )
			return NULL; 

		domElement * element = (domElement *) MaterialElement->getInstance_effect()->getUrl().getElement(); 
		if (element==NULL)
			return NULL;
		
		// find the effect that the material is refering too 
		ModelEffect * effect = ReadEffect((domEffect *) element);
		if (effect) 
		{
			ModelMaterial * n = new ModelMaterial(effect);
			n->setID(MaterialElement->getID());
			materials.insert(std::pair<std::string,ModelMaterial*>(n->getID(),n));
			return n; 	
		} 
	}
	return NULL; 
}