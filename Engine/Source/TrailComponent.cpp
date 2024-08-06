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
#include "Timer.h"

#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define COLOR_LOCATION 2
#define DIRECTION_LOCATION 3

#define COLOR_BUFFER_BINDING 30
#define WIDTH_BUFFER_BINDING 31

#define VBO_FLOAT_SIZE 9 // number of floats for every vertex 3 (position) + 2 (texcoord) + 1 (colorPos) + 3 (direction)


TrailComponent::TrailComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::TRAIL)
{
    Init();
}

TrailComponent::TrailComponent(const TrailComponent& original, GameObject* owner) : Component(owner, ComponentType::TRAIL), 
mResourceId(original.mResourceId), mFileName(original.mFileName),
mMaxPoints(original.mMaxPoints), mMinDistance(original.mMinDistance),
mPoints(original.mPoints), mGradient(ColorGradient(original.mGradient)), mWidth(original.mWidth),
mFixedDirection(original.mFixedDirection), mTrailTime(original.mTrailTime), mDirection(original.mDirection), 
mMaxLifeTime(original.mMaxLifeTime), mIsUVScrolling(original.mIsUVScrolling), mUVScroll(original.mUVScroll)
{
    Init();
}

TrailComponent::~TrailComponent()
{
    App->GetOpenGL()->RemoveTrail(this);
    App->GetResource()->ReleaseResource(mResourceId);

}

Component* TrailComponent::Clone(GameObject* owner) const
{
    return new TrailComponent(*this, owner);
}

void TrailComponent::Init()
{
    mPoints.clear();
    SetImage(mResourceId);

    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mSSBOColor);
    glGenBuffers(1, &mSSBOWidth);
    glBindVertexArray(mVAO);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    // vertex positions
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, VBO_FLOAT_SIZE * sizeof(float), (void*)0);
    // texture coordinates
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, VBO_FLOAT_SIZE * sizeof(float), (void*)(3 * sizeof(float)));
    // color position
    glEnableVertexAttribArray(COLOR_LOCATION);
    glVertexAttribPointer(COLOR_LOCATION, 1, GL_FLOAT, GL_FALSE, VBO_FLOAT_SIZE * sizeof(float), (void*)(5 * sizeof(float)));
    // direction
    glEnableVertexAttribArray(DIRECTION_LOCATION);
    glVertexAttribPointer(DIRECTION_LOCATION, 3, GL_FLOAT, GL_FALSE, VBO_FLOAT_SIZE * sizeof(float), (void*)(6 * sizeof(float)));

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSSBOColor);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, COLOR_BUFFER_BINDING, mSSBOColor);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSSBOWidth);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, WIDTH_BUFFER_BINDING, mSSBOWidth);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    App->GetOpenGL()->AddTrail(this);
}

void TrailComponent::Draw() const
{
    if (!IsEnabled() || mPoints.size() <= 1) return;

    unsigned int programId = App->GetOpenGL()->GetTrailProgramId();
    const CameraComponent* cam = App->GetCamera()->GetCurrentCamera();
    float4x4 projection = cam->GetViewProjectionMatrix();

    glDepthMask(GL_FALSE);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    struct ColorPoint
    {
        float position;
        float padding[3];
        float color[4];
    };
    std::vector<ColorPoint> colors;
    for (const auto& pair : mGradient.GetColorMarks()) 
    {
        colors.push_back({
            pair.first, 
            0.0f, 0.0f, 0.0f, 
            pair.second.x, pair.second.y , pair.second.z , pair.second.w});
    }
    // Vincular el buffer SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSSBOColor);
    size_t ssboSize = colors.size() * sizeof(ColorPoint);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssboSize, nullptr, GL_DYNAMIC_DRAW);
    float* ptrColor = reinterpret_cast<float*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
    if (ptrColor) {
        std::memcpy(ptrColor, colors.data(), colors.size() * sizeof(ColorPoint));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    else {
        std::cerr << "Error: no se pudo mapear el buffer SSBOColor." << std::endl;
    }

    struct WidthPoint
    {
        float position;
        float width;
    };
    std::vector<WidthPoint> widths;
    for (const auto& pair : mWidth.GetPoints())
    {
        widths.push_back({
            pair.x,
            pair.y});
    }
    // Vincular el buffer SSBO
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, mSSBOWidth);
    ssboSize = widths.size() * sizeof(WidthPoint);
    glBufferData(GL_SHADER_STORAGE_BUFFER, ssboSize, nullptr, GL_DYNAMIC_DRAW);
    float* ptrWidth = reinterpret_cast<float*>(glMapBuffer(GL_SHADER_STORAGE_BUFFER, GL_WRITE_ONLY));
    if (ptrWidth) {
        std::memcpy(ptrWidth, widths.data(), widths.size() * sizeof(WidthPoint));
        glUnmapBuffer(GL_SHADER_STORAGE_BUFFER);
    }
    else {
        std::cerr << "Error: no se pudo mapear el buffer SSBOWidth." << std::endl;
    }

    glUseProgram(programId);
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, (mPoints.size()) * 2 * VBO_FLOAT_SIZE * sizeof(float), mBuffer.data(), GL_DYNAMIC_DRAW);

    glBindVertexArray(mVAO);
    glUniformMatrix4fv(glGetUniformLocation(programId, "viewProj"), 1, GL_TRUE, &projection[0][0]);
    glUniform1f(glGetUniformLocation(programId, "minDist"), mMinDist);
    glUniform1f(glGetUniformLocation(programId, "maxDist"), mMaxDist);
    glUniform1f(glGetUniformLocation(programId, "scrollUV"), mUVScroll);
    glUniform1i(glGetUniformLocation(programId, "colorSize"), colors.size());
    glUniform1i(glGetUniformLocation(programId, "isUVScrolling"), mIsUVScrolling);
    glUniform1i(glGetUniformLocation(programId, "widthSize"), widths.size());
    glUniform1i(glGetUniformLocation(programId, "isCurve"), mWidth.IsCurve());
    glUniform1i(glGetUniformLocation(programId, "isRandWidth"), mWidth.GetValue().IsRand());
    glUniform1f(glGetUniformLocation(programId, "minWidth"), mWidth.GetValue().GetMinValue());
    glUniform1f(glGetUniformLocation(programId, "maxWidth"), mWidth.GetValue().GetMaxValue());
    glUniform1f(glGetUniformLocation(programId, "time"), App->GetCurrentClock()->GetTotalTime());

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

float3 TrailComponent::CalculateDirection(const float3& position, const float3& norm) const
{
    if (mFixedDirection)
    {
        return mPoints.front().mDirection.Normalized();
    }
    if (mPoints.size() < 2)
    {
        return mPoints.front().mDirection.Normalized();
    }
    float3 lastPosition = mPoints[1].mPosition;
    return (position - lastPosition).Normalized().Cross(norm).Normalized();
}

void TrailComponent::Update()
{
    if (IsEnabled())
    {
        if (mPoints.size() == 0)
        {
            AddFirstTrailPoint();
        }
        else
        {
            float3 position = mOwner->GetWorldPosition();
            Quat rotationQ = mOwner->GetWorldRotation();
            const float3 lastPosition = mPoints.begin()->mPosition;

            const float dposition = position.DistanceSq(lastPosition);
            if (dposition >= mMinDistance && mPoints.size() < mMaxPoints)
            {
                AddFirstTrailPoint();
            }

            mTrailTime += App->GetDt();
            if (/*mPoints.size() > 1 and*/ mMaxLifeTime > 0 and (mTrailTime - mPoints.back().mCreationTime) >= mMaxLifeTime)
            {
                RemoveLastTrailPoint();
            }
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
    App->GetResource()->ReleaseResource(mResourceId);
    mResourceId = resourceId;
    auto image = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
    mImage = image;
}

void TrailComponent::Reset()
{
    *this = TrailComponent(mOwner);
}

void TrailComponent::Enable()
{
    Init();
}

void TrailComponent::Disable()
{
    App->GetOpenGL()->RemoveTrail(this);
    mPoints.clear();
    mBuffer.clear();
    mMinDist = 0.0f;
    mMaxDist = 0.0f;
}

void TrailComponent::AddFirstTrailPoint()
{
    float3 position = mOwner->GetWorldPosition();
    float3 rotation = mOwner->GetWorldRotation().Transform(mDirection);
    float distUV;
    if (mPoints.size() > 0) 
    {
        distUV = mPoints.front().mDistanceUV + (position.Distance(mPoints.front().mPosition));
    }
    else 
    {
        distUV = 0.0f;
    }
    mPoints.push_front(TrailPoint({ position, rotation, mTrailTime, distUV }));

    const CameraComponent* cam = App->GetCamera()->GetCurrentCamera();
    float3 norm = cam->GetFrustum().front;
    float3 direction;
    mWidth.GetValue().CalculateInitialValue();

    float deltaPos = (distUV - mMinDist) / (mMaxDist - mMinDist);
    direction = CalculateDirection(position, norm);
    
    float3 topPointPos = position + direction * (mWidth.GetValue().GetMinValue() * 0.5f);
    float3 botPointPos = position - direction * (mWidth.GetValue().GetMinValue() * 0.5f);

    float2 topPointTexCoord = float2(mPoints.front().mDistanceUV, 1);
    float2 botPointTexCoord = float2(mPoints.front().mDistanceUV, 0);

    if (mPoints.size() == 2)
    {
        float3 topPos = mPoints[0].mPosition + direction * (mWidth.GetValue().GetMinValue() * 0.5f);;
        float3 botPos = mPoints[0].mPosition - direction * (mWidth.GetValue().GetMinValue() * 0.5f);;
        mBuffer.push_back({ botPos, botPointTexCoord, distUV, direction });
        mBuffer.push_back({ topPos, topPointTexCoord, distUV, direction });
    }    
    if (mPoints.size() >= 2)
    {
        mBuffer.push_back({ botPointPos, botPointTexCoord, distUV, direction });
        mBuffer.push_back({ topPointPos, topPointTexCoord, distUV, direction });
    }


    mMinDist = distUV;
}

void TrailComponent::RemoveLastTrailPoint()
{
    mPoints.pop_back();
    mBuffer.erase(mBuffer.begin());
    mBuffer.erase(mBuffer.begin());
    if (!mBuffer.empty()) mMaxDist = mBuffer[0].mDistance;
    else mMaxDist = 0.0f;
}

void TrailComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    obj.AddInt("Image", mResourceId);
    obj.AddInt("MaxPoints", mMaxPoints);
    obj.AddFloat("MinDistance", mMinDistance);
    obj.AddFloat("MaxLifeTime", mMaxLifeTime);
    obj.AddFloat("UVScroll", mUVScroll);
    obj.AddBool("IsFixedDirection", mFixedDirection);
    obj.AddBool("IsUVScrolling", mIsUVScrolling);
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
    if (data.HasMember("UVScroll")) mUVScroll = data.GetFloat("UVScroll");
    if (data.HasMember("IsFixedDirection")) mFixedDirection = data.GetBool("IsFixedDirection");
    if (data.HasMember("IsUVScrolling")) mIsUVScrolling = data.GetBool("IsUVScrolling");
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

TrailComponent::TrailPoint::TrailPoint(float3 pos, float3 dir, float time, float distUV) : 
    mPosition(pos), mDirection(dir), mCreationTime(time), mDistanceUV(distUV)
{
}
