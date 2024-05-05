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
#include<algorithm>

#define MATRICES_LOCATION 2
#define COLOR_LOCATION 1


ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    SetImage(mResourceId);
    mColorGradient[0.0f] = float4::one;
    Init();
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner) :  
mDuration(original.mDuration), mMaxLifeTime(original.mMaxLifeTime), mFileName(original.mFileName), mIsSpeedCurve(original.mIsSpeedCurve),
mSpeedLineal(original.mSpeedLineal), mSpeedCurve(original.mSpeedCurve), mIsSizeCurve(original.mIsSizeCurve), mSizeCurve(original.mSizeCurve),
mSizeLineal(original.mSizeLineal), mEmissionRate(original.mEmissionRate), mMaxParticles(original.mMaxParticles), mLooping(original.mLooping),
mShapeType(original.mShapeType), mColorGradient(original.mColorGradient), Component(owner, ComponentType::PARTICLESYSTEM)
{
    SetImage(original.mResourceId);
    Init();
    mShape->CopyShape(*original.mShape);
}

ParticleSystemComponent::~ParticleSystemComponent() 
{
    App->GetOpenGL()->RemoveParticleSystem(this);
    glDeleteBuffers(1, &mInstanceBuffer);
    glDeleteBuffers(1, &mVBO);
    delete mShape;
    for (auto particle : mParticles)
    {
        delete particle;
    }
    mParticles.clear();
    mColorGradient.clear();
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
    // create this->amount default particle instances
    //for (unsigned int i = 0; i < 100; ++i)
    //    this->mParticles.push_back(new Particle());

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

        CameraComponent* cam = (CameraComponent*)App->GetCamera()->GetCurrentCamera();
        float4x4 projection = cam->GetViewProjectionMatrix();
        float3 norm = cam->GetFrustum().front; //(mParticles[i]->GetPosition() - cam->GetFrustum().pos).Normalized();
        float3 up = cam->GetFrustum().up;
        float3 right = up.Cross(norm).Normalized();
        //up = norm.Cross(right).Normalized();
        glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, mParticles.size() * 20 * sizeof(float),
            nullptr, GL_DYNAMIC_DRAW);
        float* ptr = (float*)(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));


        for (int j = 0; j < mParticles.size(); ++j)
        {
            int i = mParticles.size() - 1 - j;
            if (mParticles[i]->GetLifetime() > 0.0f)
            {
                
                float scale = mParticles[i]->GetSize();
                float3x3 scaleMatrix = float3x3::identity * scale;
                float3 pos = mParticles[i]->GetPosition();
                float4x4 transform = { float4(right, 0), float4(up, 0),float4(norm, 0),float4(pos, 1) };
                transform = transform * scaleMatrix;
                transform.Transpose();                
                memcpy(ptr + 20 * j, transform.ptr(), sizeof(float) * 16);
                memcpy(ptr + 20 * j + 16, mParticles[i]->GetColor().ptr(), sizeof(float) * 4);
            }
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindVertexArray(mVAO);
        
        glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_TRUE, &projection[0][0]);
        glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());
        
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mParticles.size());

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        glEnable(GL_DEPTH_TEST);
        glDepthMask(GL_TRUE);
    }
}

void ParticleSystemComponent::Update()
{
    mEmitterTime += App->GetDt();
    mEmitterDeltaTime += App->GetDt();
    float3 camPosition = App->GetCamera()->GetCurrentCamera()->GetFrustum().pos;
    //LOG("Time = %f", mEmitterTime)

	for (int i = 0; i < mParticles.size(); i++)
	{
		bool isAlive = mParticles[i]->Update(App->GetDt(), camPosition);
        if (!isAlive)
        {
			mParticles.erase(mParticles.begin() + i);
			i--;
		}
	}

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
            Particle* particle = new Particle(emitionPosition, emitionDirection, mColorGradient[0.0f], rotation, mMaxLifeTime, mIsSpeedCurve, mIsSizeCurve);
            
            if (mIsSpeedCurve) 
            {
                particle->SetSpeedCurve(mSpeedCurve);
                particle->SetSpeedFactor(mSpeedCurveFactor);
            }
            else
            {
                particle->SetSpeedLineal(mSpeedLineal);
            }
            
            if (mIsSizeCurve)
            {
                particle->SetSizeCurve(mSizeCurve);
                particle->SetSizeFactor(mSizeCurveFactor);

            }
            else
            {
                particle->SetSize(mSizeLineal);
            }

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

}

void ParticleSystemComponent::Save(Archive& archive) const
{
    Component::Save(archive);
    archive.AddInt("Image", mResourceId);
    archive.AddFloat("Duration", mDuration);
    archive.AddFloat("Life Time", mMaxLifeTime);
    archive.AddFloat("Emission Rate", mEmissionRate);
    archive.AddFloat("Speed", mSpeedLineal);
    archive.AddInt("Max Particles", mMaxParticles);
    archive.AddBool("Looping", mLooping);
    archive.AddFloat("Size", mSizeLineal);
    archive.AddFloat("Speed", mSpeedLineal);
    archive.AddBool("isSpeedCurve", mIsSpeedCurve);
    archive.AddBool("isSizeCurve", mIsSizeCurve);
    archive.AddFloat4("SizeCurve", mSizeCurve.ptr());
    archive.AddFloat4("SpeedCurve", mSpeedCurve.ptr());
    archive.AddFloat("SizeFactor", mSizeCurveFactor);
    archive.AddFloat("SpeedFactor", mSpeedCurveFactor);
    mShape->Save(archive);
    
    std::vector<Archive> objectArray;
    std::map<float, float4>::const_iterator it;

    for (it = mColorGradient.begin(); it != mColorGradient.end(); it++)
    {
        float time = it->first;
        float4 color = it->second;
        Archive colorArchive;
        colorArchive.AddFloat("Time", time);
        const float c[4] = { color.x, color.y, color.z, color.w };
        colorArchive.AddFloat4("Color", c);
        objectArray.push_back(colorArchive);
    }
    archive.AddObjectArray("Color Gradient", objectArray);
}

void ParticleSystemComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    Component::LoadFromJSON(data, owner);
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
    if (data.HasMember("Speed") && data["Speed"].IsFloat())
    {
        mSpeedLineal = data["Speed"].GetFloat();
    } 
    if (data.HasMember("Size") && data["Size"].IsFloat())
    {
        mSizeLineal = data["Size"].GetFloat();
    }
    if (data.HasMember("SpeedFactor") && data["SpeedFactor"].IsFloat())
    {
        mSpeedCurveFactor = data["SpeedFactor"].GetFloat();
    }
    if (data.HasMember("SizeFactor") && data["SizeFactor"].IsFloat())
    {
        mSizeCurveFactor = data["SizeFactor"].GetFloat();
    }
    if (data.HasMember("Max Particles") && data["Max Particles"].IsFloat())
    {
        mMaxParticles = data["Max Particles"].GetFloat();
    }
    if (data.HasMember("Max Particles") && data["Max Particles"].IsFloat())
    {
        mMaxParticles = data["Max Particles"].GetFloat();
    }
    if (data.HasMember("Looping") && data["Looping"].IsBool())
    {
        mLooping = data["Looping"].GetBool();
    }
    if (data.HasMember("isSpeedCurve") && data["isSpeedCurve"].IsBool())
    {
        mIsSpeedCurve = data["isSpeedCurve"].GetBool();
    }
    if (data.HasMember("isSizeCurve") && data["isSizeCurve"].IsBool())
    {
        mIsSizeCurve = data["isSizeCurve"].GetBool();
    }
    if (data.HasMember("Color Gradient") && data["Color Gradient"].IsArray())
    {
        const auto& colorArray = data["Color Gradient"].GetArray();
        for (unsigned int i = 0; i < colorArray.Size(); ++i)
        {
            float time = 0.0f;
            if (colorArray[i].HasMember("Time") && colorArray[i]["Time"].IsFloat())
            {
                time = colorArray[i]["Time"].GetFloat();
            }
            if (colorArray[i].HasMember("Color") && colorArray[i]["Color"].IsArray())
            {
                float colorVec[4] { 0 };
                const auto& colArray = colorArray[i]["Color"].GetArray();
                if (colArray.Size() == 4)
                for (unsigned int j = 0; j < colArray.Size(); ++j)
                {
                    if (colArray[j].IsFloat() && j < 4) {
                        colorVec[j] = colArray[j].GetFloat();
                    }
                }
                mColorGradient[time] = float4(colorVec);
            }
        }
    }
    if (data.HasMember("ShapeType") && data["ShapeType"].IsInt())
    {
        mShapeType = (EmitterShape::Type)data["ShapeType"].GetInt();
        InitEmitterShape();
        mShape->LoadFromJSON(data);
    }

    if (data.HasMember("SizeCurve") && data["SizeCurve"].IsArray())
    {
        const rapidjson::Value& values = data["SizeCurve"];
        float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };
        if (values.Size() == 4 && values[0].IsFloat() && values[1].IsFloat() && values[2].IsFloat() && values[3].IsFloat())
        {
            x = values[0].GetFloat();
            y = values[1].GetFloat();
            z = values[2].GetFloat();
            w = values[3].GetFloat();
        }

        mSizeCurve = float4(x, y, z, w);
    }
    if (data.HasMember("SpeedCurve") && data["SpeedCurve"].IsArray())
    {
        const rapidjson::Value& values = data["SpeedCurve"];
        float x{ 0.0f }, y{ 0.0f }, z{ 0.0f }, w{ 0.0f };
        if (values.Size() == 4 && values[0].IsFloat() && values[1].IsFloat() && values[2].IsFloat() && values[3].IsFloat())
        {
            x = values[0].GetFloat();
            y = values[1].GetFloat();
            z = values[2].GetFloat();
            w = values[3].GetFloat();
        }

        mSpeedCurve = float4(x, y, z, w);
    }
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
