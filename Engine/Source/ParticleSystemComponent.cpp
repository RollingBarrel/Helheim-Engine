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

#define MATRICES_LOCATION 2
#define COLOR_LOCATION 1


ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    SetImage(mResourceId);
    mColorGradient.AddColorGradientMark(0.5f, float4(1.0f, 0.0f, 0.0f, 1.0f));
    Init();
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner) :  
Component(owner, ComponentType::PARTICLESYSTEM), mFileName(original.mFileName), mDuration(original.mDuration), 
mIsLifetimeRandom(original.mIsLifetimeRandom), mLifetime(original.mLifetime), mMaxLifetime(original.mMaxLifetime),
mSpeedCurve(original.mSpeedCurve), mSizeCurve(original.mSizeCurve), mEmissionRate(original.mEmissionRate), mMaxParticles(original.mMaxParticles),
mLooping(original.mLooping), mShapeType(original.mShapeType), mColorGradient(original.mColorGradient), 
mShapeAngle(original.mShapeAngle), mShapeRadius(original.mShapeRadius), mShapeSize(original.mShapeSize)
{
    SetImage(original.mResourceId);
    Init();
    mShapeType = original.mShapeType;
}

ParticleSystemComponent::~ParticleSystemComponent() 
{
    App->GetOpenGL()->RemoveParticleSystem(this);
    glDeleteBuffers(1, &mInstanceBuffer);
    glDeleteBuffers(1, &mVBO);
    for (auto particle : mParticles)
    {
        delete particle;
    }
    mParticles.clear();
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

    App->GetOpenGL()->AddParticleSystem(this);
}

void ParticleSystemComponent::Draw() const
{
    if (IsEnabled()) 
    {
        unsigned int programId = App->GetOpenGL()->GetParticleProgramId();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);									// Enable Blending
        BlendModeFunction(mBlendMode);                      
        //glEnable(GL_TEXTURE_2D);
        //glBlendEquation(GL_FUNC_ADD);
        glUseProgram(programId);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        const CameraComponent* cam = (const CameraComponent*)App->GetCamera()->GetCurrentCamera();
        if (cam && mParticles.size()) 
        {
            float4x4 projection = cam->GetViewProjectionMatrix();
            float3 norm = cam->GetFrustum().front; //(mParticles[i]->GetPosition() - cam->GetFrustum().pos).Normalized();
            float3 up = cam->GetFrustum().up;
            float3 right = up.Cross(norm).Normalized();
            //up = norm.Cross(right).Normalized();
            glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
            glBufferData(GL_ARRAY_BUFFER, mParticles.size() * 20 * sizeof(float),
                nullptr, GL_DYNAMIC_DRAW);
            auto ptr = (float*)(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

            for (int i = 0; i < mParticles.size(); ++i)
            {
                float scale = mParticles[i]->GetSize();
                float3 pos = mParticles[i]->GetPosition();
                float3x3 scaleMatrix = float3x3::identity * scale;
                float4x4 transform;
                if (mStretchedBillboard) 
                {
                    float3 aux1 = Cross(norm, mParticles[i]->GetDirection() * mParticles[i]->GetSpeed()).Normalized();
                    float3 aux2 = Cross(aux1, norm).Normalized();
                    transform = { float4(aux2, 0), float4(aux1, 0),float4(norm, 0),float4(pos, 1) };
                    float stretch = (mParticles[i]->GetDirection() * mParticles[i]->GetSpeed()).Dot(aux2);
                    scaleMatrix[0][0] = scaleMatrix[0][0] * std::max(1.0f, Sqrt(stretch/scale));
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
}

void ParticleSystemComponent::Update()
{
    mEmitterTime += App->GetDt();
    mEmitterDeltaTime += App->GetDt();
    if (mEmitterTime < mDelay) return;

	for (int i = 0; i < mParticles.size(); i++)
	{
		float dt = mParticles[i]->Update(App->GetDt());
        if (dt >= 1)
        {
			mParticles.erase(mParticles.begin() + i);
			i--;
		}
        else
        {
            mParticles[i]->SetSpeed(mSpeedCurve.GetValue(dt, mParticles[i]->GetInitialSpeed()));
            mParticles[i]->SetSize(mSizeCurve.GetValue(dt, mParticles[i]->GetInitialSize()));
            mParticles[i]->SetColor(mColorGradient.CalculateColor(dt));
        }
	}
    if (!mLooping and mEmitterTime - mDelay > mDuration) return;

	if (mEmitterDeltaTime > 1 / mEmissionRate)
	{
		mEmitterDeltaTime = mEmitterDeltaTime - 1 / mEmissionRate;
		if (mParticles.size() < mMaxParticles)
		{
            // Initializes a particle with a random position, direction and rotation
            // relative to the shape of emission

			float3 emitionPosition = ShapeInitPosition();
            float3 emitionDirection = ShapeInitDirection();
            float4 auxPosition = mOwner->GetWorldTransform() * float4(emitionPosition, 1.0);
            emitionPosition = float3(auxPosition.x, auxPosition.y, auxPosition.z);
            float3 auxDirection = mOwner->GetWorldTransform().Float3x3Part() * emitionDirection;
            emitionDirection = auxDirection.Normalized();

            float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rotation = (random * 3.1415 / 2) - (3.1415 / 4);

            // Create the particle and sets its speed and size 
            // considering if they are linear or curve
            Particle* particle = new Particle(emitionPosition, emitionDirection, mColorGradient.CalculateColor(0.0f), rotation, CalculateRandomLifetime());
            particle->SetInitialSpeed(mSpeedCurve.CalculateInitialValue());
            particle->SetInitialSize(mSizeCurve.CalculateInitialValue());
            
			mParticles.push_back(particle);
		}
	}
}

void ParticleSystemComponent::SetImage(unsigned int resourceId)
{
    mResourceId = resourceId;
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}

float ParticleSystemComponent::CalculateRandomLifetime() const
{
    if (mIsLifetimeRandom)
    {
        //std::srand(static_cast<unsigned int>(std::time(nullptr)));
        float random_value = static_cast<float>(std::rand()) / static_cast<float>(RAND_MAX);
        return random_value * (mMaxLifetime - mLifetime) + mLifetime;
    }
    else
    {
        return mLifetime;
    }
}

void ParticleSystemComponent::Reset()
{
    for (auto particle : mParticles)
    {
        delete particle;
    }
    *this = ParticleSystemComponent(mOwner);
}

void ParticleSystemComponent::Save(Archive& archive) const
{
    Component::Save(archive);
    archive.AddInt("Image", mResourceId);
    archive.AddFloat("Delay", mDelay);
    archive.AddFloat("Duration", mDuration);
    archive.AddFloat("Emission Rate", mEmissionRate);
    archive.AddInt("Max Particles", mMaxParticles);
    archive.AddBool("Looping", mLooping);
    archive.AddBool("Stretched Billboard", mStretchedBillboard);
    archive.AddBool("IsLifetimeRandom", mIsLifetimeRandom);
    archive.AddFloat("Lifetime", mLifetime);
    archive.AddFloat("MaxLifetime", mMaxLifetime);
    archive.AddInt("ShapeType", (int)mShapeType);
    archive.AddFloat("ShapeRadius", mShapeRadius);
    archive.AddFloat("ShapeAngle", mShapeAngle);
    archive.AddFloat3("ShapeSize", mShapeSize);
    archive.AddInt("BlendMode", mBlendMode);

    Archive size;
    Archive speed;
    mSizeCurve.SaveJson(size);
    mSpeedCurve.SaveJson(speed);
    archive.AddObject("Size", size);
    archive.AddObject("Speed", speed);

    mColorGradient.Save(archive);
    }

void ParticleSystemComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    Component::LoadFromJSON(data, owner);
    if (data.HasMember("Delay") && data["Delay"].IsFloat())
    {
        mDelay = data["Delay"].GetFloat();
    }
    if (data.HasMember("Duration") && data["Duration"].IsFloat())
    {
        mDuration = data["Duration"].GetFloat();
    }
    if (data.HasMember("Image") && data["Image"].IsInt())
    {
        mResourceId = data["Image"].GetInt();
        SetImage(mResourceId);
    }
    if (data.HasMember("IsLifetimeRandom") && data["IsLifetimeRandom"].IsBool())
    {
        mIsLifetimeRandom = data["IsLifetimeRandom"].GetBool();
    }
    if (data.HasMember("Lifetime") && data["Lifetime"].IsFloat())
    {
        mLifetime = data["Lifetime"].GetFloat();
    }
    if (data.HasMember("MaxLifetime") && data["MaxLifetime"].IsFloat())
    {
        mMaxLifetime = data["MaxLifetime"].GetFloat();
    }
    if (data.HasMember("Emission Rate") && data["Emission Rate"].IsFloat())
    {
        mEmissionRate = data["Emission Rate"].GetFloat();
    }
    if (data.HasMember("Speed") && data["Speed"].IsObject())
    {
        mSpeedCurve.LoadJson(data["Speed"]);
    } 
    if (data.HasMember("Size") && data["Size"].IsObject())
    {
        mSizeCurve.LoadJson(data["Size"]);
    }
    if (data.HasMember("Max Particles") && data["Max Particles"].GetInt())
    {
        mMaxParticles = data["Max Particles"].GetInt();
    }
    if (data.HasMember("Looping") && data["Looping"].IsBool())
    {
        mLooping = data["Looping"].GetBool();
    }
    if (data.HasMember("Stretched Billboard") && data["Stretched Billboard"].IsBool())
    {
        mStretchedBillboard = data["Stretched Billboard"].GetBool();
    }
    if (data.HasMember("Color Gradient") && data["Color Gradient"].IsArray())
    {
        mColorGradient.LoadFromJSON(data);
    }
    if (data.HasMember("ShapeType") && data["ShapeType"].IsInt())
    {
        mShapeType = (EmitterType)data["ShapeType"].GetInt();
    }
    if (data.HasMember("ShapeRadius") && data["ShapeRadius"].IsFloat())
    {
        mShapeRadius = data["ShapeRadius"].GetFloat();
    }
    if (data.HasMember("ShapeAngle") && data["ShapeAngle"].IsFloat())
    {
        mShapeAngle = data["ShapeAngle"].GetFloat();
    }
    if (data.HasMember("ShapeSize") && data["ShapeSize"].IsArray())
    {
        const rapidjson::Value& values = data["ShapeSize"];
        float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
        if (values.Size() == 3 && values[0].IsFloat() && values[1].IsFloat() && values[2].IsFloat())
        {
            x = values[0].GetFloat();
            y = values[1].GetFloat();
            z = values[2].GetFloat();
        }
        mShapeSize = float3(x, y, z);
    }
    if (data.HasMember("BlendMode") && data["BlendMode"].IsInt())
    {
        mBlendMode = data["BlendMode"].GetInt();
    }

}

void ParticleSystemComponent::Enable()
{
    App->GetOpenGL()->AddParticleSystem(this);
    mEmitterTime = 0.0f;
}

void ParticleSystemComponent::Disable()
{
    App->GetOpenGL()->RemoveParticleSystem(this);
    for (Particle* particle : mParticles)
    {
        delete particle;
    }
    mParticles.clear();
}

float3 ParticleSystemComponent::ShapeInitPosition() const
{
    switch (mShapeType)
    {
    case EmitterType::CONE:
    {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float angle = r * 2 * math::pi;
        r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float distance = r * mShapeRadius;
        float x = distance * cos(angle);
        float y = distance * sin(angle);

        return float3(x, y, 0);
        break;
    }
    case EmitterType::SQUARE:
    {
        float randX = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        randX -= 0.5f;
        float randY = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        randY -= 0.5f;
        return float3(mShapeSize.x * randX, mShapeSize.y * randY, 0);
        break;
    }
    case EmitterType::CIRCLE:
    {
        float r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float angle = r * 2 * math::pi;
        r = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
        float distance = r * mShapeRadius;
        float x = distance * cos(angle);
        float y = distance * sin(angle);

        return float3(x, y, 0);
        break;
    }
    default:
        return float3(1.0f, 1.0f, 1.0f);
    }
}

float3 ParticleSystemComponent::ShapeInitDirection() const
{
    switch (mShapeType)
    {
    case EmitterType::CONE:
    {
        float r1 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float theta = r1 * 2 * math::pi; // Angle in XY plane

        // Instead of a linear distribution for angleB, we use an angular distribution.
        float r2 = static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
        float phi = std::acos(1 - r2 * (1 - std::cos(mShapeAngle))); // Angle from Z axis

        float x = std::sin(phi) * std::cos(theta);
        float y = std::sin(phi) * std::sin(theta);
        float z = std::cos(phi);

        return float3(x, y, z).Normalized(); // Normalize to ensure unit length        break;
    }
    //case EmitterType::SQUARE:
    //    break;
    //case EmitterType::CIRCLE:
    //    break;
    //case EmitterType::NONE:
    //    break;
    default:
        return float3(0, 0, 1);
    }
}