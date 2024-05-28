#include "Physics.h"
#include "Application.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "ModuleWindow.h"
#include "Geometry/Ray.h"
#include "float2.h"

void Physics::Raycast(const Ray& ray, std::map<float, Hit>&  hits)
{
    return App->GetScene()->GetQuadtreeRoot()->RayCast(ray, hits);
}

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
