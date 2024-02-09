#include "BatchManager.h"
#include "GeometryBatch.h"
#include "ImporterMesh.h"
#include "MeshRendererComponent.h"

BatchManager::BatchManager()
{
}

BatchManager::~BatchManager()
{
	for (auto batch : mBatches) {
		delete batch;
	}


}

void BatchManager::AddMeshRendererComponent(MeshRendererComponent* meshComponent) //Que asco de funcion dios como que un triple for
{

	bool batchFound = false;
	int batchPos = -1;


	for (int i = 0; i < mBatches.size() && !batchFound; ++i) 
	{
		const std::vector<Attribute*> batchAttributes = mBatches[i]->GetAttributes();
		bool sameAttibutes = true;

		for (int j = 0; j < batchAttributes.size() && sameAttibutes; ++j) 
		{
			const std::vector<Attribute*> meshAttributes = meshComponent->GetResourceMesh()->GetAttributes();
			bool found = false;

				for (int k = 0; k < meshAttributes.size() && !found; ++k) 
				{
					if (meshAttributes[k]->type == batchAttributes[j]->type)
					{
						found = true;
					}
				}
				if (!found) 
				{
					sameAttibutes = false;
				}	
		}

		if (sameAttibutes) {
			batchFound = true;
			batchPos = i;
		}
		
	}

	if (batchFound) {
		 mBatches[batchPos]->AddMesh(meshComponent);
	}
	else {

		GeometryBatch* newBatch = new GeometryBatch(meshComponent);
		mBatches.push_back(newBatch);
	}

}

void BatchManager::Draw()
{

	for (auto batch : mBatches) {
		batch->Draw();
	}

}


