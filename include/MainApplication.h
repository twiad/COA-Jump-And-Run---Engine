
#ifndef COABLASTER_MAINAPPLICATION_INCLUDED
#define COABLASTER_MAINAPPLICATION_INCLUDED

#include "Dependencies.h"

namespace CoABlaster
{

class Renderer;

/**
 * the applications root
 */
class MainApplication
{
    Renderer* m_renderer;

public:
    MainApplication();

    virtual ~MainApplication();    
    
    void go();
};

}

#endif
