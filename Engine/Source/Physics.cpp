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

Ray Physics::ScreenPointToRay(float2 mouseGlobalPosition)
{
	Ray ray;

	float2 screenPoint(mouseGlobalPosition - App->GetWindow()->GetGameWindowsPosition());
	screenPoint.x /= App->GetWindow()->GetGameWindowsSize().x;
	screenPoint.y /= App->GetWindow()->GetGameWindowsSize().y;
	screenPoint *= 2.0f - 1.0f;

	const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();
	if (camera)
	{
		//float4 view = camera->GetFrustum().ViewMatrix() * float4(screenPoint, 1.0f, 1.0f);
		//ray.dir = (float3(view.x, view.y, view.z) - float3(0.0f)).Normalized();
		//ray.pos = camera->GetFrustum().pos;
		LineSegment raySegment = App->GetCamera()->GetCurrentCamera()->GetFrustum().UnProjectLineSegment(screenPoint.x, screenPoint.y);
		ray.pos = raySegment.a;
		ray.dir = (raySegment.b - raySegment.a).Normalized();

		return (ray.dir.IsNormalized()) ? ray : Ray();
	}
	
	return ray;
    
}
