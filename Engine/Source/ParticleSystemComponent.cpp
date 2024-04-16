#include "ParticleSystemComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"

ParticleSystemComponent::ParticleSystemComponent(GameObject* ownerGameObject)
{
}

ParticleSystemComponent::ParticleSystemComponent(const ParticleSystemComponent& original, GameObject* owner)
{
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
    for (unsigned int i = 0; i < 10; ++i)
        this->particles.push_back(Particle());
}


void ParticleSystemComponent::Draw() const{
    unsigned int programId = App->GetOpenGL()->GetParticleProgramId();
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);
    glUseProgram(programId);
    for (Particle particle : particles)
    {
        if (particle.getLifetime() > 0.0f)
        {
            glUniform2f(glGetUniformLocation(programId, "offset"), particle.getPosition().x, particle.getPosition().y);
            glUniform4f(glGetUniformLocation(programId, "color"), particle.getColor().x, particle.getColor().y, particle.getColor().z, particle.getColor().w);
            //particleTexture.Bind();
            glBindVertexArray(mVAO);
            glDrawArrays(GL_TRIANGLES, 0, 6);
            glBindVertexArray(0);
        }
    }
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void ParticleSystemComponent::Update() 
{
	mEmitterTime += App->GetGameDt();
	mEmitterDeltaTime += App->GetGameDt();

	for (int i = 0; i < particles.size(); i++)
	{
		particles[i].Update();
	}

	if (mEmitterDeltaTime > 1 / mEmissionRate)// deltaTime in seconds Use Timer
	{
		mEmitterDeltaTime = mEmitterDeltaTime - 1 / mEmissionRate;
		if (particles.size() < mMaxParticles) 
		{
			float3 position = mShapeType.RandomInitPosition();
            float3 direction = mShapeType.RandomInitDirection();
            float random = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
            float rotation = (random * 3.1415 / 2) - (3.1415 / 4);
			particles.push_back(Particle(position, direction, rotation, mLifeTime, mSpeed));
		}
	}
}

