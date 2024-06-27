#include "TrailComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleResource.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "ResourceTexture.h"
#include "ModuleResource.h"
#include "glew.h"
#include "MathGeoLib.h"

#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define COLOR_LOCATION 2

#define VBO_FLOAT_SIZE 9 // number of floats for every vertex 3 (position) + 2 (texcoord) + 4 (color)

TrailComponent::TrailComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::TRAIL)
{
    Init();
}

TrailComponent::TrailComponent(const TrailComponent& original, GameObject* owner) : Component(owner, ComponentType::TRAIL), 
mResourceId(original.mResourceId), mFileName(original.mFileName),
mMaxPoints(original.mMaxPoints), mMinDistance(original.mMinDistance),
mPoints(original.mPoints), mGradient(ColorGradient(original.mGradient)), mWidth(original.mWidth),
mFixedDirection(original.mFixedDirection), mTrailTime(original.mTrailTime), mDirection(original.mDirection), mMaxLifeTime(original.mMaxLifeTime)
{
    Init();
}

TrailComponent::~TrailComponent()
{
    App->GetOpenGL()->RemoveTrail(this);
}

Component* TrailComponent::Clone(GameObject* owner) const
{
    return new TrailComponent(*this, owner);
}

void TrailComponent::Init()
{
    mPoints.clear();
    float3 position = mOwner->GetWorldPosition();
    Quat rotationQ = mOwner->GetWorldRotation();
    float3 rotation = RotationToVector(rotationQ);
    mPoints.push_front(TrailPoint({ position, rotation, mTrailTime }));
    mPoints.push_front(TrailPoint({ position, rotation, mTrailTime }));

    SetImage(mResourceId);

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

void TrailComponent::Draw() const
{
    if (IsEnabled()) 
    {
        if (mPoints.size() <= 2) return;
        unsigned int programId = App->GetOpenGL()->GetTrailProgramId();
        glDepthMask(GL_FALSE);
        glDisable(GL_CULL_FACE);
        glEnable(GL_BLEND);									// Enable Blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Type Of Blending To Perform
        const CameraComponent* cam = App->GetCamera()->GetCurrentCamera();
        float4x4 projection = cam->GetViewProjectionMatrix();
        //float3 up = cam->GetFrustum().up;
        float3 norm = cam->GetFrustum().front;
        //float3 right = up.Cross(norm).Normalized();
        glUseProgram(programId);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, (mPoints.size() + 1) * 2 * VBO_FLOAT_SIZE * sizeof(float), nullptr, GL_DYNAMIC_DRAW);
        byte* ptr = reinterpret_cast<byte*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
        int testSize = 0;
        float3 nextPoint = mPoints[0].position;
        float3 position;
        float3 direction;
        mWidth.GetValue().CalculateInitialValue();
        for (int i = 0; i < mPoints.size()+1; ++i)
        {
            if (i < 1)
            {
                float3 scale = mOwner->GetWorldScale();
                Quat rotationQ = mOwner->GetWorldRotation();
                position = mOwner->GetWorldPosition();
                //mOwner->GetWorldTransform().Decompose(position, rotationQ, scale);
            }
            else
            {
                position = mPoints[i-1].position;
            }

            float dp = (static_cast<float>(i) / (mPoints.size()));
            // The last point uses the direction of the previous
            // because there is no next point to calculate the direction
            if (!mFixedDirection and i < mPoints.size())
            {
                nextPoint = mPoints[i].position;
                if (i > 1 or (nextPoint - position).Length() > 0.000001f) 
                {
                    direction = (nextPoint - position).Normalized();
                    direction = direction.Cross(norm).Normalized();
                }
                else if (mPoints.size() > 2)
                {
                    nextPoint = mPoints[i+1].position;
                    direction = (nextPoint - position).Normalized();
                    direction = direction.Cross(norm).Normalized();
                }
                else
                {
                    direction = RotationToVector(mOwner->GetWorldRotation());
                }
                
            }
            else if (mFixedDirection)
            {
                if (i == 0)
                {
                    direction = mPoints[i].direction.Normalized();
                }
                else
                {
                    direction = mPoints[i - 1].direction.Normalized();
                }
            }
            float3 topPointPos = position + direction * mWidth.CalculateValue(dp, mWidth.GetValue().GetInitialValue()) * 0.5f;
            float3 botPointPos = position - direction * mWidth.CalculateValue(dp, mWidth.GetValue().GetInitialValue()) * 0.5f;
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
}

void TrailComponent::Update()
{
    if (IsEnabled())
    {
        float3 position, scale;
        Quat rotationQ;
        mOwner->GetWorldTransform().Decompose(position, rotationQ, scale);
        const float3 lastPosition = mPoints.begin()->position;

        const float dposition = position.DistanceSq(lastPosition);
        if (mPoints.size() <= 1 || dposition >= mMinDistance && mPoints.size() < mMaxPoints)
        {
            float3 rotation = rotationQ.Transform(mDirection);
            mPoints.push_front(TrailPoint{ position, rotation, mTrailTime });
        }
        //*(mPoints.begin()) = TrailPoint{ mOwner->GetPosition(), lastRotation, mTrailTime };
        mTrailTime += App->GetDt();
        if (mPoints.size() > 2 and mMaxLifeTime > 0 and (mTrailTime - mPoints.back().creationTime) >= mMaxLifeTime)
        {
            mPoints.pop_back();
        }
    }
}

float3 TrailComponent::RotationToVector(Quat rotation) const
{
    rotation.Normalize();
    Quat rotatedQuat = rotation * Quat(0, mDirection.x, mDirection.y, mDirection.z) * rotation.Inverted();
    float3 rotatedVector(rotatedQuat.x, rotatedQuat.y, rotatedQuat.z);
    return rotatedVector.Normalized();
}

void TrailComponent::SetImage(unsigned int resourceId)
{
    mResourceId = resourceId;
    auto image = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
    mImage = image;
}

void TrailComponent::Reset()
{
    *this = TrailComponent(mOwner);
}

void TrailComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    obj.AddInt("Image", mResourceId);
    obj.AddInt("MaxPoints", mMaxPoints);
    obj.AddFloat("MinDistance", mMinDistance);
    obj.AddFloat("MaxLifeTime", mMaxLifeTime);
    obj.AddBool("IsFixedDirection", mFixedDirection);
    obj.AddFloats("Direction", mDirection.ptr(), 3);

    JsonObject width = obj.AddNewJsonObject("Width");
    mWidth.Save(width);

    JsonObject gradient = obj.AddNewJsonObject("Gradient");
    mGradient.Save(gradient);
}

void TrailComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    Component::Load(data, uidPointerMap);
    if(data.HasMember("Image")) 
    {
        mResourceId = data.GetInt("Image");
        SetImage(mResourceId);
    }
    if (data.HasMember("MaxPoints")) mMaxPoints = data.GetInt("MaxPoints");
    if (data.HasMember("MinDistance")) mMinDistance = data.GetFloat("MinDistance");    
    if (data.HasMember("MaxLifeTime")) mMaxLifeTime = data.GetFloat("MaxLifeTime");
    if (data.HasMember("IsFixedDirection")) mFixedDirection = data.GetBool("IsFixedDirection");
    if (data.HasMember("Direction")) 
    {
        float dir[3];
        data.GetFloats("Direction", dir);
        mDirection = float3(dir[0], dir[1], dir[2]);
    }
    if (data.HasMember("Gradient")) 
    {
        JsonObject gradient = data.GetJsonObject("Gradient");
        mGradient.Load(gradient);
    }
    if (data.HasMember("Width")) 
    {
        JsonObject width = data.GetJsonObject("Width");
        mWidth.Load(width);
    }
}

void TrailComponent::Enable()
{
    App->GetOpenGL()->AddTrail(this);
    Init();
}

void TrailComponent::Disable()
{
    App->GetOpenGL()->RemoveTrail(this);
    mPoints.clear();
}
