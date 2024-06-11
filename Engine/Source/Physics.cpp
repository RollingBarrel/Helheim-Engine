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

void Physics::Raycast(const Ray& ray, std::map<float, Hit>&  hits)
{
    return App->GetScene()->GetQuadtreeRoot()->RayCast(ray, hits);
}

/* //JORDI SOLUTION
Ray Physics::ScreenPointToRay(float2 screenPoint)
{
	screenPoint -= App->GetWindow()->GetGameWindowsPosition();
	screenPoint.x /= App->GetWindow()->GetGameWindowsSize().x;
	screenPoint.y /= App->GetWindow()->GetGameWindowsSize().y;
	screenPoint.y = 1.0f - screenPoint.y;
	screenPoint = screenPoint * 2.0f - float2(1.0f);
	Clamp(screenPoint, -1.0f, 1.0f);

	const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();
	if (camera)
	{
		Ray ray = camera->GetFrustum().UnProject(screenPoint);
		return ray;
	}

	return Ray();
    
}
*/

Ray Physics::ScreenPointToRay(float2 mouseGlobalPosition)
{
	Ray ray;

	float normalizedX = -1.0 + 2.0 * (float)(mouseGlobalPosition.x - App->GetWindow()->GetGameWindowsPosition().x) / (float)App->GetWindow()->GetGameWindowsSize().x;
	float normalizedY = 1.0 - 2.0 * (float)(mouseGlobalPosition.y - App->GetWindow()->GetGameWindowsPosition().y) / (float)App->GetWindow()->GetGameWindowsSize().y;

	const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();
	if (camera)
	{
		LineSegment raySegment = App->GetCamera()->GetCurrentCamera()->GetFrustum().UnProjectLineSegment(normalizedX, normalizedY);
		ray.pos = raySegment.a;
		ray.dir = (raySegment.b - raySegment.a).Normalized();

		return (ray.dir.IsNormalized()) ? ray : Ray();
	}

	return ray;

}