
#include "OgreBulletCollisions.h"

#include "Shapes/OgreBulletCollisionsEmptyShape.h"

using namespace Ogre;
using namespace OgreBulletCollisions;

namespace OgreBulletCollisions
{
    // -------------------------------------------------------- //
    EmptyCollisionShape::EmptyCollisionShape():	
        CollisionShape()
    {
        // mShape = new btEmptyShape();
        mShape = 0;
#warning "just compile fix, EmptyCollisionShape may be bugged due to 0 ptr"
    }
    // -------------------------------------------------------- //
    EmptyCollisionShape::~EmptyCollisionShape()
    {
    }

}

