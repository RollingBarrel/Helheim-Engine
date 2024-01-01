#include "MeshRendererComponent.h"

MeshRendererComponent::MeshRendererComponent()
{
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original)
{
}

void MeshRendererComponent::Draw()
{

}

void MeshRendererComponent::Load()
{
    LoadVBO();
}

void MeshRendererComponent::Update()
{
    Draw();
}

void MeshRendererComponent::DrawEditor()
{
}

Component* MeshRendererComponent::Clone()
{
    return new MeshRendererComponent(*this);
}

void MeshRendererComponent::LoadVBO()
{

}

void MeshRendererComponent::LoadEBO()
{
}

void MeshRendererComponent::LoadVAO()
{
}
