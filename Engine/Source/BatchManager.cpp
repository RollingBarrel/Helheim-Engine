#include "BatchManager.h"
#include "GeometryBatch.h"
#include "ImporterMesh.h"
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
	for (int i = 0; i < mBatches.size(); ++i)
	{
		const std::vector<Attribute*> batchAttributes = mBatches[i]->GetAttributes();
		if (batchAttributes.size() == rMesh->NumAttributes())
		{
			int j = 0;
			while (j < batchAttributes.size())
			{
				if (!rMesh->HasAttribute(batchAttributes[j]->type))
				{
					break;
				}
				++j;
			}
			if (j == batchAttributes.size())
			{
				//Found
				mBatches[i]->AddMesh(meshComponent);
				meshComponent->SetBatch(mBatches[i]);
				return;
			}
		}
	}

	//Not Found
	GeometryBatch* newBatch = new GeometryBatch(meshComponent);
	mBatches.push_back(newBatch);
	meshComponent->SetBatch(newBatch);



}

void BatchManager::Draw()
{

	for (GeometryBatch* batch : mBatches) {
		batch->Draw();
	}

}


