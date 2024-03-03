#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleResource.h"
#include "ModuleWindow.h"
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
    SetImage(mResourceId);
}

ImageComponent:: ~ImageComponent() {
}



void ImageComponent::Draw(bool useOrthographicProjection) const
{
    unsigned int program = App->GetUI()->GetProgram();
    if (program)
    {
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        glUseProgram(program);

        float4x4 proj;
        if (useOrthographicProjection) {
            // Use orthographic projection matrix for UI rendering
            float screenWidth = static_cast<float>(App->GetWindow()->GetWidth());
            float screenHeight = static_cast<float>(App->GetWindow()->GetHeight());
            proj = float4x4::D3DOrthoProjLH(-1.0f, 1.0f, screenWidth, screenHeight);
        }
        else {
            // Use perspective projection matrix for regular rendering
            proj = App->GetCamera()->GetFrustum()->ProjectionMatrix();
        }

        float4x4 model = float4x4::identity;

        float4x4 view = App->GetCamera()->GetViewMatrix();

        glBindVertexArray(App->GetUI()->GetQuadVAO());

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

        // TODO Bind mColor

        glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
        glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
        glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

        glDrawArrays(GL_TRIANGLES, 0, 6);

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

void ImageComponent::SetImage(unsigned int resourceId) {
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}
