#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleResource.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ModulePrograms.h"

#include "glew.h"
#include "ResourceTexture.h"
#include "ImporterTexture.h"

#include "GameObject.h"
#include "ImageComponent.h"
#include "CanvasComponent.h"
#include "Transform2DComponent.h"

#include "Math/TransformOps.h"

ImageComponent::ImageComponent(GameObject* owner, bool active) : Component(owner, ComponentType::IMAGE) {
}

ImageComponent::ImageComponent(GameObject* owner) : Component(owner, ComponentType::IMAGE) {
    SetImage(mResourceId);
}

ImageComponent:: ~ImageComponent() {
	CleanUp();
}

void ImageComponent::Draw()
{
    unsigned int UIImageProgram= App->GetPrograms()->GetUIImageProgram();
    if (UIImageProgram == 0) return;

	if (hasAlpha) {
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}

	glUseProgram(UIImageProgram);

	float4x4 proj = App->GetUI()->GetFrustum()->ProjectionMatrix();
	float4x4 model;

	if (App->GetUI()->GetScreenSpace()) {  //Ortographic Mode
		/*float3 OwnerTranslation = GetOwner()->GetPosition();

		float3 scaleImage = GetOwner()->GetScale();
		float scaleFloatX = scaleImage.x;
		float scaleFloatY = scaleImage.y;
		float scaledWidth = scaleFloatX * GetImage()->GetWidth();
		float scaledHeight = scaleFloatY * GetImage()->GetHeight();

		model = float4x4::FromTRS(OwnerTranslation, Quat::identity, float3(scaledWidth, scaledHeight, 1.0f));*/
		Transform2DComponent* component = reinterpret_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		if (component != nullptr) 
		{
			model = component->GetGlobalMatrix();
		}
		else 
		{
			model = float4x4::identity;
		}
	}
	else //World Mode
	{
		model = GetOwner()->GetWorldTransform();
	}

	//float4x4 model = GetOwner()->GetWorldTransform();

	float4x4 view = App->GetUI()->GetFrustum()->ViewMatrix();

	FillVBO();
	CreateVAO();

	glBindVertexArray(mQuadVAO);

	glActiveTexture(GL_TEXTURE0);

	glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());
	//glUniform1i(glGetUniformLocation(UIImageProgram, "hasDiffuse"), mHasDiffuse);
	//glUniform2fv(glGetUniformLocation(UIImageProgram, "offSet"), 1, mTexOffset.ptr());

	glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

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

Component* ImageComponent::Clone(GameObject* owner) const
{
	return nullptr;
}

void ImageComponent::Save(Archive& archive) const
{
    //archive.AddInt("ID", mID);
    archive.AddInt("ImageID", mImage->GetUID());
    archive.AddInt("ComponentType", static_cast<int>(GetType()));
}

void ImageComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    //int ID = { 0 };
    int imageId = { 0 };
    /*if (data.HasMember("ID") && data["ID"].IsInt()) {
        ID = data["ID"].GetInt();
    }*/
    if (data.HasMember("ImageID") && data["ImageID"].IsInt()) {
        imageId = data["ImageID"].GetInt();
    }

    mResourceId = imageId;
    SetImage(imageId);
}

void ImageComponent::SetImage(unsigned int resourceId) {
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}

void ImageComponent::FillVBO()
{
	float vertices[] = {
		// texture coordinates
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		-0.5f, -0.5f,  0.0f,  1.0f,   // bottom-left vertex
		0.5f, -0.5f,  1.0f,  1.0f,   // bottom-right vertex
		0.5f,  0.5f,  1.0f,  0.0f,   // top-right vertex
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		0.5f, -0.5f,  1.0f,  1.0f    // bottom-right vertex
	};
	glGenBuffers(1, &mQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
}

void ImageComponent::CreateVAO()
{
	glGenVertexArrays(1, &mQuadVAO);
	glBindVertexArray(mQuadVAO);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

bool ImageComponent::CleanUp()
{
	glDeleteBuffers(1, &mQuadVBO);
	glDeleteVertexArrays(1, &mQuadVAO);
	return true;
}
