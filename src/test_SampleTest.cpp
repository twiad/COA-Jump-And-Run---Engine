#ifdef COAJNR_UNITTEST

#include "Dependencies.h"

namespace tut 
{ 
    struct data
    { 
    };
    
    typedef test_group<data> tg;
    tg testgroup("my first test");
    
    typedef tg::object testobject;
    
    template<> 
    template<> 
    void testobject::test<1>() 
    { 
        set_test_name("testtest");
        ensure_equals("equal",1 ,1);
        ensure("true",true);
        ensure_not("false", false);
        if(false)
            fail("so bad");
        ensure_distance(
                "in range",
                1.0 /* is */,
                2.0 /*expected*/,
                1.9 /*dist*/);
        
    }   
}; 

#endif
