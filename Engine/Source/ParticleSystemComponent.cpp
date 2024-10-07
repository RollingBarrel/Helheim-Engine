#include "ParticleSystemComponent.h"
#include "ParticleSystemComponent.h"
#include "ParticleSystemComponent.h"
#include "Particle.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "Resource.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"
#include "Math/MathAll.h"
#include "Trail.h"

#define MATRICES_LOCATION 2
#define COLOR_LOCATION 1


ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(148626881, Resource::Type::Texture);
    mColorGradient.AddColorGradientMark(0.5f, float4(1.0f, 0.0f, 0.0f, 1.0f));
    Init();
    mTrail = new Trail();
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner) :  
Component(owner, ComponentType::PARTICLESYSTEM), mImageName(original.mImageName), mDuration(original.mDuration), mLifetime(original.mLifetime),
mSpeedCurve(original.mSpeedCurve), mSizeCurve(original.mSizeCurve), mEmissionRate(original.mEmissionRate), mMaxParticles(original.mMaxParticles),
mLooping(original.mLooping), mShapeType(original.mShapeType), mColorGradient(original.mColorGradient), 
mShapeAngle(original.mShapeAngle), mShapeRadius(original.mShapeRadius), mShapeSize(original.mShapeSize), mBlendMode(original.mBlendMode), 
mShapeRandAngle(original.mShapeRandAngle), mIsShapeAngleRand(original.mIsShapeAngleRand), mShapeInverseDir(original.mShapeInverseDir),
mFollowEmitter(original.mFollowEmitter), mSpinSpeed(original.mSpinSpeed), mBurst(original.mBurst), mGravity(original.mGravity),
mHasTrails(original.mHasTrails), mTrail(new Trail(*original.mTrail))

{
    if (original.mImage)
        mImage = (ResourceTexture*)App->GetResource()->RequestResource(original.mImage->GetUID(), Resource::Type::Texture);
    Init();
    mShapeType = original.mShapeType;
}

ParticleSystemComponent::~ParticleSystemComponent() 
{
    App->GetOpenGL()->RemoveParticleSystem(this);
    App->GetResource()->ReleaseResource(mImage->GetUID());
    mImage = nullptr;

    glDeleteBuffers(1, &mInstanceBuffer);
    glDeleteBuffers(1, &mVBO);
    for (Particle* particle : mParticles)
    {
        delete particle;
    }
    mParticles.clear();
    delete mTrail;
}

Component* ParticleSystemComponent::Clone(GameObject* owner) const
{
    return new ParticleSystemComponent(*this, owner);
}

void ParticleSystemComponent::Init()
{
    // set up mesh and attribute properties
    float particleQuad[] = {
        -0.1f, 0.1f, 0.0f, 1.0f,
        0.1f, -0.1f, 1.0f, 0.0f,
        -0.1f, -0.1f, 0.0f, 0.0f,

        -0.1f, 0.1f, 0.0f, 1.0f,
        0.1f, 0.1f, 1.0f, 1.0f,
        0.1f, -0.1f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glGenBuffers(1, &mInstanceBuffer);
    glBindVertexArray(mVAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);

    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

    glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
    for (unsigned int i = 0; i < 4; i++) 
    {
        glEnableVertexAttribArray(MATRICES_LOCATION + i);
        glVertexAttribPointer(MATRICES_LOCATION + i, 4, GL_FLOAT, GL_FALSE,
            20 * sizeof(float),
            (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(MATRICES_LOCATION + i, 1);
    }
    glEnableVertexAttribArray(COLOR_LOCATION);
    glVertexAttribPointer(COLOR_LOCATION, 4, GL_FLOAT, GL_FALSE,
        20 * sizeof(float),
        (const GLvoid*)(sizeof(GLfloat) * 16));
    glVertexAttribDivisor(COLOR_LOCATION, 1);
    glBindVertexArray(0);

    if (IsEnabled()) App->GetOpenGL()->AddParticleSystem(this);
}

void ParticleSystemComponent::Draw()
{
    if (IsEnabled() or mDisabling) 
    {
        unsigned int programId = App->GetOpenGL()->GetParticleProgramId();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);
        BlendModeFunction(mBlendMode);                      
        glUseProgram(programId);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        const CameraComponent* cam = (const CameraComponent*)App->GetCamera()->GetCurrentCamera();
        if (cam && mParticles.size()) 
        {
            float4x4 projection = cam->GetViewProjectionMatrix();
            float3 norm = cam->GetFrustum().front;
            float3 up = cam->GetFrustum().up;
            float3 right = up.Cross(norm).Normalized();
            glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
            glBufferData(GL_ARRAY_BUFFER, mParticles.size() * 20 * sizeof(float),
                nullptr, GL_DYNAMIC_DRAW);
            auto ptr = (float*)(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

            for (int i = 0; i < mParticles.size(); ++i)
            {
                float scale = mParticles[i]->GetSize();
                float3 pos;
                if (mFollowEmitter)
                {
                    pos =  (mOwner->GetWorldTransform() * float4(mParticles[i]->GetPosition(),1)).Float3Part();
                }
                else
                {
                    pos = mParticles[i]->GetPosition();
                }
                float3x3 scaleMatrix = float3x3::identity * scale;
                float4x4 transform;
                if (mStretchedBillboard) 
                {
                    float3 aux1 = Cross(norm, mParticles[i]->GetDirection() * mParticles[i]->GetSpeed()).Normalized();
                    float3 aux2 = Cross(aux1, norm).Normalized();
                    transform = { float4(aux2, 0), float4(aux1, 0),float4(norm, 0),float4(pos, 1) };
                    float stretch = (mParticles[i]->GetDirection() * mParticles[i]->GetSpeed()).Dot(aux2);
                    scaleMatrix[0][0] = scaleMatrix[0][0] * std::max(1.0f, Sqrt(stretch/scale) * mStretchedRatio);
                }
                else 
                {
                    transform = { float4(right, 0), float4(up, 0),float4(norm, 0),float4(pos, 1) };
                }
                transform = transform * scaleMatrix;
                transform.Transpose();
                memcpy(ptr + 20 * i, transform.ptr(), sizeof(float) * 16);
                memcpy(ptr + 20 * i + 16, mParticles[i]->GetColor().ptr(), sizeof(float) * 4);
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
            glBindVertexArray(mVAO);

            glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_TRUE, &projection[0][0]);
            glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

            glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mParticles.size());
        }

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }

    if (mDisabling)
    {
        for (int i = 0; i < mParticles.size(); i++)
        {
            float dt = mParticles[i]->Update(App->GetDt(), mGravity, mOwner);
            if (dt >= 1)
            {
                delete mParticles[i];
                mParticles.erase(mParticles.begin() + i);
                i--;
            }
            else
            {
                mParticles[i]->SetSpeed(mSpeedCurve.CalculateValue(dt, mParticles[i]->GetInitialSpeed()));
                mParticles[i]->SetSize(mSizeCurve.CalculateValue(dt, mParticles[i]->GetInitialSize()));
                mParticles[i]->SetColor(mColorGradient.CalculateColor(dt));
            }
        }
        if (mParticles.empty())
        {
            App->GetOpenGL()->RemoveParticleSystem(this);
            for (Particle* particle : mParticles)
            {
                delete particle;
            }
            mParticles.clear();
            mDisabling = false;
        }
    }
}

void ParticleSystemComponent::Update()
{
    if (IsEnabled() and !App->IsPause())
    {
        mEmitterTime += App->GetDt();
        mEmitterDeltaTime += App->GetDt();
        if (mEmitterTime < mDelay) return;

        for (int i = 0; i < mParticles.size(); i++)
        {
            float dt = mParticles[i]->Update(App->GetDt(), mGravity, mOwner);
            if (dt >= 1)
            {
                delete mParticles[i];
                mParticles.erase(mParticles.begin() + i);
                i--;
            }
            else
            {
                float speed = mSpeedCurve.CalculateValue(dt, mParticles[i]->GetInitialSpeed());
                if (mSpeedCenterShape) 
                {
                    float distance = DistanceToCenter(mParticles[i]->GetPosition());
                    speed *= distance;
                }
                mParticles[i]->SetSpeed(speed);
                mParticles[i]->SetSize(mSizeCurve.CalculateValue(dt, mParticles[i]->GetInitialSize()));
                mParticles[i]->SetColor(mColorGradient.CalculateColor(dt));
            }
        }

        if (mFollowEmitter)
        {
            mOwner->SetLocalRotation(mOwner->GetLocalRotation() * Quat::RotateZ(mSpinSpeed * App->GetDt()));
        }

        if (!mLooping and mEmitterTime - mDelay > mDuration) return;
        
        if (mIsInBurst)
        {
            for (size_t i = 0; i < mBurst; ++i)
            {
                if (mParticles.size() < mMaxParticles)
                {
                    CreateNewParticle();
                }
            }
            mIsInBurst = false;
        }

        while (mEmitterDeltaTime > 1 / mEmissionRate)
        {
            mEmitterDeltaTime = mEmitterDeltaTime - 1 / mEmissionRate;

            if (mParticles.size() < mMaxParticles)
            {
                CreateNewParticle();
            }
        }
    }
}

void ParticleSystemComponent::CreateNewParticle()
{
    // Initializes a particle with a random position, direction and rotation
    // relative to the shape of emission
    float3 emitionPosition = ShapeInitPosition();
    float3 emitionDirection = ShapeInitDirection(emitionPosition);
    if (!mFollowEmitter)
    {
        float4 auxPosition = mOwner->GetWorldTransform() * float4(emitionPosition, 1.0);
        emitionPosition = float3(auxPosition.x, auxPosition.y, auxPosition.z);
        float3 auxDirection = mOwner->GetWorldTransform().Float3x3Part() * emitionDirection;
        emitionDirection = auxDirection.Normalized();
    }

    float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
    float rotation = (random * 3.1415 / 2) - (3.1415 / 4);

    float initialSize = mSizeCurve.GetValue().CalculateRandom();
    float initialSpeed = mSpeedCurve.GetValue().CalculateRandom();
    float speed = mSpeedCurve.CalculateValue(0, initialSpeed);
    if (mSpeedCenterShape)
    {
        float distanceToCenter = DistanceToCenter(emitionPosition);
        speed *= distanceToCenter;
    }
    Particle* particle = new Particle(emitionPosition, emitionDirection, 
        mColorGradient.CalculateColor(0.0f), rotation, mLifetime.CalculateRandom(), 
        initialSize, mSizeCurve.CalculateValue(0, initialSize),
        initialSpeed, speed,
        mHasTrails, mTrail, mFollowEmitter);
    mParticles.push_back(particle);
}

float ParticleSystemComponent::DistanceToCenter(float3 position)
{
    float3 center;
    if (mFollowEmitter) center = float3(0, 0, 0);
    else center = mOwner->GetWorldPosition();
    switch (mShapeType)
    {
    case EmitterType::CILINDER:
    {
        float3 projectedPosition = float3(position.x, position.y, center.z);
        return std::max(projectedPosition.Distance(center) - mSpeedCenterFactor, 0.0f); 
    }
    case EmitterType::DONUT:
    {
        float3 projectedPosition = float3(position.x, position.y, center.z);  
        float distToXYCenter = sqrt(pow(position.x - center.x, 2) + pow(position.y - center.y, 2)); 

        float distanceToTubeCenter = std::abs(distToXYCenter - mShapeRadius);

        float zDistance = std::abs(position.z - center.z);
        return std::max(sqrt(distanceToTubeCenter * distanceToTubeCenter + zDistance * zDistance) - mSpeedCenterFactor,0.0f);
    }
    default:
    {
        return std::max(position.Distance(center) - mSpeedCenterFactor, 0.0f);
    }
    }
}

void ParticleSystemComponent::SetImage(unsigned int resourceId)
{
    App->GetResource()->ReleaseResource(mImage->GetUID());
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}

void ParticleSystemComponent::Reset()
{
    for (auto particle : mParticles)
    {
        delete particle;
    }
    *this = ParticleSystemComponent(mOwner);
}

void ParticleSystemComponent::Save(JsonObject& obj) const
{
    //TODO: REDOOO
    Component::Save(obj);
    obj.AddInt("Image", mImage->GetUID());
    obj.AddFloat("Delay", mDelay);
    obj.AddFloat("Duration", mDuration);
    obj.AddFloat("EmissionRate", mEmissionRate);
    obj.AddInt("MaxParticles", mMaxParticles);
    obj.AddInt("Burst", mBurst);
    obj.AddBool("Looping", mLooping);
    obj.AddBool("FollowEmitter", mFollowEmitter);
    obj.AddFloat("SpinSpeed", mSpinSpeed);
    obj.AddBool("StretchedBillboard", mStretchedBillboard);
    obj.AddFloat("StretchedRatio", mStretchedRatio);
    obj.AddFloat("Gravity", mGravity);
    JsonObject lifetime = obj.AddNewJsonObject("Lifetime");
    mLifetime.Save(lifetime);

    obj.AddInt("ShapeType", static_cast<int>(mShapeType));
    obj.AddFloat("ShapeRadius", mShapeRadius);
    obj.AddFloat("ShapeTubeRadius", mShapeTubeRadius);
    obj.AddFloat("ShapeHeight", mShapeHeight);
    obj.AddFloat("ShapeAngle", mShapeAngle);
    obj.AddFloats("ShapeSize", mShapeSize.ptr(), 3);
    obj.AddFloat("ShapeRandAngle", mShapeRandAngle);
    obj.AddBool("ShapeIsRandAngle", mIsShapeAngleRand);
    obj.AddBool("ShapeInvertedDir", mShapeInverseDir);
    obj.AddBool("SpeedCenterShape", mSpeedCenterShape);
    obj.AddFloat("SpeedCenterFactor", mSpeedCenterFactor);

    obj.AddInt("BlendMode", mBlendMode);

    JsonObject size = obj.AddNewJsonObject("SizeCurve");
    mSizeCurve.Save(size);
    JsonObject speed = obj.AddNewJsonObject("SpeedCurve");
    mSpeedCurve.Save(speed);
    mColorGradient.Save(obj);
    obj.AddBool("HasTrails", mHasTrails);
    JsonObject trail = obj.AddNewJsonObject("Trail");
    mTrail->Save(trail);
}

void ParticleSystemComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
    //TODO REDOOO
    Component::Load(data, uidPointerMap);
    if (data.HasMember("Image")) 
    {
        SetImage(data.GetInt("Image"));
    }
    if (data.HasMember("Delay")) mDelay = data.GetFloat("Delay");
    if (data.HasMember("Duration")) mDuration = data.GetFloat("Duration");
    if (data.HasMember("EmissionRate")) mEmissionRate = data.GetFloat("EmissionRate");
    if (data.HasMember("MaxParticles")) mMaxParticles = data.GetInt("MaxParticles");
    if (data.HasMember("Burst")) mBurst = data.GetInt("Burst");
    if (data.HasMember("Looping")) mLooping = data.GetBool("Looping");
    if (data.HasMember("FollowEmitter")) mFollowEmitter = data.GetBool("FollowEmitter");
    if (data.HasMember("SpinSpeed")) mSpinSpeed = data.GetFloat("SpinSpeed");
    if (data.HasMember("StretchedBillboard")) mStretchedBillboard = data.GetBool("StretchedBillboard");
    if (data.HasMember("StretchedRatio")) mStretchedRatio = data.GetFloat("StretchedRatio");
    if (data.HasMember("Gravity")) mGravity = data.GetFloat("Gravity");
    if (data.HasMember("Lifetime")) 
    {
        JsonObject lifetime = data.GetJsonObject("Lifetime");
        mLifetime.Load(lifetime);
    }
    if (data.HasMember("ShapeType")) mShapeType = static_cast<EmitterType>(data.GetInt("ShapeType"));
    if (data.HasMember("ShapeRadius")) mShapeRadius = data.GetFloat("ShapeRadius");
    if (data.HasMember("ShapeTubeRadius")) mShapeTubeRadius = data.GetFloat("ShapeTubeRadius");
    if (data.HasMember("ShapeHeight")) mShapeHeight = data.GetFloat("ShapeHeight");
    if (data.HasMember("ShapeAngle")) mShapeAngle = data.GetFloat("ShapeAngle");
    if (data.HasMember("ShapeSize")) 
    {
        float size[3];
        data.GetFloats("ShapeSize", size);
        mShapeSize = float3(size[0], size[1], size[2]);

    }
    if (data.HasMember("ShapeIsRandAngle")) mIsShapeAngleRand = data.GetBool("ShapeIsRandAngle");
    if (data.HasMember("ShapeRandAngle")) mShapeRandAngle = data.GetFloat("ShapeRandAngle");
    if (data.HasMember("ShapeInvertedDir")) mShapeInverseDir = data.GetBool("ShapeInvertedDir");
    if (data.HasMember("SpeedCenterShape")) mSpeedCenterShape = data.GetBool("SpeedCenterShape");
    if (data.HasMember("SpeedCenterFactor")) mSpeedCenterFactor = data.GetFloat("SpeedCenterFactor");
    if (data.HasMember("BlendMode")) mBlendMode = data.GetInt("BlendMode");
    if (data.HasMember("SizeCurve")) 
    {
        JsonObject sizeObj = data.GetJsonObject("SizeCurve"); 
        mSizeCurve.Load(sizeObj);
    }
    if (data.HasMember("SpeedCurve")) 
    {
        JsonObject speedObj = data.GetJsonObject("SpeedCurve");
        mSpeedCurve.Load(speedObj);
    }
    mColorGradient.Load(data);
    if (data.HasMember("HasTrails")) mHasTrails = data.GetBool("HasTrails");
    if (data.HasMember("Trail"))
    {
        JsonObject trailObj = data.GetJsonObject("Trail");
        mTrail->Load(trailObj);
    }
}

void ParticleSystemComponent::Enable()
{
    if (!mDisabling) App->GetOpenGL()->AddParticleSystem(this);
    mDisabling = false;
    mEmitterTime = 0.0f;
    mEmitterDeltaTime = 0.0f;
    mIsInBurst = mBurst;
}

void ParticleSystemComponent::Disable()
{
    mDisabling = true;
    //App->GetOpenGL()->RemoveParticleSystem(this);

}

float3 ParticleSystemComponent::ShapeInitPosition() const
{
    auto randFloat = []() -> float 
        {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        };

    switch (mShapeType)
    {
    case EmitterType::CONE:
    {
        float r = sqrt(randFloat());
        float angle = randFloat() * 2 * math::pi;
        float distance = r * mShapeRadius;
        float x = distance * cos(angle);
        float y = distance * sin(angle);

        return float3(x, y, 0);
    }
    case EmitterType::BOX:
    {
        float randX = randFloat() - 0.5f;
        float randY = randFloat() - 0.5f;
        float randZ = randFloat() - 0.5f;

        return float3(mShapeSize.x * randX, mShapeSize.y * randY, mShapeSize.z * randZ);
    }
    case EmitterType::SPHERE:
    {
        float u = randFloat();
        float v = randFloat();
        float theta = u * 2.0f * math::pi;
        float phi = acos(2.0f * v - 1.0f);
        float r = cbrt(randFloat()) * mShapeRadius;
        float x = r * sin(phi) * cos(theta);
        float y = r * sin(phi) * sin(theta);
        float z = r * cos(phi);

        return float3(x, y, z);
    }
    case EmitterType::CILINDER:
    {
        float height = (randFloat() - 0.5f) * mShapeHeight;
        float angle = randFloat() * 2 * math::pi;
        float radius = randFloat() * mShapeRadius;

        float x = radius * cos(angle);
        float y = radius * sin(angle);
        float z = height;

        return float3(x, y, z);
    }
    case EmitterType::DONUT:
    {
        float angle1 = randFloat() * 2 * math::pi;
        float angle2 = randFloat() * 2 * math::pi;

        float majorRadius = mShapeRadius;
        float minorRadius = mShapeTubeRadius;

        float x = (majorRadius + minorRadius * cos(angle2)) * cos(angle1);
        float y = (majorRadius + minorRadius * cos(angle2)) * sin(angle1);
        float z = minorRadius * sin(angle2);

        return float3(x, y, z);
    }
    default:
        return float3(1.0f, 1.0f, 1.0f);
    }
}

float3 ParticleSystemComponent::ShapeInitDirection(const float3& pos) const
{
    auto randFloat = []() -> float 
        {
            return static_cast<float>(rand()) / static_cast<float>(RAND_MAX) - 0.5f;
        };

    switch (mShapeType)
    {
    case EmitterType::CONE:
    {
        float theta = 0.0f;
        float phi = 0.0f;
        if (mIsShapeAngleRand) 
        {
            theta = pos.AngleBetween(float3(1, 0, 0)) + randFloat() * mShapeRandAngle;
            if (pos.y < 0) theta = -theta;
            phi = (pos.Length() / mShapeRadius) * mShapeAngle + randFloat() * mShapeRandAngle;
        }
        else 
        {
            theta = pos.AngleBetween(float3(1, 0, 0));
            if (pos.y < 0) theta = -theta;
            phi = (pos.Length() / mShapeRadius) * mShapeAngle;

        }
        float x = std::sin(phi) * std::cos(theta);
        float y = std::sin(phi) * std::sin(theta);
        float z = std::cos(phi);

        return float3(x, y, z).Normalized();
    }
    case EmitterType::BOX:
    {
        float3 direction;
        if (mShapeFollowZAxis)
        {
            direction = float3(0.0f, 0.0f, 1.0f);
        }
        else
        {
            direction = pos.Normalized();
        }
        if (mShapeInverseDir) direction = -direction;

        if (mIsShapeAngleRand)
        {
            float theta = randFloat() * math::pi * 2;
            float phi = randFloat() * mShapeRandAngle;

            float3 arbitraryVec = std::abs(direction.y) > 0.9f ? float3(1, 0, 0) : float3(0, 1, 0);
            float3 rotationAxis = Cross(direction, arbitraryVec).Normalized();

            direction = float3x3::RotateAxisAngle(rotationAxis, phi) * direction;
        }
        return direction.Normalized();
    }
    case EmitterType::SPHERE:
    {
        float3 direction;
        if (mShapeInverseDir) direction = (-pos).Normalized();
        else direction = pos.Normalized();
        if (mIsShapeAngleRand)
        {
            float theta = randFloat() * math::pi * 2;
            float phi = randFloat() * mShapeRandAngle;

            float3 arbitraryVec = std::abs(direction.y) > 0.9f ? float3(1, 0, 0) : float3(0, 1, 0);
            float3 rotationAxis = Cross(direction, arbitraryVec).Normalized();

            direction = float3x3::RotateAxisAngle(rotationAxis, phi) * direction;
        }
        return direction.Normalized();
    }
    case EmitterType::CILINDER:
    {
        float3 direction;
        if (mShapeFollowZAxis)
        {
            direction = float3(0.0f, 0.0f, 1.0f);
        }
        else
        {
            float3 basePos = float3(pos.x, pos.y, 0.0f);
            if (mShapeInverseDir)
                direction = (-basePos).Normalized();
            else
                direction = basePos.Normalized();
        }
        if (mIsShapeAngleRand)
        {
            float theta = randFloat() * 2 * math::pi;
            float phi = randFloat() * mShapeRandAngle;
            float3 arbitraryVec = std::abs(direction.y) > 0.9f ? float3(1, 0, 0) : float3(0, 1, 0);
            float3 rotationAxis = Cross(direction, arbitraryVec).Normalized();
            direction = float3x3::RotateAxisAngle(rotationAxis, phi) * direction;
        }
        return direction.Normalized();
    }
    case EmitterType::DONUT:
    {
        float3 direction = pos - float3(pos.x, pos.y, 0).Normalized() * mShapeRadius;
        if (mShapeInverseDir) direction = -direction;
        direction = direction.Normalized();

        if (mIsShapeAngleRand)
        {
            float theta = randFloat() * 2 * math::pi;
            float phi = randFloat() * mShapeRandAngle;
            float3 arbitraryVec = std::abs(direction.y) > 0.9f ? float3(1, 0, 0) : float3(0, 1, 0);
            float3 rotationAxis = Cross(direction, arbitraryVec).Normalized();
            direction = float3x3::RotateAxisAngle(rotationAxis, phi) * direction;
        }
        return direction.Normalized();
    }

    default:
        return float3(0, 0, 1).Normalized();
    }
}

bool ParticleSystemComponent::HasEnded() const
{
    return (mEmitterTime > mDuration) && mParticles.empty() && !mLooping;
}
