#include "ModulePhysics.h"
#include "Collider.h"
#include "GameObject.h"
#include "BoxColliderComponent.h"

#include "btBulletDynamicsCommon.h"

ModulePhysics::ModulePhysics()
{
	Init();
}

ModulePhysics::~ModulePhysics()
{
	delete(world);
	delete(constraintSolver);
	delete(broadPhase);
	delete(dispatcher);
	delete(collisionConfiguration);
}

bool ModulePhysics::Init()
{
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	broadPhase = new btDbvtBroadphase();
	constraintSolver = new btSequentialImpulseConstraintSolver();
	world = new btDiscreteDynamicsWorld(dispatcher, broadPhase, constraintSolver, collisionConfiguration);
	world->setGravity(btVector3(0.f, mGravity, 0.f));

	return true;
}

update_status ModulePhysics::PreUpdate(float dt)
{
	// TODO: if game is running
	int numManifolds = world->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; i++)
	{
		btPersistentManifold* contactManifold = world->getDispatcher()->getManifoldByIndexInternal(i);
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
			else
			{
				//ComponentParticleSystem::Particle* pbodyB = (ComponentParticleSystem::Particle*)bodyB->collider;
				//boxCollider->OnCollision(pbodyB->emitter->GetOwner(), collisionNormal, diff, pbodyB);
			}
			break;
		}
		default:
			break;
		}
	}
}
