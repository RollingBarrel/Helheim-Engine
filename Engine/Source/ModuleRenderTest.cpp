#include "ModuleRenderTest.h"
#include "Application.h"
#include "glew.h"
#include "ModuleWindow.h"
#include "ModuleCamera.h"
#include "MathGeoLib.h"
#include "ModuleDebugDraw.h"



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
