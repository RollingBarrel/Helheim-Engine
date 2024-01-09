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

update_status ModuleRenderTest::Update()
{
    float4x4 viewproj = App->GetCamera()->GetProjectionMatrix() * App->GetCamera()->GetViewMatrix();
    App->GetDebugDraw()->Draw(viewproj, App->GetWindow()->GetWidth(), App->GetWindow()->GetHeight());

    return UPDATE_CONTINUE;
}
