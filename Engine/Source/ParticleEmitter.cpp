#include "ParticleEmitter.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"

void ParticleEmitter::Init()
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


void ParticleEmitter::Draw() {
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