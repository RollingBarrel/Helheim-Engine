#include "pch.h"
#include "Gun.h"
#include <utility>
#include <limits.h>
#include "Globals.h"
#include "Geometry/Ray.h"
#include "GameObject.h"
#include "EnemyBase.h"
#include "ScriptComponent.h"
#include "Tag.h"

CREATE(Gun) 
{
    CLASS(owner);
    END_CREATE;
}

Gun::Gun(GameObject* owner) : Script(owner)
{
	mMaxBullets = 16.0f;
	mBulletCostPerSecond = 1;
    mDamage = 1;
    mBullets = mMaxBullets;
    mReloadTime = 1.5f;
    mChargedShotMaxTime = 20.0f;

}

void Gun::Shoot(bool isChargedShot, float chargeTime)
{
    if (isChargedShot) {
        //definir que el maximo del tiempo de carga es 20 segundos y el minimo 5 segundos
        if(chargeTime > 20) chargeTime = 20;

        int mDamage = chargeTime * 5;
        int bulletCost = chargeTime * mBulletCostPerSecond;
        if (mBullets >= bulletCost) {
            ShootLogic(mDamage);
            mBullets -= bulletCost;
            LOG("Charged shot fired. Damage:  %i", mDamage);
            LOG("Bullets:  %i", mBullets);
        }
        else {
           LOG("Not enough bullets to fire charged shot.");
        }
    }
    else {
        if (mBullets > 0) {
            ShootLogic(mDamage);
            mBullets--;
            if (mBullets == 0) {
                // Lógica para entrar en el estado de recarga
                LOG("Out of bullets! Cannot shoot.");
            }
            else {
                // Lógica para regresar al estado de reposo
                LOG("Basic shoot fire. Remining Bullets %i", mBullets);
            }
        }
        else {
            LOG("Out of bullets! Cannot shoot.");
        }
    }
}


void Gun::ShootLogic(int damage)
{
    std::map<float, GameObject*> hits;

    Ray ray;
    ray.pos = mGameObject->GetPosition();
    ray.dir = mGameObject->GetFront();

    float distance = 100.0f;
    hits = Physics::Raycast(&ray);

    Debug::DrawLine(ray.pos, ray.dir * distance, float3(255.0f, 255.0f, 255.0f));

    //log the first object hit by the ray
    if (!hits.empty()) {
        LOG("Object %s dhas been hit at distance: %f", hits.begin()->second->GetName().c_str(), hits.begin()->first);
    }

    //recorrer todos los hits y hacer daño a los objetos que tengan tag = target
    for (auto hit : hits) {
        if (hit.second->GetTag()->GetName() == "Enemy") {

            EnemyBase* enemy = (EnemyBase*)((ScriptComponent*)hit.second->GetComponent(ComponentType::SCRIPT))->GetScriptInstance();
            if (enemy != nullptr) {
                enemy->SetEnemyDamage(damage);
            }
        }
    }
}

void Gun::Reload()
{
	mBullets = mMaxBullets;
    LOG("Reloaded!Remaining bullets : % f", mBullets);
}

void Gun::Start()
{
    mMaxBullets = 16.0f;
    mBulletCostPerSecond = 1;
    mDamage = 1;
    mBullets = mMaxBullets;
    mReloadTime = 1.5f;
    mChargedShotMaxTime = 20.0f;
}

void Gun::Update()
{
	if (mBullets == 0) {
		// Lógica para entrar en el estado de recarga
		LOG("Out of bullets! Cannot shoot.");
	}
	else {
		// Lógica para regresar al estado de reposo
		LOG("Basic shoot fire. Remining Bullets %i", mBullets);
	}
}

