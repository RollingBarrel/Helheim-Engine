#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleResource.h"
#include "ModuleUI.h"

#include "glew.h"
#include "ResourceTexture.h"
#include "ImporterTexture.h"

#include "GameObject.h"
#include "ImageComponent.h"
#include "CanvasComponent.h"
#include "Transform2DComponent.h"


ImageComponent::ImageComponent(GameObject* owner, bool active) : Component(owner, ComponentType::IMAGE) {
}

ImageComponent::ImageComponent(GameObject* owner) : Component(owner, ComponentType::IMAGE) {
	//mImage = Importer::Texture::Load("Assets/Textures/CesiumLogoFlat.png", 630045728);
	mImage = ( ResourceTexture *) App->GetResource()->RequestResource(630045728, Resource::Type::Texture);
}

ImageComponent:: ~ImageComponent() {
}

void ImageComponent::Draw() const
{
	unsigned int program = App->GetUI()->GetProgram();
	if (program)
	{
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(program);

		//Transform2DComponent* transform = (Transform2DComponent*) GetOwner()->GetComponent(ComponentType::TRANSFORM2D);

		float4x4 proj = App->GetCamera()->GetFrustum()->ProjectionMatrix();
		/* 
		std::pair<float, float> region = App->GetModule<ModuleEditor>()->GetAvailableRegion();
		orthoProjectionMatrix = float4x4::D3DOrthoProjLH(-1, 1, floor(region.first), floor(region.second));
		*/
		float4x4 model = float4x4::identity; //transform->GetGlobalScaledMatrix();
		float4x4 view = App->GetCamera()->GetViewMatrix();

		//CanvasComponent* canvas = (CanvasComponent*) App->GetScene()->GetCanvas()->GetComponent(ComponentType::CANVAS);
		//if (canvas)
		//{
			//canvas->RecalculateSizeAndScreenFactor();
			//float factor = canvas->GetScreenFactor();
			//view = view * float4x4::Scale(factor, factor, factor);
		//} 

		
		/*glUniformMatrix4fv(0, 1, GL_TRUE, model.ptr());
		OpenGLBuffer* cameraUniBuffer = new OpenGLBuffer(GL_UNIFORM_BUFFER, GL_STATIC_DRAW, 0, sizeof(float) * 16 * 2);
		cameraUniBuffer->UpdateData(view.Transposed().ptr(), sizeof(float) * 16, 0);
		cameraUniBuffer->UpdateData(proj.Transposed().ptr(), sizeof(float) * 16, sizeof(float) * 16);
		*/
		glBindVertexArray(App->GetUI()->GetQuadVAO());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());
		
		// TODO Bind mColor

		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 4);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		glDisable(GL_BLEND);
	}
}

Component* ImageComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void ImageComponent::Save(Archive& archive) const
{
}

void ImageComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
}
