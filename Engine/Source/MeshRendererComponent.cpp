#include "MeshRendererComponent.h"
#include "ResourceMaterial.h"
#include "imgui.h"
#include "Application.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include <iostream>
#include <random>

MeshRendererComponent::MeshRendererComponent(GameObject* owner)
	:Component("Mesh Renderer", owner, ComponentType::MESHRENDERER)
{
	//Create Random BBO
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the distribution for values between -100 and 100
	std::uniform_int_distribution<int> distribution(-10, 10);

	// Generate three random values
	float rv1 = distribution(gen) / 10;
	float rv2 = distribution(gen) / 10;
	float rv3 = distribution(gen) / 10;
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1 + 1.0f, rv2 + 1.0f, rv3 + 1.0f)));

	//Load("models/testing/Cube/Cube.gltf");
	//Load("models/testing/bakerHouse/BakerHouse.gltf");
	Load("models/testing/Robot/Robot.gltf");
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(owner->GetName()->c_str(), owner, ComponentType::MESHRENDERER)
{
	//Create Random BBO
	std::random_device rd;
	std::mt19937 gen(rd());

	// Define the distribution for values between -100 and 100
	std::uniform_int_distribution<int> distribution(-100, 90);

	// Generate three random values
	float rv1 = distribution(gen) / 10;
	float rv2 = distribution(gen) / 10;
	float rv3 = distribution(gen) / 10;
	mOBB = OBB(AABB(float3(rv1, rv2, rv3), float3(rv1 + 1.0f, rv2 + 1.0f, rv3 + 1.0f)));

}

void MeshRendererComponent::Draw()
{
	if (!mInsideFrustum)
	{
		return;
	}
	if (*mDrawBox)
	{
		App->GetDebugDraw()->DrawBoundingBox(mOBB);
	}
	mInsideFrustum = false;
	for (Mesh* mesh : mMeshes)
	{

		mesh->Render(lightDir, lightColor, lightIntensity, ambientColor);
	}
}

void MeshRendererComponent::Reset()
{

}

void MeshRendererComponent::Update()
{
	//Draw();
}

Component* MeshRendererComponent::Clone(GameObject* owner)
{
	return new MeshRendererComponent(*this, owner);
}

void MeshRendererComponent::Load(const char* assetFileName)
{
	tinygltf::TinyGLTF gltfContext;
	tinygltf::Model srcModel;
	std::string error, warning;
	bool loadOk = gltfContext.LoadASCIIFromFile(&srcModel, &error, &warning, assetFileName);
	if (!loadOk)
	{
		LOG("Error loading %s: %s", assetFileName, error.c_str());
	}
	for (const auto& srcMesh : srcModel.meshes)
	{
		for (const auto& primitive : srcMesh.primitives)
		{
			Mesh* mesh = new Mesh();
			mesh->LoadVBO(srcModel, srcMesh, primitive);
			mesh->LoadEBO(srcModel, srcMesh, primitive);
			mesh->CreateVAO();
			mMeshes.push_back(mesh);

			int materialID = primitive.material;

			if (materialID != -1) {
				ResourceMaterial* material = new ResourceMaterial();
				material->LoadMaterial(srcModel, srcModel.materials[materialID]);
				mesh->SetMaterial(material);
			}
		}
	}
}

void MeshRendererComponent::Clear()
{
	mMeshes.clear();
}