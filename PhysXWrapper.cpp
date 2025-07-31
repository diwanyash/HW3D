// PhysXWrapper.cpp
#include "PhysXWrapper.h"
#include <stdexcept>
#include <physX/PxPhysicsAPI.h> // Includes everything needed

using namespace physx;

PhysXWrapper::PhysXWrapper()
{
    mFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, mAllocator, mErrorCallback);
    if (!mFoundation) throw std::runtime_error("PxCreateFoundation failed");

    mPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *mFoundation, PxTolerancesScale());
    if (!mPhysics) throw std::runtime_error("PxCreatePhysics failed");

    PxSceneDesc sceneDesc(mPhysics->getTolerancesScale());
    sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

    mDispatcher = PxDefaultCpuDispatcherCreate(2); // PxCpuDispatcher*
    sceneDesc.cpuDispatcher = mDispatcher;
    sceneDesc.filterShader = PxDefaultSimulationFilterShader;

    mScene = mPhysics->createScene(sceneDesc);
    if (!mScene) throw std::runtime_error("Failed to create PhysX scene");

    mMaterial = mPhysics->createMaterial(0.5f, 0.5f, 0.6f);
}

PhysXWrapper::~PhysXWrapper()
{
    if (mScene) mScene->release();
    if (mDispatcher) static_cast<physx::PxDefaultCpuDispatcher*>(mDispatcher)->release();
    if (mPhysics) mPhysics->release();
    if (mFoundation) mFoundation->release();
}

PxPhysics* PhysXWrapper::GetgPhysics()
{
    return mPhysics;
}

PxMaterial* PhysXWrapper::GetgMaterial()
{
    return mMaterial;
}

PxScene* PhysXWrapper::GetgScene()
{
    return mScene;
}

PhysXWrapper* PhysXWrapper::Get()
{
    static PhysXWrapper instance;
    return &instance;
}

void PhysXWrapper::StepSimulation(float deltaTime)
{
    if (mScene)
    {
        mScene->simulate(deltaTime);
        mScene->fetchResults(true);
    }
}

PxRigidDynamic* PhysXWrapper::CreateDynamicActor(const physx::PxVec3& position, float mass)
{
    physx::PxTransform transform(position);
    physx::PxRigidDynamic* body = mPhysics->createRigidDynamic(transform);
    if (!body) throw std::runtime_error("Failed to create dynamic actor");

    body->attachShape(*mPhysics->createShape(
        physx::PxSphereGeometry(1.0f), // For example, a sphere shape
        *mMaterial
    ));
    physx::PxRigidBodyExt::updateMassAndInertia(*body, mass);
    mScene->addActor(*body);
    return body;
}
