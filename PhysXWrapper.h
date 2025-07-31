#pragma once

#define PX_PHYSX_STATIC_LIB
#include <physx/PxPhysicsAPI.h>
#include <stdexcept>

using namespace physx;

class PhysXWrapper
{
public:
    PhysXWrapper();
    ~PhysXWrapper();

    static PhysXWrapper* Get();

    PxPhysics* GetgPhysics();
    PxMaterial* GetgMaterial();
    PxScene* GetgScene();


    void StepSimulation(float deltaTime);

    PxScene* GetScene() const { return mScene; }
    PxPhysics* GetPhysics() const { return mPhysics; }

    PxRigidDynamic* CreateDynamicActor(const physx::PxVec3& position, float mass = 1.0f);

private:
   PxDefaultAllocator       mAllocator;
   PxDefaultErrorCallback   mErrorCallback;
   PxFoundation* mFoundation = nullptr;
   PxPhysics* mPhysics = nullptr;
   PxScene* mScene = nullptr;
   PxCpuDispatcher* mDispatcher = nullptr;
   PxMaterial* mMaterial = nullptr;
};



/*
    // declare vars
    physx::PxDefaultAllocator		 mDefaultAllocatorCallback;
    physx::PxDefaultErrorCallback	 mDefaultErrorCallback;
    physx::PxDefaultCpuDispatcher*	 mDispatcher = NULL;
    physx::PxTolerancesScale		 mToleranceScale;
    physx::PxFoundation*		     mFoundation = NULL;
    physx::PxPhysics*			     mPhysics = NULL;
    physx::PxScene*				     mScene = NULL;
    physx::PxMaterial*			     mMaterial = NULL;
    physx::PxPvd*				     mPvd = NULL;

    // init physx
    void InitPhysX()
    {
        mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mDefaultAllocatorCallback, mDefaultErrorCallback);
        if (!mFoundation) throw std::runtime_error("PxCreateFoundation failed");

        mPvd = PxCreatePvd(*mFoundation);
        PxPvdTransport* transport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
        mPvd->connect(*transport, PxPvdInstrumentationFlag::eALL);

        mToleranceScale.length = 100; // typical length of object
        mToleranceScale.speed = 981; // typical speed of object
        mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, mToleranceScale, true, mPvd);

        //if (!gPhysics) throw std::runtime_error("PxCreatePhysics failed");

        //gDispatcher = PxDefaultCpuDispatcherCreate(2);

        PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
        sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);
        mDispatcher = PxDefaultCpuDispatcherCreate(2);

        sceneDesc.cpuDispatcher = mDispatcher;
        sceneDesc.filterShader = PxDefaultSimulationFilterShader;

        mScene = mPhysics->createScene(sceneDesc);

        // PVD
        PxPvdSceneClient* pvdClient = mScene->getScenePvdClient();
        if ( pvdClient )
        {
            pvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true );
            pvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_CONTACTS, true );
            pvdClient->setScenePvdFlag( PxPvdSceneFlag::eTRANSMIT_SCENEQUERIES, true );
        }
        // Simulation

        mMaterial = mPhysics->createMaterial( 0.5f, 0.5f, 0.6f );
        PxRigidStatic* groundPlane = PxCreatePlane( *mPhysics, PxPlane(0,1,0,99), *mMaterial);
        mScene->addActor( *groundPlane );

        // run simulation
        while (1)
        {
            mScene->simulate( 1.0f/ 144.0f );
            mScene->fetchResults(true);
        }
    }
*/