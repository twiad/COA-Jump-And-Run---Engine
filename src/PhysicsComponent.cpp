/******************************************************************************
 *                         CoAJnR - CoA Jump n Run                            *
 *                     Copyright (C) 2007  Adrian Jäkel                       *
 *                     Copyright (C) 2007  Franz Köhler                       *
 *                     Copyright (C) 2007  Robert Timm                        *
 ******************************************************************************
 * This library is free software; you can redistribute it and/or              *
 * modify it under the terms of the GNU Lesser General Public                 *
 * License as published by the Free Software Foundation; either               *
 * version 2.1 of the License, or (at your option) any later version.         *
 *                                                                            *
 * This library is distributed in the hope that it will be useful,            *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of             *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU          *
 * Lesser General Public License for more details.                            *
 *                                                                            *
 * You should have received a copy of the GNU Lesser General Public           *
 * License along with this library; if not, write to the Free Software        *
 * Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301 USA *
\******************************************************************************/

#include "PhysicsComponent.h"

#include "CollisionObject.h"
#include "ComponentContainer.h"
#include "DeferredCall.h"
#include "GraphicsManager.h"
#include "GraphicsMeshComponent.h"
#include "LogicsManager.h"
#include "MeshToShapeConverter.h"
#include "PhysicsManager.h"
#include "ThreadPool.h"

namespace CoAJnR
{

PhysicsComponent::State::State(
        PhysicsComponentPtr p_owner, 
        PositionComponentPtr p_ownerPosition,
        btTransform p_startTrans) : 
    m_transform(p_startTrans), 
    m_owner(p_owner),
    m_ownerPosition(p_ownerPosition)
{
}

PhysicsComponent::State::~State()
{
}

void
PhysicsComponent::State::getWorldTransform(btTransform& p_transform) const
{
    p_transform = m_transform;
}

void
PhysicsComponent::State::sync()
{
    btVector3 linearVelocity = btVector3(0,0,0);
    
    if(m_owner->ready())
        linearVelocity = m_owner->collisionObject()->getLinearVelocity();
    
    m_ownerPosition->dataChangedCallback(
            Ogre::Vector3(
                    m_transform.getOrigin().x(), 
                    m_transform.getOrigin().y(), 
                    m_transform.getOrigin().z()),
            Ogre::Quaternion(
                    m_transform.getRotation().w(), 
                    m_transform.getRotation().x(), 
                    m_transform.getRotation().y(), 
                    m_transform.getRotation().z()),
            Ogre::Vector3(
                    linearVelocity.x(),
                    linearVelocity.y(),
                    linearVelocity.z()));
}

void 
PhysicsComponent::State::setWorldTransform(
        const btTransform& p_transform)
{
    m_transform = p_transform;

    sync();
}

PhysicsComponent::PhysicsComponent(
        PhysicsShape p_shape, 
        double p_mass) :
    m_manualBounds(Ogre::Vector3::ZERO), 
    m_shape(p_shape), 
    m_collisionObject(0), 
    m_mass(p_mass)
{
}

PhysicsComponent::PhysicsComponent(
        Ogre::Vector3 p_manualBounds,
        PhysicsShape p_shape, 
        double p_mass) : 
    m_manualBounds(p_manualBounds), 
    m_shape(p_shape), 
    m_collisionObject(0), 
    m_mass(p_mass)
{
}

PhysicsComponent::~PhysicsComponent()
{
}

std::string 
PhysicsComponent::defaultName() const
{
    return std::string("PhysicsMain");
}


std::vector<std::string> 
PhysicsComponent::dependencies()
{
    std::vector<std::string> dependencies;

    dependencies.push_back("Position");

    // if we did not get an manual shape bounding box, 
    // look out for a graphics mesh component 
    if(m_manualBounds == Ogre::Vector3::ZERO)
    {
        if(m_mass > 0.0f)
            dependencies.push_back("GraphicsMeshMain");
        else
            dependencies.push_back("GraphicsStaticMeshMain");
    }

    return dependencies;
}

void
PhysicsComponent::attachedCallback()
{
    assert(parent() && "parent must not be null");
    assert(parent()->id() && "component container has invalid ObjectId");

    // get dependencies
    m_positionComponent = 
        boost::dynamic_pointer_cast<PositionComponent>(
                parent()->component("Position"));

    if(m_manualBounds == Ogre::Vector3::ZERO)
    {
        if(m_mass > 0.0f)
            m_graphicsMeshComponent = 
                    boost::dynamic_pointer_cast<GraphicsMeshComponentInterface>(
                            parent()->component("GraphicsMeshMain"));
        else
            m_graphicsMeshComponent = 
                    boost::dynamic_pointer_cast<GraphicsMeshComponentInterface>(
                            parent()->component("GraphicsStaticMeshMain"));

        assert(m_graphicsMeshComponent && "failed to get dependent component");
    }
    else
    {
        assert((m_shape == PS_Box || m_shape == PS_Sphere) && 
                "only box and sphere shape support manual bounds");
    }
    
    
    // Graphic Component is not present, could be created
    assert(m_positionComponent && "failed to get dependent component");


    struct CreatePhysicsObjectFromGraphicsMeshCall : public DeferredCall
    {
        PositionComponentPtr positionComponent;
        GraphicsMeshComponentInterfacePtr graphicsMeshComponent;
        PhysicsComponentPtr physicsComponent;

        bool operator()()
        {
            if(physicsComponent->m_manualBounds == Ogre::Vector3::ZERO)
                if(!graphicsMeshComponent->ready())
                    return false;

            struct CreatePhysicsBodyCall : public DeferredCall
            {
                btCollisionShape* shape;
                PhysicsComponentPtr physicsComponent;
                PositionComponentPtr positionComponent;

                void finalizeBodyCreation()
                {
                    btVector3 localInertia = btVector3(0,0,0);
                    double mass = physicsComponent->mass();

                    if(mass > 0.0f)
                        shape->calculateLocalInertia(mass,localInertia);

                    btTransform trans = btTransform::getIdentity();
                    trans.setOrigin(btVector3(
                            positionComponent->position().x,
                            positionComponent->position().y,
                            positionComponent->position().z));

                    trans.setRotation(btQuaternion(
                            positionComponent->orientation().x,
                            positionComponent->orientation().y,
                            positionComponent->orientation().z,
                            positionComponent->orientation().w));

                    PhysicsComponent::State* state =
                            new PhysicsComponent::State(
                                    physicsComponent,
                                    positionComponent,
                                    trans);
                    
                    CollisionObject* collisionObject = 
                            new CollisionObject(
                                    physicsComponent, 
                                    mass, 
                                    state, 
                                    shape, 
                                    localInertia);

                    collisionObject->setRestitution(0.0);
                    collisionObject->setFriction(1.0);

                    PhysicsManager::get().world()->
                            addRigidBody(collisionObject);
                    // std::cout << "O";
                    if(mass == 0.0f)
                        collisionObject->setCollisionFlags(
                                collisionObject->getCollisionFlags() | 
                                btCollisionObject::CF_STATIC_OBJECT);

                    state->sync();

                    assert(!physicsComponent->m_collisionObject);
                    physicsComponent->m_collisionObject = collisionObject;

                    ThreadPool::get().schedule(boost::bind(
                            &ComponentContainer::componentAttachedCallback,
                            physicsComponent->parent(), 
                            physicsComponent));
                }
            };

            struct CreatePhysicsConvexBodyCall : 
                    public CreatePhysicsBodyCall
            {
                Ogre::Real* vertices;
                uint numVertices;

                bool operator()()
                {
                    if(numVertices > 99)
                        std::cout << "WARNING: 100 or more vertices in "
                            << "convex hull shape. "
                            << "this may lead to instability." 
                            << std::endl;
                    
                    shape = new btConvexHullShape(
                            (btScalar*)vertices, 
                            numVertices, 
                            3 * sizeof(Ogre::Real));

                    delete[] vertices;

                    finalizeBodyCreation();

                    return true;
                }
            };

            struct CreatePhysicsBoxBodyCall : 
                    public CreatePhysicsBodyCall
            {
                Ogre::Vector3 size;

                bool operator()()
                {   
                    shape = new btBoxShape(btVector3(
                            size.x, 
                            size.y, 
                            size.z));

                    finalizeBodyCreation();

                    return true;
                }
            };

            struct CreatePhysicsSphereBodyCall : 
                    public CreatePhysicsBodyCall
            {
                Ogre::Real radius;

                bool operator()()
                {   
                    shape = new btSphereShape(radius * 2.0f);

                    finalizeBodyCreation();

                    return true;
                }
            };

            struct CreatePhysicsTrimeshBodyCall : 
                    public CreatePhysicsBodyCall
            {
                Ogre::Real* vertices;
                uint numVertices;
                uint* indices;
                uint numIndices;

                bool operator()()
                {   
                    btTriangleMesh* triMesh = new btTriangleMesh;

                    for (uint n = 0; n < numIndices / 3; n++)
                        triMesh->addTriangle(
                                btVector3(
                                    vertices[(3 * indices[(3 * n)]) + 0],
                                    vertices[(3 * indices[(3 * n)]) + 1],
                                    vertices[(3 * indices[(3 * n)]) + 2]),
                                btVector3(
                                    vertices[(3 * indices[(3 * n) + 1]) + 0],
                                    vertices[(3 * indices[(3 * n) + 1]) + 1],
                                    vertices[(3 * indices[(3 * n) + 1]) + 2]),
                                btVector3(
                                    vertices[(3 * indices[(3 * n) + 2]) + 0],
                                    vertices[(3 * indices[(3 * n) + 2]) + 1],
                                    vertices[(3 * indices[(3 * n) + 2]) + 2]));

                    shape = new btBvhTriangleMeshShape(triMesh, true);                             
                    
                    delete[] vertices;
                    delete[] indices;
                    
                    finalizeBodyCreation();

                    return true;
                }
            };


            switch(physicsComponent->physicsShape())
            {
            default:
            case PS_Convex:
            {
                Ogre::Real* vertices;
                uint numVertices;

                MeshToShapeConverter shapeConv(
                        graphicsMeshComponent->entity());

                shapeConv.getVertexArray(vertices, numVertices);

                CreatePhysicsConvexBodyCall* call = 
                        new CreatePhysicsConvexBodyCall;

                call->vertices = vertices;
                call->numVertices = numVertices;
                call->physicsComponent = physicsComponent;
                call->positionComponent = positionComponent;

                PhysicsManager::get().enqueueCall(call);
            }
            break;
            case PS_Trimesh:
            {
                assert(physicsComponent->mass() == 0.0f && 
                        "only static trimesh objects supported");

                Ogre::Real* vertices;
                uint numVertices;
                uint* indices;
                uint numIndices;

                MeshToShapeConverter shapeConv(
                        graphicsMeshComponent->entity());

                shapeConv.getVertexArray(vertices, numVertices);
                        shapeConv.getIndexArray(indices, numIndices);

                CreatePhysicsTrimeshBodyCall* call = 
                        new CreatePhysicsTrimeshBodyCall;

                call->vertices = vertices;
                call->numVertices = numVertices;
                call->indices = indices;
                call->numIndices = numIndices;
                call->physicsComponent = physicsComponent;
                call->positionComponent = positionComponent;

                PhysicsManager::get().enqueueCall(call);
            }
            break;
            case PS_Box:
            {
                CreatePhysicsBoxBodyCall* call = new CreatePhysicsBoxBodyCall;

                if(physicsComponent->m_manualBounds == Ogre::Vector3::ZERO)
                {
                    MeshToShapeConverter shapeConv(
                            graphicsMeshComponent->entity());

                    shapeConv.getBox(call->size);
                }
                else
                    call->size = physicsComponent->m_manualBounds;
                
                call->physicsComponent = physicsComponent;
                call->positionComponent = positionComponent;

                PhysicsManager::get().enqueueCall(call);
            }
            break;
            case PS_Sphere:
            {
                CreatePhysicsSphereBodyCall* call = 
                        new CreatePhysicsSphereBodyCall;

                if(physicsComponent->m_manualBounds == Ogre::Vector3::ZERO)
                {
                    MeshToShapeConverter shapeConv(
                            graphicsMeshComponent->entity());
                    
                    shapeConv.getSphereRadius(call->radius);
                }
                else
                {
                    call->radius = physicsComponent->m_manualBounds.x;
                    
                    if(call->radius < physicsComponent->m_manualBounds.y)
                        call->radius = physicsComponent->m_manualBounds.y;
                    
                    if(call->radius < physicsComponent->m_manualBounds.z)
                        call->radius = physicsComponent->m_manualBounds.z;
                }
                call->physicsComponent = physicsComponent;
                call->positionComponent = positionComponent;

                PhysicsManager::get().enqueueCall(call);
            }
            break;
            }
            
            return true;
        }
        
    };
    
    CreatePhysicsObjectFromGraphicsMeshCall* call = 
            new CreatePhysicsObjectFromGraphicsMeshCall();

    call->graphicsMeshComponent = m_graphicsMeshComponent;
    call->positionComponent = m_positionComponent;
    call->physicsComponent = shared_from_this();
    
    GraphicsManager::get().enqueueCall(call);
}

void 
PhysicsComponent::attachCollisionHandlerComponent(
        CollisionHandlerComponentPtr p_collisionHandler)
{

    struct AttachCollisionHandlerComponentCall : public DeferredCall
    {
        PhysicsComponentPtr physicsComponent;
        CollisionHandlerComponentPtr collisionHandler;

        bool operator()()
        {
            if(!physicsComponent->collisionObject())
                return false;

            physicsComponent->collisionObject()->
                    attachCollisionHandlerComponent(collisionHandler);

            return true;
        }
    };

    AttachCollisionHandlerComponentCall* call = 
            new AttachCollisionHandlerComponentCall;
    call->physicsComponent = shared_from_this();
    call->collisionHandler = p_collisionHandler;

    PhysicsManager::get().enqueueCall(call);
}

void 
PhysicsComponent::detachCollisionHandlerComponent(
        CollisionHandlerComponentPtr p_collisionHandler)
{

    struct DetachCollisionHandlerComponentCall : public DeferredCall
    {
        PhysicsComponentPtr physicsComponent;
        CollisionHandlerComponentPtr collisionHandler;

        bool operator()()
        {
            if(!physicsComponent->collisionObject())
                return false;

            physicsComponent->collisionObject()->
                    detachCollisionHandlerComponent(collisionHandler);

            return true;
        }
    };

    DetachCollisionHandlerComponentCall* call = 
            new DetachCollisionHandlerComponentCall;
    call->physicsComponent = shared_from_this();
    call->collisionHandler = p_collisionHandler;

    PhysicsManager::get().enqueueCall(call);
}

void
PhysicsComponent::detachedCallback()
{
    struct DestroyPhysicsBodyCall : public DeferredCall
    {
        PhysicsComponentPtr physicsComponent;
        bool operator()()
        {
            if(!physicsComponent->ready())
                return false;
            
            PhysicsManager::get().world()->removeRigidBody(
                    physicsComponent->m_collisionObject);
            
            if(physicsComponent->physicsShape() == PS_Trimesh)
            {
                btTriangleMeshShape* triShape = 
                        dynamic_cast<btTriangleMeshShape*>(
                                physicsComponent->
                                        collisionObject()->getCollisionShape());
                
                if(triShape)
                    delete triShape->getMeshInterface();
            
            }
            
            delete physicsComponent->m_collisionObject->getMotionState();
            delete physicsComponent->m_collisionObject;
            delete physicsComponent->collisionObject()->getCollisionShape();

            physicsComponent->m_collisionObject = 0;

            ThreadPool::get().schedule(boost::bind(
                                &ComponentContainer::componentDetachedCallback,
                                physicsComponent->parent(), physicsComponent));

            return true;
        }
    };
    
    DestroyPhysicsBodyCall* call = new DestroyPhysicsBodyCall;
    call->physicsComponent = shared_from_this();

    PhysicsManager::get().enqueueCall(call);
}

void 
PhysicsComponent::setPosition(Ogre::Vector3 p_position)
{
    struct SetPositionCall : public DeferredCall
    {
        Ogre::Vector3 position;
        PhysicsComponentPtr physicsComponent;
        bool operator()()
        {
            if(!physicsComponent->ready())
                return false;

            assert((!physicsComponent->collisionObject()->isStaticObject() ||
                    physicsComponent->collisionObject()->isKinematicObject()) &&
                    "static objects must not be moved, setKinematic() first");

            /// @todo TODO: is there a more easy way?
            physicsComponent->collisionObject()->getWorldTransform().
                    setOrigin(btVector3(position.x, position.y, position.z));

            btTransform trans;

            physicsComponent->collisionObject()->
                    getMotionState()->getWorldTransform(trans);

            trans.setOrigin(btVector3(position.x, position.y, position.z));

            physicsComponent->collisionObject()->
                    getMotionState()->setWorldTransform(trans);

            return true;
        }
    };

    SetPositionCall* call = new SetPositionCall;
    call->physicsComponent = shared_from_this();
    call->position = p_position;

    PhysicsManager::get().enqueueCall(call);
}

void 
PhysicsComponent::setOrientation(Ogre::Quaternion p_orientation)
{
    DEFERRED_CALL_VARS(SetOrientationCall)
        Ogre::Quaternion orientation;
        PhysicsComponentPtr physicsComponent;

    DEFERRED_CALL_CODE()
        if(!physicsComponent->ready())
            return false;

        assert((!physicsComponent->collisionObject()->isStaticObject() ||
                physicsComponent->collisionObject()->isKinematicObject()) &&
                "static objects must not be moved, setKinematic() first");

        physicsComponent->collisionObject()->getWorldTransform().
                setRotation(btQuaternion(
                        orientation.x, 
                        orientation.y, 
                        orientation.z, 
                        orientation.w));

        return true;
    
    DEFERRED_CALL_END()
    
    DEFERRED_CALL_CREATE(SetOrientationCall)
        call->physicsComponent = shared_from_this();
        call->orientation = p_orientation;

    DEFERRED_CALL_ENQUEUE(PhysicsManager)
}

void 
PhysicsComponent::setKinematic(bool p_kinematic)
{
    DEFERRED_CALL_VARS(SetKinematicCall)
        bool kinematic;
        PhysicsComponentPtr physicsComponent;

    DEFERRED_CALL_CODE()
        if(!physicsComponent->ready())
            return false;

        /// @todo TODO: this may be possible for former dynamic objects too
        assert(physicsComponent->mass() == 0.0f && 
                "only static objects can get kinematic");

        if(kinematic)
        {
            physicsComponent->collisionObject()->setCollisionFlags(
                    physicsComponent->collisionObject()->
                            getCollisionFlags() | 
                    btCollisionObject::CF_KINEMATIC_OBJECT);
    
            physicsComponent->collisionObject()->
                    setActivationState(DISABLE_DEACTIVATION); 
        }
        else
        {
            physicsComponent->collisionObject()->setCollisionFlags(
                    physicsComponent->collisionObject()->
                            getCollisionFlags() & 
                    !btCollisionObject::CF_KINEMATIC_OBJECT);

            physicsComponent->collisionObject()->
                    setActivationState(WANTS_DEACTIVATION); 
        }

        return true;

    DEFERRED_CALL_END()
    
    SetKinematicCall* call = new SetKinematicCall;
    call->physicsComponent = shared_from_this();
    call->kinematic = p_kinematic;

    PhysicsManager::get().enqueueCall(call);
}

void 
PhysicsComponent::applyForce(
        Ogre::Vector3 p_force, 
        Ogre::Vector3 p_relativePosition)
{
    struct SetForceCall : public DeferredCall
    {
        Ogre::Vector3 force;
        Ogre::Vector3 relativePosition;
        PhysicsComponentPtr physicsComponent;
        bool operator()()
        {
            if(!physicsComponent->ready())
                return false;

            assert((!physicsComponent->collisionObject()->isStaticObject() &&
                        !physicsComponent->collisionObject()->
                                isKinematicObject()) 
                        && "tried to apply force on not a dynamic object");

            physicsComponent->collisionObject()->
                    applyForce(
                            btVector3(force.x, force.y, force.z),
                            btVector3(
                                relativePosition.x, 
                                relativePosition.y, 
                                relativePosition.z));
            
            return true;
        }
    };

    SetForceCall* call = new SetForceCall;
    call->physicsComponent = shared_from_this();
    call->force = p_force;
    call->relativePosition = p_relativePosition;
    
    PhysicsManager::get().enqueueCall(call);
    
}

void 
PhysicsComponent::applyTorque(Ogre::Vector3 p_torque)
{
    struct ApplyTorqueCall : public DeferredCall
        {
            Ogre::Vector3 torque;
            PhysicsComponentPtr physicsComponent;
            bool operator()()
            {
                if(!physicsComponent->ready())
                    return false;

                assert((!physicsComponent->collisionObject()->isStaticObject()
                        && !physicsComponent->collisionObject()->
                                isKinematicObject()) &&
                        "tried to apply force on not a dynamic object");

                physicsComponent->collisionObject()->
                        applyTorque(btVector3(torque.x, torque.y, torque.z));
                
                return true;
            }
        };

        ApplyTorqueCall* call = new ApplyTorqueCall;
        call->physicsComponent = shared_from_this();
        call->torque = p_torque;
        
        PhysicsManager::get().enqueueCall(call);    
}

void 
PhysicsComponent::applyImpulse(Ogre::Vector3 p_impulse, Ogre::Vector3 p_relativePosition)
{
    struct ApplyImpulseCall : public DeferredCall
        {
            Ogre::Vector3 impulse;
            Ogre::Vector3 relativePosition;
            PhysicsComponentPtr physicsComponent;
            bool operator()()
            {
                if(!physicsComponent->ready())
                    return false;

                assert((!physicsComponent->collisionObject()->isStaticObject()
                        && !physicsComponent->collisionObject()->
                                isKinematicObject()) 
                        && "tried to apply force on not a dynamic object");

                physicsComponent->collisionObject()->
                        applyImpulse(
                                btVector3(impulse.x, impulse.y, impulse.z),
                                btVector3(
                                        relativePosition.x, 
                                        relativePosition.y,
                                        relativePosition.z));
                
                return true;
            }
        };

        ApplyImpulseCall* call = new ApplyImpulseCall;
        call->physicsComponent = shared_from_this();
        call->impulse = p_impulse;
        call->relativePosition = p_relativePosition;
        
        PhysicsManager::get().enqueueCall(call);    
}

void 
PhysicsComponent::applyTorqueImpulse(Ogre::Vector3 p_torqueImpulse)
{
    struct ApplyTorqueImpulseCall : public DeferredCall
        {
            Ogre::Vector3 torqueImpulse;
            PhysicsComponentPtr physicsComponent;
            bool operator()()
            {
                if(!physicsComponent->ready())
                    return false;

                assert((!physicsComponent->collisionObject()->isStaticObject()
                        &&!physicsComponent->collisionObject()->
                        isKinematicObject()) &&
                        "tried to apply force on not a dynamic object");

                physicsComponent->collisionObject()->
                        applyTorqueImpulse(btVector3(
                                torqueImpulse.x, 
                                torqueImpulse.y, 
                                torqueImpulse.z));
                
                return true;
            }
        };

        ApplyTorqueImpulseCall* call = new ApplyTorqueImpulseCall;
        call->physicsComponent = shared_from_this();
        call->torqueImpulse = p_torqueImpulse;
        
        PhysicsManager::get().enqueueCall(call);    
}
void 
PhysicsComponent::setLinearVelocity(Ogre::Vector3 p_velocity)
{
    DEFERRED_CALL_VARS(SetLinearVelocityCall)
        Ogre::Vector3 velocity;
        PhysicsComponentPtr physicsComponent;

    DEFERRED_CALL_CODE()
        if(!physicsComponent->ready())
            return false;

        assert((!physicsComponent->collisionObject()->isStaticObject()
                &&!physicsComponent->collisionObject()->
                isKinematicObject()) &&
                "tried to set velocity on not a dynamic object");

        physicsComponent->collisionObject()->
                setLinearVelocity(btVector3(
                        velocity.x, 
                        velocity.y, 
                        velocity.z));
        
        return true;
        
    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(SetLinearVelocityCall)
        call->physicsComponent = shared_from_this();
        call->velocity = p_velocity;

    DEFERRED_CALL_ENQUEUE(PhysicsManager)
}

void 
PhysicsComponent::setAngularVelocity(Ogre::Vector3 p_velocity)
{
    DEFERRED_CALL_VARS(SetAngularVelocityCall)
        Ogre::Vector3 velocity;
        PhysicsComponentPtr physicsComponent;

    DEFERRED_CALL_CODE()
        if(!physicsComponent->ready())
            return false;

        assert((!physicsComponent->collisionObject()->isStaticObject()
                &&!physicsComponent->collisionObject()->
                isKinematicObject()) &&
                "tried to set velocity on not a dynamic object");

        physicsComponent->collisionObject()->
                setAngularVelocity(btVector3(
                        velocity.x, 
                        velocity.y, 
                        velocity.z));
        
        return true;
        
    DEFERRED_CALL_END()

    DEFERRED_CALL_CREATE(SetAngularVelocityCall)
        call->physicsComponent = shared_from_this();
        call->velocity = p_velocity;

    DEFERRED_CALL_ENQUEUE(PhysicsManager)
}

}
