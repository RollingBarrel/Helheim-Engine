#include "MeshRendererComponent.h"
#include "ResourceMaterial.h"
#include "imgui.h"
#include "ImporterMesh.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "glew.h"
#include "Quadtree.h"
#include "ModuleScene.h"
#include "ModuleDebugDraw.h"
#include "ModuleRenderTest.h"



MeshRendererComponent::MeshRendererComponent(GameObject* owner) 
	:Component("Mesh Renderer" ,owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh())
{

	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();
}

MeshRendererComponent::MeshRendererComponent(const MeshRendererComponent& original, GameObject* owner)
	:Component(owner->GetName()->c_str(), owner, ComponentType::MESHRENDERER), mMesh(new ResourceMesh())
{


	
	mOBB = OBB(AABB(float3(0.0f), float3(1.0f)));
	mAABB = AABB();


}

void MeshRendererComponent::Draw()
{
	if (!mInsideFrustum && App->GetScene()->GetApplyFrustumCulling())
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

	//App->GetOpenGL()->BindSceneFramebuffer();
//
	//if (mDrawBox)
	//{
	//	App->GetDebugDraw()->DrawBoundingBox(mOBB);
	//}
	//glUseProgram(App->GetTest()->GetProgramId());
	//glUniformMatrix4fv(0, 1, GL_TRUE, mOwner->GetWorldTransform().ptr());
	//glBindVertexArray(mMesh->GetVao());
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, App->GetTest()->GetDifuseTexture());
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, App->GetTest()->GetNormalTexture());
	//glDrawElements(GL_TRIANGLES, mMesh->mNumIndices, GL_UNSIGNED_INT, 0);
	//glUseProgram(0);
	//glBindVertexArray(0);
	//App->GetOpenGL()->UnbindSceneFramebuffer();
}

void MeshRendererComponent::Reset()
{

}

void MeshRendererComponent::Load(const char* uid)
{
	Importer::Mesh::Load(mMesh, uid);

	
	float3* positions = (float3*)(mMesh->GetAttributeData(Attribute::POS));

	mAABB.SetFrom(positions, mMesh->mNumVertices);

	float4x4 model = mOwner->GetWorldTransform();

	mOBB.SetFrom(mAABB, model);

}

void MeshRendererComponent::Update()
{
	mOBB.SetFrom(mAABB, mOwner->GetWorldTransform());
	Draw();
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
