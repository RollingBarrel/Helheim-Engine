#include "BatchManager.h"
#include "GeometryBatch.h"
#include "ResourceMesh.h"
#include "MeshRendererComponent.h"

BatchManager::BatchManager()
{
}

BatchManager::~BatchManager()
{
	for (GeometryBatch* batch : mBatches) {
		delete batch;
	}
}

void BatchManager::AddMeshComponent(const MeshRendererComponent& meshComponent)
{
	const ResourceMesh* rMesh = meshComponent.GetResourceMesh();
	std::vector<Attribute> batchAttributes;
	for (int i = 0; i < mBatches.size(); ++i)
	{
		mBatches[i]->GetAttributes(batchAttributes);
		if (batchAttributes.size() == rMesh->GetNumberAttributes(Attribute::Usage::RENDER))
		{
			int j = 0;
			while (j < batchAttributes.size())
			{
				if (!rMesh->HasAttribute(batchAttributes[j].type))
				{
					break;
				}
				++j;
			}
			if (j == batchAttributes.size())
			{
				//Found
				mBatches[i]->AddMeshComponent(meshComponent);
				return;
			}
		}
		batchAttributes.clear();
	}

	//Not Found
	GeometryBatch* newBatch = new GeometryBatch(meshComponent);
	mBatches.push_back(newBatch);
}

void BatchManager::RemoveMeshComponent(const MeshRendererComponent& meshComponent)
{
	for (GeometryBatch* batch : mBatches)
	{
		if (batch->RemoveMeshComponent(meshComponent))
			return;
	}
}

void BatchManager::EditMaterial(const MeshRendererComponent& meshComponent)
{
	for (GeometryBatch* batch : mBatches)
	{
		if (batch->EditMaterial(meshComponent))
			return;
	}
}

void BatchManager::Update()
{
	for (GeometryBatch* batch : mBatches)
	{
		batch->Update();
	}
}

void BatchManager::Draw(const math::Frustum& frustum, unsigned int programId)
{
	for (int i = 0; i< mBatches.size(); ++i)
	{
		mBatches[i]->Draw(frustum, programId);
	}
}

void BatchManager::EndFrameDraw()
{
	for (GeometryBatch* batch : mBatches)
	{
		batch->EndFrameDraw();
	}
}