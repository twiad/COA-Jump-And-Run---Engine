#include "MainApplication.h"

#include "GraphicsManager.h"

namespace CoABlaster
{

MainApplication::MainApplication()
{
    
}

MainApplication::~MainApplication()
{
    
}

void
MainApplication::go()
{
    std::cout << COABLASTER_VERION_STRING << std::endl;

    GraphicsManager* gm = new GraphicsManager();

    if(!gm->init())
        return;

    gm->startRendering();
    
    delete gm;
}


}
