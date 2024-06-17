#include "ModulePhysics.h"
#include "Application.h"
#include "ModuleScriptManager.h"

#include "GameObject.h"
#include "BoxColliderComponent.h"

#include "MotionState.h"
#include "btBulletDynamicsCommon.h"

ModulePhysics::ModulePhysics()
{
}

ModulePhysics::~ModulePhysics()
{
}

bool ModulePhysics::Init()
{
	mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
	mBroadPhase = new btDbvtBroadphase();
	mConstraintSolver = new btSequentialImpulseConstraintSolver();
	mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadPhase, mConstraintSolver, mCollisionConfiguration);
	mWorld->setGravity(btVector3(0.f, mGravity, 0.f));

	return true;
}

bool ModulePhysics::CleanUp()
{
	delete mWorld;
	delete mBroadPhase;
	
	delete mConstraintSolver;
	delete mDispatcher;
	delete mCollisionConfiguration;
	
	return true;
}

update_status ModulePhysics::PreUpdate(float dt)
{
	for (btRigidBody* rigidBody : mRigidBodiesToRemove)
	{
		mWorld->removeCollisionObject(rigidBody);
		btCollisionShape* shape = rigidBody->getCollisionShape();
		delete shape;
		delete rigidBody;
	}
	mRigidBodiesToRemove.clear();

	
	mWorld->stepSimulation(dt, 15);

	int numManifolds = mWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
		btCollisionObject* obA = (btCollisionObject*)(contactManifold->getBody0());
		btCollisionObject* obB = (btCollisionObject*)(contactManifold->getBody1());

		int numContacts = contactManifold->getNumContacts();
		if (numContacts > 0)
		{
			Collider* bodyA = (Collider*)obA->getUserPointer();
			Collider* bodyB = (Collider*)obB->getUserPointer();

			if (bodyA && bodyB)
			{
				float3 contactOnA = float3(contactManifold->getContactPoint(0).getPositionWorldOnA());
				float3 contactOnB = float3(contactManifold->getContactPoint(0).getPositionWorldOnB());

				float3 diff = contactOnB - contactOnA;
				float3 collisionNormal = float3(contactManifold->getContactPoint(0).m_normalWorldOnB);

				ProcessCollision(bodyA, bodyB, collisionNormal, diff);
				ProcessCollision(bodyB, bodyA, -collisionNormal, -diff);
			}
		}
	}
	
	return UPDATE_CONTINUE;
}

void ModulePhysics::RayCast(float3 from, float3 to, std::multiset<Hit>& hits)
{
	btVector3 fromBt(from.x, from.y, from.z);
	btVector3 toBt(to.x, to.y, to.z);
	btCollisionWorld::AllHitsRayResultCallback callback(fromBt, toBt);
	mWorld->rayTest(fromBt, toBt, callback);
	
	if (callback.hasHit())
	{
		for (int i = 0; i < callback.m_collisionObjects.size(); ++i)
		{
			LOG("RayCast %i", i);
			Collider* collider = reinterpret_cast<Collider*>(callback.m_collisionObjects[i]->getUserPointer());
			if (collider)
			{
				Component* component = (Component*)collider->mCollider;
				if (component)
				{
					Hit hit;
					float3 hitPoint = float3(callback.m_hitPointWorld[i].x(), callback.m_hitPointWorld[i].y(), callback.m_hitPointWorld[i].z());

					hit.mDistance = from.Distance(hitPoint);
					hit.mGameObject = component->GetOwner();;
					hit.mHitPoint = float3(callback.m_hitPointWorld[i].x(), callback.m_hitPointWorld[i].y(), callback.m_hitPointWorld[i].z());

					hits.insert(hit);
					LOG("RayCast %i", i);
				}
			}
		}
	}	
}

void ModulePhysics::CreateBoxRigidbody(BoxColliderComponent* boxCollider)
{
	// Set up the motion state for the box collider
	boxCollider->SetMotionState(new MotionState(boxCollider, boxCollider->GetCenter(), boxCollider->GetFreezeRotation()));

	// Calculate the size (half extents) for the box shape
	float3 boxSize = boxCollider->GetSize();

	// Create the collision shape for the box
	btCollisionShape* collisionShape = new btBoxShape(btVector3(boxSize.x, boxSize.y, boxSize.z));

	// Define the mass and local inertia for the rigid body
	float mass = 0.0f; // You might want to adjust this based on whether the object is static or dynamic
	btVector3 localInertia(0, 0, 0);
	if (mass != 0.0f)
	{
		collisionShape->calculateLocalInertia(mass, localInertia);
	}

	// Set up the construction info for the rigid body
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, boxCollider->GetMotionState(), collisionShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	// Set the user pointer to the collider for later collision processing
	body->setUserPointer((void*)(boxCollider->GetCollider()));

	// Add the rigid body to the box collider
	boxCollider->SetRigidBody(body);

	// Add the rigid body to the physics world
	AddBodyToWorld(body, boxCollider->GetColliderType());
}

void ModulePhysics::RemoveBoxRigidbody(BoxColliderComponent* boxCollider)
{
	if (boxCollider->GetRigidBody())
	{
		mRigidBodiesToRemove.push_back(boxCollider->GetRigidBody());
		boxCollider->SetRigidBody(nullptr);
	}
}

void ModulePhysics::UpdateBoxRigidbody(BoxColliderComponent* boxCollider)
{
	RemoveBoxRigidbody(boxCollider);
	CreateBoxRigidbody(boxCollider);
}

btRigidBody* ModulePhysics::AddBoxBody(btMotionState* motionState, float3 size, float mass)
{
	btCollisionShape* collisionShape = new btBoxShape(btVector3(size.x, size.y, size.z));

	btVector3 localInertia(0, 0, 0);
	if (mass != 0.f) 
	{
		collisionShape->calculateLocalInertia(mass, localInertia);
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	return body;
}

void ModulePhysics::AddBodyToWorld(btRigidBody* rigidbody, ColliderType colliderType)
{
	// Configure collision flags based on the collider type
	switch (colliderType)
	{
	case ColliderType::STATIC:
		rigidbody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		break;
	case ColliderType::DYNAMIC:
		rigidbody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidbody->setActivationState(DISABLE_DEACTIVATION);
		break;
	case ColliderType::TRIGGER:
		rigidbody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		break;
	default:
		break;
	}

	// All objects should collide with all other objects
	short group = 1;
	short mask = 1;

	// Add the rigid body to the physics world with the appropriate layer and collision mask
	mWorld->addRigidBody(rigidbody, group, mask);
}

void ModulePhysics::ProcessCollision(Collider* bodyA, Collider* bodyB, const float3& collisionNormal, const float3& diff)
{
	// TODO: Distinguish event types
	Component* componentA = (Component*)bodyA->mCollider;

	switch (bodyA->mTypeId)
	{
		case ComponentType::BOXCOLLIDER:
		{
			BoxColliderComponent* boxColliderA = (BoxColliderComponent*)componentA;

			switch (bodyB->mTypeId) 
			{
				case ComponentType::BOXCOLLIDER:
					BoxColliderComponent* boxColliderB = (BoxColliderComponent*)bodyB->mCollider;
					CollisionData collisionData{ CollisionEventType::ON_COLLISION_ENTER, boxColliderB->GetOwner(), collisionNormal, diff };
					boxColliderA->OnCollision(&collisionData);
					break;
			}
			break;
		}
		default:
			break;
	}
	
}
