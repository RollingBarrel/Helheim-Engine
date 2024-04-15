#pragma once

#include <vector>
#include "Particle.h"

enum class EmiterShape
{
    Circle,
    Cone,
    Box
};

class ParticleEmitter 
{
public:
    ParticleEmitter() {
    }
    ~ParticleEmitter() {
    }
    
    void Init();
    void Update(float deltaTime);
    void Draw();

    void addParticle(const Particle& particle) { particles.push_back(particle); }
    int getParticleCount() const { return particles.size(); }
    const Particle& getParticle(int index) const { return particles[index]; }

private:
    float3 mPosition;
    bool mRandomPosition;
    float3 mDirection;
    unsigned int mVAO;

    float mSpeed;




    float mEmissionRate;
    EmiterShape mShapeType;
    float mShapeAngle;
    float mShapeRadius;
    float2 mShapeBox;

    std::vector<Particle> particles;
    int mNextParticlePos;

};
