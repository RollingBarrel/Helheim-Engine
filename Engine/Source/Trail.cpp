#include "Trail.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"

#define POSITION_LOCATION 0
#define TEXCOORD_LOCATION 1
#define UNIFORM_LOCATION 2

void Trail::Init()
{
    // set up mesh and attribute properties
    float trailMesh[] = {
        0.0f,-0.5f,0.0f,    0.0f,0.0f,
        0.0f, 0.5f,0.0f,    0.0f,1.0f,
        0.5f, 0.5f,0.0f,    0.5f,1.0f,
        0.5f,-0.5f,0.0f,    0.5f,0.0f,
        1.0f, 0.5f,0.0f,    1.0f,1.0f,
        1.0f,-0.5f,0.0f,    1.0f,0.0f,
    };
    glGenVertexArrays(1, &mVAO);
    glGenBuffers(1, &mVBO);
    glBindVertexArray(mVAO);
    // fill mesh buffer
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(trailMesh), trailMesh, GL_STATIC_DRAW); // Probably this wont be static at the end.

    // Enable the attribute for vertex positions
    glEnableVertexAttribArray(POSITION_LOCATION);
    glVertexAttribPointer(POSITION_LOCATION, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);

    // Enable the attribute for texture coordinates
    glEnableVertexAttribArray(TEXCOORD_LOCATION);
    glVertexAttribPointer(TEXCOORD_LOCATION, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

    glGenBuffers(1, &mUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, mUBO);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(float), &gameTime, GL_DYNAMIC_DRAW); // Example: passing gameTime
    glBindBufferBase(GL_UNIFORM_BUFFER, 0, mUBO);


    App->GetOpenGL()->AddParticleSystem(this);
}

void Trail::Draw()
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


        for (int i = 0; i < mParticles.size(); ++i)
        {
            float scale = mParticles[i]->GetSize();
            float3x3 scaleMatrix = float3x3::identity * scale;
            float3 pos = mParticles[i]->GetPosition();
            float4x4 transform = { float4(right, 0), float4(up, 0),float4(norm, 0),float4(pos, 1) };
            transform = transform * scaleMatrix;
            transform.Transpose();
            memcpy(ptr + 20 * i, transform.ptr(), sizeof(float) * 16);
            memcpy(ptr + 20 * i + 16, mParticles[i]->CalculateColor().ptr(), sizeof(float) * 4);
        }
        glUnmapBuffer(GL_ARRAY_BUFFER);
        glBindVertexArray(mVAO);

        glUniformMatrix4fv(glGetUniformLocation(programId, "projection"), 1, GL_TRUE, &projection[0][0]);
        glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

        glDrawArraysInstanced(GL_TRIANGLES, 0, 6, mParticles.size());

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindVertexArray(0);
        glUseProgram(0);
        glDisable(GL_BLEND);
        glDepthMask(GL_TRUE);
    }
}