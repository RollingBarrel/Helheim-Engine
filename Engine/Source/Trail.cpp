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

Trail::Trail(const Trail& original): mPoints(original.mPoints), mGradient(ColorGradient(original.mGradient)), mWidth(original.mWidth), 
mFixedDirection(original.mFixedDirection), mTrailTime(original.mTrailTime), mDirection(original.mDirection), mMaxLifeTime(original.mMaxLifeTime)
{

}

Trail::~Trail()
{
    App->GetOpenGL()->RemoveTrail(this);
}


void Trail::Init()
{
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);

    // Enable the attribute for vertex positions
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    // Enable the attribute for texture coordinates
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    // Enable the attribute for vertex color
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
    float3 nextPoint = mPoints[1].position;
    float3 direction;
    for (int i = 0; i < mPoints.size(); ++i)
    {
        float dp = 1 - (static_cast<float>(i) / (mPoints.size() - 1));
        // The last point uses the direction of the previous
        // because there is no next point to calculate the direction
        if (!mFixedDirection and i < mPoints.size()-1)
        {
            nextPoint = mPoints[i + 1].position;
            direction = (nextPoint - mPoints[i].position).Normalized();
            direction = direction.Cross(norm);
            direction.Normalize();
        }
        else if(mFixedDirection)
        {
            direction = mPoints[i].direction.Normalized();
        }

        float3 topPointPos = mPoints[i].position + direction * mWidth.GetValue(dp) * 0.5f;
        float3 botPointPos = mPoints[i].position - direction * mWidth.GetValue(dp) * 0.5f;
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
        ptr += sizeof(float4);
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
    rotation.Normalize();
    Quat rotatedQuat = rotation * Quat(0, mDirection.x, mDirection.y, mDirection.z) * rotation.Inverted();
    float3 rotatedVector(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
    rotatedVector.Normalize();
    mPoints.push_back(TrailPoint{ position, rotatedVector, mTrailTime});
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

void Trail::Save(JsonObject& obj) const
{
    obj.AddFloat("Max Life Time", mMaxLifeTime);

    obj.AddNewJsonObject("Width");
    mWidth.Save(obj);

    obj.AddNewJsonObject("Gradient");
    mGradient.Save(obj);
}

void Trail::Load(const JsonObject& data)
{
    mMaxLifeTime = data.GetFloat("Max Life Time");

    JsonObject width = data.GetJsonObject("Width");
    mWidth.Load(width);

    JsonObject gradient = data.GetJsonObject("Gradient");
    mWidth.Load(gradient);
}
