#include "ModulePhysics.h"
#include "ModuleScriptManager.h"
#include "BoxColliderComponent.h"
#include "Application.h"
#include "GameObject.h"
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
	/*
	for (int i = mWorld->getNumCollisionObjects() - 1; i >= 0; i--)
	{
		btCollisionObject* obj = mWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body)
		{
			if (body->getCollisionShape())
			{
				delete body->getCollisionShape();
			}
			if (body->getMotionState())
			{
				delete body->getMotionState();
			}
		}
		mWorld->removeCollisionObject(obj);
		delete obj;
	}
	*/
	delete(mWorld);
	delete(mConstraintSolver);
	delete(mBroadPhase);
	delete(mDispatcher);
	delete(mCollisionConfiguration);
	
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

	if (false) // TODO: Check game is running App->GetScriptManager()->IsPlaying()
	{
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
	}

	return UPDATE_CONTINUE;
}

void ModulePhysics::CreateBoxRigidbody(BoxColliderComponent* boxCollider)
{
	boxCollider->SetMotionState(new MotionState(boxCollider, boxCollider->GetCenter(), boxCollider->GetFreezeRotation()));
	btRigidBody* rigidbody = AddBoxBody(boxCollider->GetMotionState(), boxCollider->GetSize() / 2.0f, 0.f);
	rigidbody->setUserPointer(boxCollider->GetCollider().mCollider);
	boxCollider->SetRigidBody(rigidbody);
	// TODO: Define world layers, collider types
	AddBodyToWorld(boxCollider->GetRigidBody(), ColliderType::STATIC, WorldLayers::COUNT);
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
	if (mass != 0.f) {
		collisionShape->calculateLocalInertia(mass, localInertia);
	}

	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, collisionShape, localInertia);
	btRigidBody* body = new btRigidBody(rbInfo);

	return body;
}

void ModulePhysics::AddBodyToWorld(btRigidBody* rigidbody, ColliderType colliderType, WorldLayers layer)
{
	switch (colliderType)
	{
	case ColliderType::STATIC:
		rigidbody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
		break;
	case ColliderType::KINEMATIC:
		rigidbody->setCollisionFlags(btCollisionObject::CF_KINEMATIC_OBJECT);
		rigidbody->setActivationState(DISABLE_DEACTIVATION);
		break;
	case ColliderType::TRIGGER:
		rigidbody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
		break;
	default:
		break;
	}

	int collisionMask = 0;
	switch (layer)
	{
	default: //NO_COLLISION
		collisionMask = 0;
		break;
	}

	mWorld->addRigidBody(rigidbody, (short)layer, collisionMask);
}

void ModulePhysics::ProcessCollision(Collider* bodyA, Collider* bodyB, const float3& collisionNormal, const float3& diff)
{
	if (bodyA->mTypeId == typeid(Component))
	{
		Component* pbodyA = (Component*)bodyA->mCollider;
		switch (pbodyA->GetType())
		{
		case ComponentType::BOXCOLLIDER:
		{
			BoxColliderComponent* boxCollider = (BoxColliderComponent*)pbodyA;
			if (bodyB->mTypeId == typeid(Component))
			{
				Component* pbodyB = (Component*)bodyB->mCollider;
				// TODO: Distinguish event types
				CollisionData collisionData{ CollisionEventType::ON_COLLISION_ENTER, pbodyB->GetOwner(), collisionNormal, diff };
				boxCollider->OnCollision(&collisionData);
			}
			break;
		}
		default:
			break;
		}
	}
}
