#include "ModelLoader.h"
#include <dae.h>


ModelLoader::ModelLoader(void)
{
	DAE root;
	root.open("../resources/SteamEngine/models/Steam EngineV2_6.dae");
}


ModelLoader::~ModelLoader(void)
{
}
