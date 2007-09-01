#include "Character.h"

#include "PhysicsManager.h"
#include "GraphicsManager.h"
#include "InteractionManager.h"

#include "MainApplication.h"

namespace CoABlaster
{

/// @todo TODO: create a special object state for characters to have only
/// characters always axis aligned, next is the boxstack then ^^

Character::Character(std::string p_idenitfier, std::string p_meshFile)
    : OgreBulletDynamics::RigidBody(
            p_idenitfier, PhysicsManager::get()->world()) 
{
    m_identifier = p_idenitfier;
    
    m_moveRotation = 20;
    m_moveImpule   = 50;
    m_jumpForce    =  10;
    m_maxGrabDistance = 3;

    m_entity = GraphicsManager::get()->sceneManager()->
            createEntity(p_idenitfier, p_meshFile);

    m_entity->setNormaliseNormals(true);

    setShape(
            GraphicsManager::get()->sceneManager()->
                    getRootSceneNode()->createChildSceneNode(m_identifier), 
            new OgreBulletCollisions::SphereCollisionShape(1), 
            2.0, /* ............................................. restitution */
            2.0, /* ............................................. friction    */
            1,   /* ............................................. mass        */
            Ogre::Vector3(0, 7, 0));

    mRootNode->attachObject(m_entity);
    
    InteractionManager::get()->addCharacter(this);

    m_grabConstraint = 0;    
    m_grabbedObject = 0;    
}

Character::~Character()
{
    /// @todo TODO: cleanup
    InteractionManager::get()->removeCharacter(this);
    GraphicsManager::get()->sceneManager()->destroyEntity(m_entity);
    GraphicsManager::get()->sceneManager()->destroySceneNode(m_identifier);

}

/// @todo TODO: dynamic (character size)
/// @todo TODO: maybe another way to prevent ray-self collision?
/// @todo TODO: different ray tresholds to approx sphere shape and prevent
///             jumping off corners
bool
Character::isCharacterOnGround()
{
    Ogre::Vector3 origin = getWorldPosition();
    Ogre::Vector3 target;
    
    // go one to the left
    origin.x--;
        
    /// test 3, left, middle, right (!!! adjust with char size !!!)
    for(int i = 0; i < 3; i++)
    {   
        // ray in negative y axis
        target = Ogre::Vector3(origin.x, origin.y - 1.1, origin.z);
        
        Ogre::Ray ray(origin, target);
        OgreBulletCollisions::CollisionClosestRayResultCallback
                cb(ray, mWorld);
    
        mWorld->launchRay(cb);
        
        if(cb.doesCollide() && cb.getCollidedObject() != this && 
                cb.getCollidedObject() != m_grabbedObject)
            return true;
    
        origin.x++;
    }
    
    return false;
}

void
Character::move(double p_factor)
{
    Ogre::Vector3 angularVelocity = Ogre::Vector3::ZERO; 
    Ogre::Vector3 linearImpulse = Ogre::Vector3::ZERO; 

    angularVelocity += Ogre::Vector3(0,0,-m_moveRotation * p_factor);
    linearImpulse += Ogre::Vector3(m_moveImpule * p_factor,0,0);

    enableActiveState();

    setAngularVelocity(angularVelocity);
    applyImpulse(linearImpulse, Ogre::Vector3(0,0,0));
}

void
Character::jump(double p_elapsed)
{
    if(isCharacterOnGround())
    {
        Ogre::Vector3 vel = getLinearVelocity();
        setLinearVelocity(vel.x, m_jumpForce, vel.z);
    }
}
    

void
Character::moveLeft(double p_elapsed)
{
    move(-p_elapsed);
}

void
Character::moveRight(double p_elapsed)
{
    move(p_elapsed);
}

void
Character::applyMovementCorrections()
{
    Ogre::Vector3 vel = getLinearVelocity();
    if((vel != Ogre::Vector3::ZERO))
    {
        enableActiveState();
        setLinearVelocity(
            // vel.x * (pow(0.0001, p_elapsed * 1.0)), vel.y, 0);
            vel.x * 0.5, vel.y, 0);

        // std::cout << vel << std::endl;
    }

    Ogre::Vector3 pos = getWorldPosition();
    getBulletRigidBody()->getWorldTransform().setOrigin(
            btVector3(pos.x, pos.y, 0));

    // setPosition(pos.x, pos.y, 0);
    //setOrientation(btQuaternion());
    
}

OgreBulletDynamics::RigidBody* 
Character::findNextObject()
{
    /// @todo TODO: look also top bottom right front and back
    
    Ogre::Vector3 origin = getWorldPosition();
    origin.y -= 0.5; // look a bit lower

    Ogre::Vector3 target;


    // cast a ray to the left
    target = Ogre::Vector3(origin.x - m_maxGrabDistance, origin.y, origin.z);
    
    Ogre::Ray ray(origin, target);
    OgreBulletCollisions::CollisionClosestRayResultCallback cb(ray, mWorld);
    mWorld->launchRay(cb);

    if(cb.doesCollide() && cb.getCollidedObject() != this)
        return dynamic_cast<OgreBulletDynamics::RigidBody*>(
                cb.getCollidedObject());
    
    return 0;
}

void
Character::grab()
{
    if(m_grabConstraint)
        return;
    
    OgreBulletDynamics::RigidBody* grabObject = findNextObject();
    
    if(grabObject)
    {
        Ogre::Vector3 distance = 
                this->getWorldPosition() - grabObject->getWorldPosition();
        
        m_grabConstraint = new OgreBulletDynamics::ConeTwistConstraint(
		    grabObject, 
		    this, 
            distance,
            grabObject->getWorldOrientation(),
            Ogre::Vector3::ZERO,
            this->getWorldOrientation());

        PhysicsManager::get()->world()->addConstraint(m_grabConstraint);

        m_grabbedObject = grabObject;
    }    
}

void
Character::ungrab()
{
    if(!m_grabConstraint)
        return;
    
    PhysicsManager::get()->world()->removeConstraint(m_grabConstraint);
    delete m_grabConstraint;
    m_grabConstraint = 0;
    m_grabbedObject = 0;
}

}
