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

void BatchManager::AddMeshRendererComponent(MeshRendererComponent* meshComponent)
{
	const ResourceMesh* rMesh = meshComponent->GetResourceMesh();
	std::vector<Attribute> batchAttributes;
	for (int i = 0; i < mBatches.size(); ++i)
	{
		mBatches[i]->GetAttributes(batchAttributes);
		if (batchAttributes.size() == rMesh->GetNumberAttributes())
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
				mBatches[i]->AddMesh(meshComponent);
				return;
			}
		}
		batchAttributes.clear();
	}

	//Not Found
	GeometryBatch* newBatch = new GeometryBatch(meshComponent);
	mBatches.push_back(newBatch);
}

void BatchManager::Draw(const std::vector<const MeshRendererComponent*>& renderList)
{
	for (GeometryBatch* batch : mBatches) 
	{
		batch->Draw();
	}
}


