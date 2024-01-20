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
	Clear();
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
				material->SetTemporalID(mTemporalID);
				material->LoadMaterial(srcModel, srcModel.materials[materialID]);
				mesh->SetMaterial(material);
			}
		}
	}
}

void MeshRendererComponent::LoadByIDTemporal(const int id)
{
	mTemporalID = id;
	switch (id) {
		case 0:
			Load("Assets\\Models\\Clock\\Clock.gltf");
			break;
		case 1:
			Load("Assets\\Models\\DollHouse\\Dollhouse.gltf");
			break;
		case 2:
			Load("Assets\\Models\\Drawers\\Drawers.gltf");
			break;
		case 3:
			Load("Assets\\Models\\Duck\\Duck.gltf");
			break;
		case 4:
			Load("Assets\\Models\\Firetruck\\Firetruck.gltf");
			break;
		case 5:
			Load("Assets\\Models\\Floor\\Floor.gltf");
			break;
		case 6:
			Load("Assets\\Models\\Hearse\\Hearse.gltf");
			break;
		case 7:
			Load("Assets\\Models\\Player\\Player.gltf");
			break;
		case 8:
			Load("Assets\\Models\\SpinningTop\\SpinningTop.gltf");
			break;
		case 9:
			Load("Assets\\Models\\testing\\Robot\\Robot.gltf");
			break;
		case 10:
			Load("Assets\\Models\\Wall\\Wall.gltf");
			break;
		case 11:
			Load("Assets\\Models\\ZomBunny\\Zombunny.gltf");
			break;
		default:
			// Handle the case when the ID is not found
			break;
		}
}

void MeshRendererComponent::Clear()
{
	mMeshes.clear();
}