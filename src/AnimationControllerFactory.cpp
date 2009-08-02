#include "stdafx.h"
#include "ModelLoader.h"
#include "ModelLoaderMD2.h"
#include "ModelLoaderMD3.h"
#include "ModelLoaderOBJ.h"
#include "AnimationControllerFactory.h"

AnimationControllerFactory::AnimationControllerFactory(TextureFactory &_textureFactory)
: textureFactory(_textureFactory)
{
	loader[".objxml"] = shared_ptr<ModelLoader>(new ModelLoaderOBJ());
	loader[".md3xml"] = shared_ptr<ModelLoader>(new ModelLoaderMD3());
	loader[".md2xml"] = shared_ptr<ModelLoader>(new ModelLoaderMD2());
}

AnimationController*
AnimationControllerFactory::createFromFile(const FileName &fileName)
{
    AnimationController *model = 0;

	MAP::iterator i = loader.find(fileName.getExtension());

    if(i == loader.end())
    {
        return createDefault();
    }

	shared_ptr<ModelLoader> modelLoader = i->second;

	ASSERT(modelLoader, "modelLoader was null");

	model = modelLoader->load(fileName, textureFactory);

	if(!model)
	{
        return createDefault();
	}

    return model;
}

AnimationController* AnimationControllerFactory::createDefault()
{
    AnimationController *model = 0;

	const FileName fileName("data/models/cylinder/cylinder.md3xml");

	MAP::iterator i = loader.find(fileName.getExtension());

	if(i == loader.end() ||
      (i != loader.end() &&
	       !(model = i->second->load(fileName, textureFactory))))
	{
        FAIL("Failed to load default model: " + fileName.str());
        return 0;
	}
       
    return model;
}
