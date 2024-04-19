#include "ParticleSystemComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "ColorGradient.h"
#include "ModuleCamera.h"
#include "CameraComponent.h"
#include "Resource.h"
#include "ModuleResource.h"
#include "ResourceTexture.h"

ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject) : Component(ownerGameObject, ComponentType::PARTICLESYSTEM)
{
    SetImage(mResourceId);
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner) : ParticleSystemComponent(owner)
{
}

Component* ParticleSystemComponent::Clone(GameObject* owner) const
{
    return new ParticleSystemComponent(*this, owner);
}

void ParticleSystemComponent::Init()
{
    // set up mesh and attribute properties
    unsigned int VBO;
    float particleQuad[] = {
        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f,

        0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f
    };
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(mVAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(particleQuad), particleQuad, GL_STATIC_DRAW);
    // set mesh attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glBindVertexArray(0);

    // create this->amount default particle instances
    for (unsigned int i = 0; i < 100; ++i)
        this->particles.push_back(Particle());

    App->GetOpenGL()->AddParticleSystem(this);
}


void ParticleSystemComponent::Draw() const
{
    if (IsEnabled()) 
    {
        float4x4 viewproj = ((CameraComponent*)App->GetCamera()->GetCurrentCamera())->GetProjectionMatrix() * ((CameraComponent*)App->GetCamera()->GetCurrentCamera())->GetViewMatrix();
        unsigned int programId = App->GetOpenGL()->GetParticleProgramId();
        glBlendFunc(GL_SRC_ALPHA, GL_ONE);
        glUseProgram(programId);
        for (Particle particle : particles)
        {
            if (particle.getLifetime() > 0.0f)
            {
                glUniform3f(glGetUniformLocation(programId, "offset"), particle.getPosition().x, particle.getPosition().y, particle.getPosition().y);
                glUniform4f(glGetUniformLocation(programId, "color"), particle.getColor().x, particle.getColor().y, particle.getColor().z, particle.getColor().w);
                glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_TRUE, &viewproj[0][0]);
                glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());
                glBindVertexArray(mVAO);
                glDrawArrays(GL_TRIANGLES, 0, 6);
                glBindTexture(GL_TEXTURE_2D, 0);
                glBindVertexArray(0);
            }
        }
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
}

void ParticleSystemComponent::Update()
{
    mEmitterTime += App->GetGameDt();
    mEmitterDeltaTime += App->GetGameDt();

	for (int i = 0; i < particles.size(); i++)
	{
		bool isAlive = particles[i].Update(mEmitterDeltaTime);
        if (!isAlive)
        {
			particles.erase(particles.begin() + i);
			i--;
		}
	}

	if (mEmitterDeltaTime > 1 / mEmissionRate)// deltaTime in seconds Use Timer
	{
		mEmitterDeltaTime = mEmitterDeltaTime - 1 / mEmissionRate;
		if (particles.size() < mMaxParticles) 
		{
            // Initializes a particle with a random position, direction and rotation 
            // considering the shape of emission
			float3 position = mShapeType.RandomInitPosition();

            float3 direction = mShapeType.RandomInitDirection();

            float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rotation = (random * 3.1415 / 2) - (3.1415 / 4);

            // Create the particle and sets its speed and size 
            // considering if they are linear or curve
            Particle particle = Particle(position, direction, rotation, mLifeTime, mIsSpeedCurve, mIsSizeCurve);
            
            if (mIsSpeedCurve) particle.SetSpeedCurve(mSpeedCurve);
            else particle.SetSpeedLineal(mSpeedLineal);
            
            if (mIsSizeCurve) particle.SetSizeCurve(mSizeCurve);
            else particle.SetSize(mSizeLineal);

			particles.push_back(particle);
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
