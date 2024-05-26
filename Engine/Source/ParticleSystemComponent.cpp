#include "ParticleSystemComponent.h"
#include "Particle.h"
#include "EmitterShape.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "Resource.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"

#define MATRICES_LOCATION 2
#define COLOR_LOCATION 1


ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    SetImage(mResourceId);
    mColorGradient->AddColorGradientMark(0.5f, float4(1.0f, 0.0f, 0.0f, 1.0f));
    Init();
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner) :  
Component(owner, ComponentType::PARTICLESYSTEM), mFileName(original.mFileName), mDuration(original.mDuration), mMaxLifeTime(original.mMaxLifeTime),
mSpeedCurve(original.mSpeedCurve), mSizeCurve(original.mSizeCurve), mEmissionRate(original.mEmissionRate), mMaxParticles(original.mMaxParticles),
mLooping(original.mLooping), mShapeType(original.mShapeType), mColorGradient(new ColorGradient(*original.mColorGradient))
{
    SetImage(original.mResourceId);
    Init();
    mShape->CopyShape(*original.mShape);
    mColorGradient = new ColorGradient(*(original.mColorGradient));
}

ParticleSystemComponent::~ParticleSystemComponent() 
{
    App->GetOpenGL()->RemoveParticleSystem(this);
    glDeleteBuffers(1, &mInstanceBuffer);
    glDeleteBuffers(1, &mVBO);
    delete mShape;
    delete mColorGradient;
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
    for (unsigned int i = 0; i < 4; i++) {
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
    InitEmitterShape();
}

void ParticleSystemComponent::Draw() const
{
    if (IsEnabled()) 
    {
        unsigned int programId = App->GetOpenGL()->GetParticleProgramId();
        glDepthMask(GL_FALSE);
        glEnable(GL_BLEND);									// Enable Blending
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);					// Type Of Blending To Perform
        //glEnable(GL_TEXTURE_2D);
        //glBlendEquation(GL_FUNC_ADD);
        glUseProgram(programId);
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        const CameraComponent* cam = (const CameraComponent*)App->GetCamera()->GetCurrentCamera();
        if (cam) {
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
            mParticles[i]->SetSpeed(mSpeedCurve.GetValue(dt));
            mParticles[i]->SetSize(mSizeCurve.GetValue(dt));
            mParticles[i]->SetColor(mColorGradient->CalculateColor(dt));
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

			float3 emitionPosition = mShape->RandomInitPosition();
            float3 emitionDirection = mShape->RandomInitDirection();
            float4 auxPosition = mOwner->GetWorldTransform() * float4(emitionPosition, 1.0);
            emitionPosition = float3(auxPosition.x, auxPosition.y, auxPosition.z);
            float3 auxDirection = mOwner->GetWorldTransform().Float3x3Part() * emitionDirection;
            emitionDirection = auxDirection.Normalized();

            float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rotation = (random * 3.1415 / 2) - (3.1415 / 4);

            // Create the particle and sets its speed and size 
            // considering if they are linear or curve
            Particle* particle = new Particle(emitionPosition, emitionDirection, mColorGradient->CalculateColor(0.0f), rotation, mMaxLifeTime);
            particle->SetSpeed(mSpeedCurve.GetInitialValue());
            particle->SetSize(mSizeCurve.GetInitialValue());
            
			mParticles.push_back(particle);
		}
	}
}

void ParticleSystemComponent::SetImage(unsigned int resourceId)
{
    mResourceId = resourceId;
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}

void ParticleSystemComponent::Reset()
{
    delete mColorGradient;
    for (auto particle : mParticles)
    {
        delete particle;
    }
    delete mShape;
    *this = ParticleSystemComponent(mOwner);
}

void ParticleSystemComponent::Save(JsonObject& obj) const
{
    Component::Save(obj);
    obj.AddInt("Image", mResourceId);
    obj.AddFloat("Duration", mDuration);
    obj.AddFloat("Life Time", mMaxLifeTime);
    obj.AddFloat("Emission Rate", mEmissionRate);
    obj.AddFloat("Speed", mSpeedLineal);
    obj.AddInt("Max Particles", mMaxParticles);
    obj.AddBool("Looping", mLooping);
    obj.AddFloat("Size", mSizeLineal);
    obj.AddFloat("Speed", mSpeedLineal);
    obj.AddBool("IsSpeedCurve", mIsSpeedCurve);
    obj.AddBool("IsSizeCurve", mIsSizeCurve);
    obj.AddFloats("SizeCurve", mSizeCurve.ptr(), 4);
    obj.AddFloats("SpeedCurve", mSpeedCurve.ptr(), 4);
    obj.AddFloat("SizeFactor", mSizeCurveFactor);
    obj.AddFloat("SpeedFactor", mSpeedCurveFactor);
    mShape->Save(obj);
    
    JsonArray arr = obj.AddNewJsonArray("Color Gradient");
    for (std::map<float, float4>::const_iterator it = mColorGradient.begin(); it != mColorGradient.end(); it++)
    {
        float time = it->first;
        float4 color = it->second;
        JsonObject colorObj = arr.PushBackNewObject();
        colorObj.AddFloat("Time", time);
        colorObj.AddFloats("Color", color.ptr(), 4);
    }

    /*Component::Save(archive);
    archive.AddInt("Image", mResourceId);
    archive.AddFloat("Delay", mDelay);
    archive.AddFloat("Duration", mDuration);
    archive.AddFloat("Life Time", mMaxLifeTime);
    archive.AddFloat("Emission Rate", mEmissionRate);
    archive.AddInt("Max Particles", mMaxParticles);
    archive.AddBool("Looping", mLooping);
    archive.AddBool("Stretched Billboard", mStretchedBillboard);
    Archive size;
    Archive speed;
    mSizeCurve.SaveJson(size);
    mSpeedCurve.SaveJson(speed);
    archive.AddObject("Size", size);
    archive.AddObject("Speed", speed);
    mShape->Save(archive);
    
    mColorGradient->Save(archive);*/
}


void ParticleSystemComponent::Load(const JsonObject& data)
{
    Component::Load(data);

    mResourceId = data.GetInt("Image");
    SetImage(mResourceId);

    mDuration = data.GetFloat("Duration");
    mMaxLifeTime = data.GetFloat("Life Time");
    mEmissionRate = data.GetFloat("Emission Rate");
    mSpeedLineal = data.GetFloat("Speed");
    mSizeLineal = data.GetFloat("Size");
    mSpeedCurveFactor = data.GetFloat("SpeedFactor");
    mSizeCurveFactor = data.GetFloat("SizeFactor");
    mMaxParticles = data.GetInt("Max Particles");
    mLooping = data.GetBool("Looping");
    mIsSpeedCurve = data.GetBool("IsSpeedCurve");
    mIsSizeCurve = data.GetBool("IsSizeCurve");

    JsonArray arr = data.GetJsonArray("Color Gradient");
    for (unsigned int i = 0; i < arr.Size(); ++i)
    {
        JsonObject color = arr.GetJsonObject(i);
        float time = 0.0f;
        time = color.GetFloat("Time");
        float col[4];
        color.GetFloats("Color", col);
        mColorGradient[time] = float4(col);
    }

    InitEmitterShape();
    mShape->Load(data);

    float size[4];
    data.GetFloats("SizeCurve", size);
    mSizeCurve = float4(size);

    float speed[4];
    data.GetFloats("SpeedCurve", speed);
    mSpeedCurve = float4(speed);

    /*Component::LoadFromJSON(data, owner);
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
    if (data.HasMember("Life Time") && data["Life Time"].IsFloat())
    {
        mMaxLifeTime = data["Life Time"].GetFloat();
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
    if (data.HasMember("Max Particles") && data["Max Particles"].IsInt())
    {
        mMaxParticles = data["Max Particles"].IsInt();
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
        mColorGradient->LoadFromJSON(data);
    }
    if (data.HasMember("ShapeType") && data["ShapeType"].IsInt())
    {
        mShapeType = (EmitterShape::Type)data["ShapeType"].GetInt();
        InitEmitterShape();
        mShape->LoadFromJSON(data);
    }
*/
}

void ParticleSystemComponent::InitEmitterShape()
{
    switch (mShapeType)
    {
    case EmitterShape::Type::CONE:
        mShape = new EmitterShapeCone();
        break;
    case EmitterShape::Type::SQUARE:
        //mShape = EmitterShapeSquare();
        break;
    case EmitterShape::Type::CIRCLE:
        //mShape = EmitterShapeCircle();
        break;
    case EmitterShape::Type::NONE:
        break;
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

