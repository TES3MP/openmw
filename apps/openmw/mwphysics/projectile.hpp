#ifndef OPENMW_MWPHYSICS_PROJECTILE_H
#define OPENMW_MWPHYSICS_PROJECTILE_H

#include <atomic>
#include <memory>
#include <mutex>

#include <LinearMath/btVector3.h>

#include "ptrholder.hpp"

class btCollisionObject;
class btCollisionShape;
class btConvexShape;

namespace osg
{
    class Vec3f;
}

namespace MWPhysics
{
    class PhysicsTaskScheduler;
    class PhysicsSystem;

    class Projectile final : public PtrHolder
    {
    public:
<<<<<<< HEAD
        Projectile(const MWWorld::Ptr& caster, const osg::Vec3f& position, float radius, PhysicsTaskScheduler* scheduler, PhysicsSystem* physicssystem);
=======
        Projectile(const MWWorld::Ptr& caster, const osg::Vec3f& position, float radius,
            PhysicsTaskScheduler* scheduler, PhysicsSystem* physicssystem);
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        ~Projectile() override;

        btConvexShape* getConvexShape() const { return mConvexShape; }

        void updateCollisionObjectPosition();

        bool isActive() const { return mActive.load(std::memory_order_acquire); }

        MWWorld::Ptr getTarget() const;

        MWWorld::Ptr getCaster() const;
        void setCaster(const MWWorld::Ptr& caster);
        const btCollisionObject* getCasterCollisionObject() const { return mCasterColObj; }

<<<<<<< HEAD
        void setHitWater()
        {
            mHitWater = true;
        }

        bool getHitWater() const
        {
            return mHitWater;
        }
=======
        void setHitWater() { mHitWater = true; }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

        bool getHitWater() const { return mHitWater; }

        void hit(const btCollisionObject* target, btVector3 pos, btVector3 normal);

        void setValidTargets(const std::vector<MWWorld::Ptr>& targets);
        bool isValidTarget(const btCollisionObject* target) const;

<<<<<<< HEAD
        btVector3 getHitPosition() const
        {
            return mHitPosition;
        }
=======
        btVector3 getHitPosition() const { return mHitPosition; }
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7

    private:
        std::unique_ptr<btCollisionShape> mShape;
        btConvexShape* mConvexShape;

<<<<<<< HEAD
        std::unique_ptr<btCollisionObject> mCollisionObject;
        bool mTransformUpdatePending;
        bool mHitWater;
        std::atomic<bool> mActive;
        MWWorld::Ptr mCaster;
        MWWorld::Ptr mHitTarget;
        osg::Vec3f mPosition;
=======
        bool mHitWater;
        std::atomic<bool> mActive;
        MWWorld::Ptr mCaster;
        const btCollisionObject* mCasterColObj;
        const btCollisionObject* mHitTarget;
>>>>>>> 8a33edd64a6f0e9fe3962c88618e8b27aad1b7a7
        btVector3 mHitPosition;
        btVector3 mHitNormal;

        std::vector<const btCollisionObject*> mValidTargets;

        mutable std::mutex mMutex;

        PhysicsSystem* mPhysics;
        PhysicsTaskScheduler* mTaskScheduler;

        Projectile(const Projectile&);
        Projectile& operator=(const Projectile&);
    };

}

#endif
