#include "MainApplication.h"

#include "GraphicsManager.h"
#include "SceneryTest.h"

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

    GraphicsManager* gm = GraphicsManager::get();

    if(!gm->init(new SceneryTest))
        return;

    gm->startRendering();
    
    delete gm;
}


}
