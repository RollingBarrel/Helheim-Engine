#include "ParticleSystemComponent.h"
#include "Particle.h"
#include "EmitterShape.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "ColorGradient.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "Resource.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"

#define MATRICES_LOCATION 1

ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    SetImage(mResourceId);
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner) : ParticleSystemComponent(owner)
{
}

ParticleSystemComponent::~ParticleSystemComponent() 
{
    glDeleteBuffers(1, &mInstanceBuffer);
    glDeleteBuffers(1, &mVBO);
    delete mImage;
    delete mShapeType;
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
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
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
            16 * sizeof(float),
            (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(MATRICES_LOCATION + i, 1);
    }
    glBindVertexArray(0);
    // create this->amount default particle instances
    for (unsigned int i = 0; i < 100; ++i)
        this->mParticles.push_back(new Particle());

    App->GetOpenGL()->AddParticleSystem(this);
}


void ParticleSystemComponent::Draw() const
{
    if (IsEnabled()) 
    {
        glBindBuffer(GL_ARRAY_BUFFER, mVBO);

        float4x4* modelMatrices = new float4x4[mParticles.size()];
        float4* colors = new float4[mParticles.size()];
        float4x4 viewproj = ((CameraComponent*)App->GetCamera()->GetCurrentCamera())->GetProjectionMatrix() * ((CameraComponent*)App->GetCamera()->GetCurrentCamera())->GetViewMatrix();
        
        for (int i = 0; i < mParticles.size(); ++i)
        {
            if (mParticles[i]->GetLifetime() > 0.0f)
            {
                Quat rotation = Quat::identity;
                float3 scale = float3(mParticles[i]->GetSize());
                modelMatrices[i] = float4x4::FromTRS(mParticles[i]->GetPosition(), rotation, scale);
                //colors[i] = mParticles[i]->getColor();
            }
        }

        glBindBuffer(GL_ARRAY_BUFFER, mInstanceBuffer);
        glBufferData(GL_ARRAY_BUFFER, mParticles.size() * 16 * sizeof(float),
            modelMatrices, GL_DYNAMIC_DRAW);

        glBindVertexArray(mVAO);
        

        unsigned int programId = App->GetOpenGL()->GetParticleProgramId();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glUseProgram(programId);
        glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_TRUE, &viewproj[0][0]);
        glUniform4f(glGetUniformLocation(programId, "color"), 1.0f, 1.0f, 1.0f, 1.0f);
        glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());
        
        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mParticles.size());

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        glUseProgram(0);

        delete[] modelMatrices;
        delete[] colors;
    }
}

void ParticleSystemComponent::Update()
{
    mEmitterTime += App->GetGameDt();
    mEmitterDeltaTime += App->GetGameDt();

	for (int i = 0; i < mParticles.size(); i++)
	{
		bool isAlive = mParticles[i]->Update(mEmitterDeltaTime);
        if (!isAlive)
        {
			mParticles.erase(mParticles.begin() + i);
			i--;
		}
	}

	if (mEmitterDeltaTime > 1 / mEmissionRate)// deltaTime in seconds Use Timer
	{
		mEmitterDeltaTime = mEmitterDeltaTime - 1 / mEmissionRate;
		if (mParticles.size() < mMaxParticles)
		{
            // Initializes a particle with a random position, direction and rotation 
            // considering the shape of emission
			float3 position = mShapeType->RandomInitPosition();

            float3 direction = mShapeType->RandomInitDirection();

            float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rotation = (random * 3.1415 / 2) - (3.1415 / 4);

            // Create the particle and sets its speed and size 
            // considering if they are linear or curve
            Particle* particle = new Particle(position, direction, rotation, mLifeTime, mIsSpeedCurve, mIsSizeCurve);
            
            if (mIsSpeedCurve) particle->SetSpeedCurve(mSpeedCurve);
            else particle->SetSpeedLineal(mSpeedLineal);
            
            if (mIsSizeCurve) particle->SetSizeCurve(mSizeCurve);
            else particle->SetSize(mSizeLineal);

			mParticles.push_back(particle);
		}
	}
}

void ParticleSystemComponent::SetImage(unsigned int resourceId)
{
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}

void ParticleSystemComponent::Reset()
{

}

void ParticleSystemComponent::Save(Archive& archive) const
{
    Component::Save(archive);
    archive.AddFloat("Duration", mDuration);
    archive.AddFloat("Life Time", mLifeTime);
    archive.AddFloat("Emission Rate", mEmissionRate);
    archive.AddFloat("Speed", mSpeedLineal);
    archive.AddInt("Max Particles", mMaxParticles);
    archive.AddBool("Looping", mLooping);
    
    std::vector<Archive> objectArray;
    for (auto const& [time, color] : mColorGradient)
    {
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

    if (data.HasMember("Life Time") && data["Life Time"].IsFloat())
    {
        mLifeTime = data["Life Time"].GetFloat();
    }
    if (data.HasMember("Emission Rate") && data["Emission Rate"].IsFloat())
    {
        mEmissionRate = data["Emission Rate"].GetFloat();
    }
    if (data.HasMember("Speed") && data["Speed"].IsFloat())
    {
        mSpeedLineal = data["Speed"].GetFloat();
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
}
