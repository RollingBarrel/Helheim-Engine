#include "ModuleRender.h"
#include "Application.h"
#include "ModuleScene.h"
#include "MeshRendererComponent.h"
#include "ModuleProgram.h"
#include "glew.h"


ModuleRender::ModuleRender()
{
}

ModuleRender::~ModuleRender()
{
}

bool ModuleRender::Init()
{

    return true;
}

update_status ModuleRender::PreUpdate()
{

    return UPDATE_CONTINUE;
}

void ModuleRender::AddToRenderList(GameObject* root)
{
    mRenderList.push_back(root);
}

void ModuleRender::GenerateRenderList(GameObject* root) 
{
    // if engine slows down there is an optimization 
    // HERE on getMeshRenderer
    if (root->getMeshRenderer()) 
    { 
        AddToRenderList(root);
    }
    for (GameObject* child : root->GetChildren())
    {
            GenerateRenderList(child);
    }
}

void ModuleRender::DrawRenderList()
{
    for (GameObject* objectToRender: mRenderList)
    {
        //Pass model matrix
        float4x4 model = objectToRender->GetWorldTransform();
        auto program = App->GetProgram()->GetProgramID("default");
        glUseProgram(program);
        glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
        //Render
        objectToRender->getMeshRenderer()->Draw();
    }
}

update_status ModuleRender::Update()
{
    GameObject* root = App->GetScene()->GetRoot();
    mRenderList.clear();
    GenerateRenderList(root);
    DrawRenderList();
    return UPDATE_CONTINUE;
}

update_status ModuleRender::PostUpdate()
{

    return UPDATE_CONTINUE;
}

bool ModuleRender::CleanUp()
{
    return true;
}