#include "ModuleRenderTest.h"
#include "Application.h"
#include "glew.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "MathGeoLib.h"
#include "ModuleDebugDraw.h"
#include "GameObject.h"
#include "MeshRendererComponent.h"
#include "ModuleScene.h"



ModuleRenderTest::ModuleRenderTest()
{
}

ModuleRenderTest::~ModuleRenderTest()
{
}

bool ModuleRenderTest::Init()
{

    return true;
}

update_status ModuleRenderTest::PreUpdate()
{
    return UPDATE_CONTINUE;
}

update_status ModuleRenderTest::Update()
{
    float4x4 viewproj = App->GetCamera()->GetViewProjMatrix();
    App->GetDebugDraw()->Draw(viewproj, App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight());
    GameObject* root = App->GetScene()->GetRoot();
    DrawMeshRenderersRecursive(root);

    return UPDATE_CONTINUE;
}

update_status ModuleRenderTest::PostUpdate()
{
    return UPDATE_CONTINUE;

}

bool ModuleRenderTest::CleanUp()
{
    return true;
}

const void ModuleRenderTest::DrawMeshRenderersRecursive(const GameObject* object)
{
    const MeshRendererComponent* rendcomp = object->getMeshRenderer();
    if (rendcomp != nullptr)
    {
        App->GetDebugDraw()->DrawBoundingBox(rendcomp->getOBB());
    }
    for (const auto& child : object->GetChildren()) 
    {
        DrawMeshRenderersRecursive(child);
    }
    
}
