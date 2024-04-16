#include "Physics.h"
#include "Application.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "Geometry/Ray.h"

std::map<float, GameObject*> Physics::Raycast(Ray* ray)
{
    return App->GetScene()->GetQuadtreeRoot()->RayCast(ray);
}
