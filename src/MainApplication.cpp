#include "MainApplication.h"

#include "Renderer.h"

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

    Renderer* renderer = new Renderer();

    if(!renderer->init())
        return;

    renderer->startRendering();
    
    delete renderer;
}


}
