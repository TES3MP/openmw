#ifndef OPENMW_MWPHYSICS_OBJECT_H
#define OPENMW_MWPHYSICS_OBJECT_H

#include "ptrholder.hpp"

#include <LinearMath/btTransform.h>
#include <osg/Node>

#include <map>
#include <mutex>

namespace Resource
{
    class BulletShapeInstance;
}

<<<<<<< HEAD
class btCollisionObject;
class btVector3;

=======
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
namespace MWPhysics
{
    class PhysicsTaskScheduler;

    class Object final : public PtrHolder
    {
    public:
        Object(const MWWorld::Ptr& ptr, osg::ref_ptr<Resource::BulletShapeInstance> shapeInstance, osg::Quat rotation,
            int collisionType, PhysicsTaskScheduler* scheduler);
        ~Object() override;

        const Resource::BulletShapeInstance* getShapeInstance() const;
        void setScale(float scale);
<<<<<<< HEAD
        void setRotation(const osg::Quat& quat);
=======
        void setRotation(osg::Quat quat);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        void updatePosition();
        void commitPositionChange();
        btTransform getTransform() const;
        /// Return solid flag. Not used by the object itself, true by default.
        bool isSolid() const;
        void setSolid(bool solid);
        bool isAnimated() const;
        /// @brief update object shape
        /// @return true if shape changed
        bool animateCollisionShapes();

    private:
        osg::ref_ptr<Resource::BulletShapeInstance> mShapeInstance;
        std::map<int, osg::NodePath> mRecIndexToNodePath;
        bool mSolid;
        btVector3 mScale;
        osg::Vec3f mPosition;
        osg::Quat mRotation;
<<<<<<< HEAD
        bool mScaleUpdatePending;
        bool mTransformUpdatePending;
=======
        bool mScaleUpdatePending = false;
        bool mTransformUpdatePending = false;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        mutable std::mutex mPositionMutex;
        PhysicsTaskScheduler* mTaskScheduler;
    };
}

#endif
