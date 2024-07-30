#include "Physics.h"
#include "Application.h"
#include "ModuleCamera.h"
#include "ModuleWindow.h"
#include "ModuleScene.h"
#include "ModulePhysics.h"

#include "CameraComponent.h"

#include "Geometry/Ray.h"
#include "Geometry/LineSegment.h"
#include "float2.h"

void Physics::Raycast(std::multiset<Hit>& hits, const Ray& ray, float maxDistance, std::vector<std::string>* ignoreTags)
{
	float3 from = float3(ray.pos);
	float3 to = float3(ray.pos + ray.dir * maxDistance);

	App->GetPhysics()->RayCast(from, to, hits, ignoreTags);
}

ENGINE_API void Physics::Raycast(Hit& hit, const Ray& ray, float maxDistance, std::vector<std::string>* ignoreTags)
{
	float3 from = float3(ray.pos);
	float3 to = float3(ray.pos + ray.dir * maxDistance);

	App->GetPhysics()->RayCast(from, to, hit, ignoreTags);
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