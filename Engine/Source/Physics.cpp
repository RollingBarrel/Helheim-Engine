#include "Physics.h"
#include "Application.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "ModuleWindow.h"
#include "Geometry/Ray.h"
#include "float2.h"
#include "Geometry/LineSegment.h"

std::map<float, GameObject*> Physics::Raycast(Ray* ray)
{
    return App->GetScene()->GetQuadtreeRoot()->RayCast(ray);
}

Ray Physics::ScreenPointToRay(float2 mousePosition)
{
	Ray ray;

	float normalizedX = -1.0 + 2.0 * (float)(mousePosition.x - App->GetWindow()->GetGameWindowsPosition().x) / (float)App->GetWindow()->GetGameWindowsSize().x;
	float normalizedY = 1.0 - 2.0 * (float)(mousePosition.y - App->GetWindow()->GetGameWindowsPosition().y) / (float)App->GetWindow()->GetGameWindowsSize().y;

	LineSegment raySegment = App->GetCamera()->GetCurrentCamera()->GetFrustum().UnProjectLineSegment(normalizedX, normalizedY);

	ray.pos = raySegment.a;
	ray.dir = (raySegment.b - raySegment.a);

    return ray;
}
