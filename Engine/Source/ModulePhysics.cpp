#include "ModulePhysics.h"
#include "GameObject.h"
#include "Collider.h"
#include "BoxColliderComponent.h"

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
	delete(mWorld);
	delete(mConstraintSolver);
	delete(mBroadPhase);
	delete(mDispatcher);
	delete(mCollisionConfiguration);

	return true;
}

update_status ModulePhysics::PreUpdate(float dt)
{
	for (btRigidBody* rigidBody : mRigidBodiesToRemove) {
		mWorld->removeCollisionObject(rigidBody);
		//btCollisionShape* shape = rigidBody->getCollisionShape();
		//delete shape;
		delete rigidBody;
	}
	mRigidBodiesToRemove.clear();

	// TODO: if game is running
	if (true)
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
	/*boxCollider->motionState = MotionState(boxCollider, boxCollider->centerOffset, boxCollider->freezeRotation);
	boxCollider->rigidBody = AddBoxBody(&boxCollider->motionState, boxCollider->size / 2, boxCollider->colliderType == ColliderType::DYNAMIC ? boxCollider->mass : 0);
	boxCollider->rigidBody->setUserPointer(&boxCollider->col);
	AddBodyToWorld(boxCollider->rigidBody, boxCollider->colliderType, boxCollider->layer);*/
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

//void ModulePhysics::AddBodyToWorld(btRigidBody* rigidbody, ColliderType colliderType, WorldLayers layer)
//{
//
//}

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

void ModulePhysics::ProcessCollision(Collider* bodyA, Collider* bodyB, const float3& collisionNormal, const float3& diff)
{
	if (bodyA->tid == typeid(Component))
	{
		Component* pbodyA = (Component*)bodyA->collider;
		switch (pbodyA->GetType())
		{
		case ComponentType::BOXCOLLIDER:
		{
			BoxColliderComponent* boxCollider = (BoxColliderComponent*)pbodyA;
			if (bodyB->tid == typeid(Component))
			{
				Component* pbodyB = (Component*)bodyB->collider;
				boxCollider->OnCollision(pbodyB->GetOwner(), collisionNormal, diff);
			}
			break;
		}
		default:
			break;
		}
	}
}
