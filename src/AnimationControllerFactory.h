#ifndef _ANIMATION_CONTROLLER_FACTORY_H_
#define _ANIMATION_CONTROLLER_FACTORY_H_

#include "AnimationController.h"

class ModelLoader; // class prototype

/** Creates AnimationController objects from file */
class AnimationControllerFactory
{
public:
    AnimationControllerFactory(TextureFactory &textureFactory);

    /**
    Creates an animated model from file
    @param fileName Name of the file containing model data
    @return New animation controller; ownership passes to the caller.
    */
    AnimationController* createFromFile(const FileName &fileName);

    /**
    Creates an instance of the "default" model.
    @return New animation controller; ownership passes to the caller.
    */
    AnimationController* createDefault();

private:
    typedef map<string, shared_ptr<ModelLoader> > MAP;

    MAP loader;

	TextureFactory &textureFactory;
};

#endif
