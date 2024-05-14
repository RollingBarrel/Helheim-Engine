#include "Trail.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "ResourceTexture.h"
#include "glew.h"

#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define COLOR_LOCATION 2

#define VBO_FLOAT_SIZE 9 // number of floats for every vertex 3 (position) + 2 (texcoord) + 4 (color)

Trail::Trail()
{

    mPoints.push_back(TrailPoint(position, direcction));
}

Trail::~Trail()
{
}


void Trail::Init()
{
    // set up mesh and attribute properties
    float trailMesh[] = {
    //  position            tex coord   color
        0.0f,-0.5f,0.0f,    0.0f,0.0f,  1.0f,0.0f,0.0f,1.0f,
        0.0f, 0.5f,0.0f,    0.0f,1.0f,  1.0f,0.0f,0.0f,1.0f,
        0.5f, 0.5f,0.0f,    0.5f,1.0f,  0.0f,1.0f,0.0f,1.0f,
        0.5f,-0.5f,0.0f,    0.5f,0.0f,  0.0f,1.0f,0.0f,1.0f,
        1.0f, 0.5f,0.0f,    1.0f,1.0f,  0.0f,0.0f,1.0f,1.0f,
        1.0f,-0.5f,0.0f,    1.0f,0.0f,  0.0f,0.0f,1.0f,1.0f
    };
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(trailMesh), trailMesh, GL_DYNAMIC_DRAW); // Probably this wont be static at the end.

    // Enable the attribute for vertex positions
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    // Enable the attribute for texture coordinates
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    // Enable the attribute for texture coordinates
    glEnableVertexAttribArray(COLOR_LOCATION);
    glVertexAttribPointer(COLOR_LOCATION, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));

    App->GetOpenGL()->AddTrail(this);
}


void Trail::Update()
{
}

void Trail::Draw()
{
    unsigned int programId = App->GetOpenGL()->GetTrailProgramId();
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);									// Enable Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Type Of Blending To Perform
    //glEnable(GL_TEXTURE_2D);
    //glBlendEquation(GL_FUNC_ADD);
    glUseProgram(programId);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mPoints.size() * VBO_FLOAT_SIZE * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    float* ptr = (float*)(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

    for (int i = 0; i < mPoints.size(); ++i)
    {
        float3 topPointPos = mPoints[i].TopPointPosition();
        float3 botPointPos = mPoints[i].BotPointPosition();
        float2 topPointTexCoord = mPoints[i].TopPointTexCoords();
        float2 botPointTexCoord = mPoints[i].BotPointTexCoords();
        float4 color = mPoints[i].Color();

        // Copiar topPoint
        memcpy(ptr, topPointPos.ptr(), sizeof(float3));
        ptr += sizeof(float3);
        memcpy(ptr, topPointTexCoord.ptr(), sizeof(float2));
        ptr += sizeof(float2);
        memcpy(ptr, color.ptr(), sizeof(float4));
        ptr += sizeof(float4);

        // Copiar botPoint
        memcpy(ptr, botPointPos.ptr(), sizeof(float3));
        ptr += sizeof(float3);
        memcpy(ptr, botPointTexCoord.ptr(), sizeof(float2));
        ptr += sizeof(float2);
        memcpy(ptr, color.ptr(), sizeof(float4));
        ptr += sizeof(float4);
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindVertexArray(mVAO);

    CameraComponent* cam = (CameraComponent*)App->GetCamera()->GetCurrentCamera();
    float4x4 projection = cam->GetViewProjectionMatrix();

    glUniformMatrix4fv(glGetUniformLocation(programId, "viewProj"), 1, GL_TRUE, &projection[0][0]);
    glUniformMatrix4fv(glGetUniformLocation(programId, "model"), 1, GL_TRUE, mModel.ptr());
    glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, mPoints.size() * 2);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glUseProgram(0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void Trail::AddTrailPositions(float3 position, float3 rotation)
{
}

void Trail::Save(Archive& archive) const
{
}

void Trail::LoadFromJSON(const rapidjson::Value& data)
{
}
