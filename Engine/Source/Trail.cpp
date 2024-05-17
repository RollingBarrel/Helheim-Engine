#include "Trail.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "ResourceTexture.h"
#include "ModuleResource.h"
#include "glew.h"

#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define COLOR_LOCATION 2

#define VBO_FLOAT_SIZE 9 // number of floats for every vertex 3 (position) + 2 (texcoord) + 4 (color)

Trail::Trail()
{
}

Trail::Trail(const Trail& original): mPoints(original.mPoints), mGradient(ColorGradient(original.mGradient)), mWidth(original.mWidth)
{

}

Trail::~Trail()
{
    App->GetOpenGL()->RemoveTrail(this);
}


void Trail::Init()
{
    // set up mesh and attribute properties
    //float trailMesh[] = {
    ////  position            tex coord   color
    //    0.0f,-0.5f,0.0f,    0.0f,0.0f,  1.0f,0.0f,0.0f,1.0f,
    //    0.0f, 0.5f,0.0f,    0.0f,1.0f,  1.0f,0.0f,0.0f,1.0f,
    //    0.5f, 0.5f,0.0f,    0.5f,1.0f,  0.0f,1.0f,0.0f,1.0f,
    //    0.5f,-0.5f,0.0f,    0.5f,0.0f,  0.0f,1.0f,0.0f,1.0f,
    //    1.0f, 0.5f,0.0f,    1.0f,1.0f,  0.0f,0.0f,1.0f,1.0f,
    //    1.0f,-0.5f,0.0f,    1.0f,0.0f,  0.0f,0.0f,1.0f,1.0f
    //};
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(trailMesh), trailMesh, GL_DYNAMIC_DRAW);

    // Enable the attribute for vertex positions
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    // Enable the attribute for texture coordinates
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    // Enable the attribute for texture coordinates
    glEnableVertexAttribArray(COLOR_LOCATION);
    glVertexAttribPointer(COLOR_LOCATION, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    App->GetOpenGL()->AddTrail(this);
}


void Trail::Update()
{
    mTrailTime += App->GetDt();
    if (mMaxLifeTime > 0 && (mTrailTime - mPoints.front().creationTime) >= mMaxLifeTime)
    {
        mPoints.pop_front();
    }
}

void Trail::Draw() const
{
    if (mPoints.size() <= 1) return;
    unsigned int programId = App->GetOpenGL()->GetTrailProgramId();
    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);									// Enable Blending
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Type Of Blending To Perform
    //glEnable(GL_TEXTURE_2D);
    //glBlendEquation(GL_FUNC_ADD);
    auto cam = (const CameraComponent*)App->GetCamera()->GetCurrentCamera();
    float4x4 projection = cam->GetViewProjectionMatrix();
    float3 up = cam->GetFrustum().up;
    float3 norm = cam->GetFrustum().front;
    float3 right = up.Cross(norm).Normalized();
    glUseProgram(programId);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, mPoints.size() * 2 * VBO_FLOAT_SIZE * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
    byte* ptr = static_cast<byte*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
    int testSize = 0;
    float3 lastPoint = mPoints[1].position;

    for (int i = 0; i < mPoints.size(); ++i)
    {
        float dp = static_cast<float>(i) / (mPoints.size() - 1);
        float3 direction = (mPoints[i].position - lastPoint).Normalized();
        lastPoint = mPoints[i].position;
        if (i > 0)
        {
            ptr += sizeof(float4);
        }
        float3 billboardDirection = (direction.Cross(norm)) + (direction.Cross(right).Length())*up;
        billboardDirection.Normalize();
        float3 topPointPos = mPoints[i].position + billboardDirection * mWidth.GetValue(dp) * 0.5f;
        float3 botPointPos = mPoints[i].position - billboardDirection * mWidth.GetValue(dp) * 0.5f;
        float2 topPointTexCoord = float2(dp, 1);
        float2 botPointTexCoord = float2(dp, 0);
        float4 color = mGradient.CalculateColor(dp);

        // Copiar botPoint
        memcpy(ptr, botPointPos.ptr(), sizeof(float3));
        ptr += sizeof(float3);
        memcpy(ptr, botPointTexCoord.ptr(), sizeof(float2));
        ptr += sizeof(float2);
        memcpy(ptr, color.ptr(), sizeof(float4));
        ptr += sizeof(float4);

        // Copiar topPoint
        memcpy(ptr, topPointPos.ptr(), sizeof(float3));
        ptr += sizeof(float3);
        memcpy(ptr, topPointTexCoord.ptr(), sizeof(float2));
        ptr += sizeof(float2);
        memcpy(ptr, color.ptr(), sizeof(float4));
    }

    glUnmapBuffer(GL_ARRAY_BUFFER);
    glBindVertexArray(mVAO);

    glUniformMatrix4fv(glGetUniformLocation(programId, "viewProj"), 1, GL_TRUE, &projection[0][0]);
    glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

    glDrawArrays(GL_TRIANGLE_STRIP, 0, mPoints.size() * 2);

    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
    glEnable(GL_CULL_FACE);
}

void Trail::AddTrailPositions(float3 position, Quat rotation)
{
    float3 direction = float3::unitY;
    mPoints.push_back(TrailPoint{ position, direction, mTrailTime });
}

float3 Trail::GetLastPosition() const
{
    if (mPoints.empty()) 
    {
        return float3::nan;
    }
    return mPoints.back().position;
}

float3 Trail::GetFirstPosition() const
{
    if (mPoints.empty())
    {
        return float3::nan;
    }
    return mPoints.front().position;
}

void Trail::SaveJson(Archive& archive) const
{
    archive.AddFloat("Max Life Time", mMaxLifeTime);
    Archive width;
    mWidth.SaveJson(width);
    archive.AddObject("Width", width);
    mGradient.Save(archive);
}

void Trail::LoadJson(const rapidjson::Value& data)
{
    if (data.HasMember("Max Life Time") && data["Max Life Time"].IsFloat())
    {
        mMaxLifeTime = data["Max Life Time"].GetFloat();
    }
    if (data.HasMember("Color Gradient") && data["Color Gradient"].IsArray())
    {
        mGradient.LoadFromJSON(data);
    }
    if (data.HasMember("Width") && data["Width"].IsObject())
    {
        mWidth.LoadJson(data["Width"]);
    }
}
