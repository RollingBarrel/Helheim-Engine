#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleResource.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ModuleEditor.h"

#include "ScenePanel.h"

#include "glew.h"
#include "ResourceTexture.h"
#include "ImporterTexture.h"

#include "GameObject.h"
#include "ImageComponent.h"
#include "CanvasComponent.h"
#include "Transform2DComponent.h"
#include "CameraComponent.h"
#include "ButtonComponent.h"

#include "Math/TransformOps.h"

ImageComponent::ImageComponent(GameObject* owner, bool active) : Component(owner, ComponentType::IMAGE) 
{
	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));

	GenVBOVAO();
}

ImageComponent::ImageComponent(GameObject* owner) : Component(owner, ComponentType::IMAGE) 
{
    //SetImage(mResourceId);
	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));

	/*ButtonComponent* component = static_cast<ButtonComponent*>(owner->GetComponent(ComponentType::BUTTON));
	if (component != nullptr) 
	{
		component->AddEventHandler(EventType::PRESS, std::bind(&ImageComponent::OnPress, this));
		component->AddEventHandler(EventType::HOVER, std::bind(&ImageComponent::OnHover, this));
		component->AddEventHandler(EventType::CLICK, std::bind(&ImageComponent::OnClick, this));
	}*/
	GenVBOVAO();
}

ImageComponent::ImageComponent(const ImageComponent& original, GameObject* owner) : Component(owner, ComponentType::IMAGE)
{
	mImage = original.mImage;

	mColor = original.mColor;
	mAlpha = original.mAlpha;
	mHasAlpha = original.mHasAlpha;

	mTexOffset = original.mTexOffset;
	mHasDiffuse = original.mHasDiffuse;
	mMantainRatio = original.mMantainRatio;

	mQuadVBO = original.mQuadVBO;
	mQuadVAO = original.mQuadVAO;

	mCanvas = original.mCanvas;

	GenVBOVAO();
}

ImageComponent:: ~ImageComponent() 
{
	CleanUp();
	mCanvas = nullptr;
	if(mImage)
		App->GetResource()->ReleaseResource(mImage->GetUID());
	glDeleteVertexArrays(1, &mQuadVAO);
	glDeleteBuffers(1, &mQuadVBO);
}

void ImageComponent::GenVBOVAO()
{
	glGenVertexArrays(1, &mQuadVAO);
	glBindVertexArray(mQuadVAO);

	float vertices[] = {
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		-0.5f, -0.5f,  0.0f,  1.0f,   // bottom-left vertex
		0.5f, -0.5f,  1.0f,  1.0f,   // bottom-right vertex
		0.5f,  0.5f,  1.0f,  0.0f,   // top-right vertex
		-0.5f,  0.5f,  0.0f,  0.0f,   // top-left vertex
		0.5f, -0.5f,  1.0f,  1.0f    // bottom-right vertex
	};
	glGenBuffers(1, &mQuadVBO);
	glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);

	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));

	glBindVertexArray(0);
}

GameObject* ImageComponent::FindCanvasOnParents(GameObject* gameObject)
{
	if (gameObject == nullptr) 
	{
		return nullptr;
	}

	GameObject* currentObject = gameObject;

	while (currentObject != nullptr) 
	{
		if (currentObject->GetComponent(ComponentType::CANVAS) != nullptr) 
		{
			return currentObject;
		}
		currentObject = currentObject->GetParent();
	}

	return nullptr; // No canvas found on parents
} 

void ImageComponent::Draw()
{ 
	if (mImage && mCanvas)
	{

		unsigned int UIImageProgram = App->GetOpenGL()->GetUIImageProgram();
		if (UIImageProgram == 0) return;

		if (mHasAlpha)
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glUseProgram(UIImageProgram);

		float4x4 proj = float4x4::identity;
		float4x4 model = float4x4::identity;
		float4x4 view = float4x4::identity;

		if (mCanvas->GetScreenSpace()) //Ortographic Mode
		{
			Transform2DComponent* component = reinterpret_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
			if (component != nullptr)
			{
				model = component->GetGlobalMatrix();

				//float2 windowSize = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->GetWindowsSize();
				float2 canvasSize = ((CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS)))->GetSize();

				model = float4x4::Scale(1 / canvasSize.x * 2, 1 / canvasSize.y * 2, 0) * model;

			}
			glEnable(GL_CULL_FACE);
		}
		else //World Mode
		{
			const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();

			proj = camera->GetProjectionMatrix();
			model = GetOwner()->GetWorldTransform();
			view = camera->GetViewMatrix();
			glDisable(GL_CULL_FACE);
		}
	
		glBindVertexArray(mQuadVAO);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

		glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());
		//glUniform1i(glGetUniformLocation(UIImageProgram, "hasDiffuse"), mHasDiffuse);
		//glUniform2fv(glGetUniformLocation(UIImageProgram, "offSet"), 1, mTexOffset.ptr());


		glUniformMatrix4fv(0, 1, GL_TRUE, model.ptr());
		//glUniformMatrix4fv(0, 1, GL_TRUE, float4x4::identity.ptr());
		glUniformMatrix4fv(1, 1, GL_TRUE, view.ptr());
		glUniformMatrix4fv(2, 1, GL_TRUE, proj.ptr());

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
		glUseProgram(0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		//glFrontFace(GL_CCW);
	}
}

Component* ImageComponent::Clone(GameObject* owner) const
{
	return new ImageComponent(*this, owner);
}

void ImageComponent::Save(Archive& archive) const
{
    archive.AddInt("ImageID", mImage->GetUID());
    archive.AddInt("ComponentType", static_cast<int>(GetType()));
	archive.AddFloat3("Color", mColor);
	archive.AddBool("HasAlpha", mHasAlpha);
	archive.AddFloat("Alpha", mAlpha);
}

void ImageComponent::LoadFromJSON(const rapidjson::Value& data, GameObject* owner)
{
    
    if (data.HasMember("ImageID") && data["ImageID"].IsInt()) 
	{
		const rapidjson::Value& imageIdValue = data["ImageID"];
		
		SetImage(imageIdValue.GetInt());
    }

	if (data.HasMember("Color") && data["Color"].IsArray()) 
	{
		const rapidjson::Value& colorValues = data["Color"];
		float x{ 0.0f }, y{ 0.0f }, z{ 0.0f };
		if (colorValues.Size() == 3 && colorValues[0].IsFloat() && colorValues[1].IsFloat() && colorValues[2].IsFloat()) 
		{
			x = colorValues[0].GetFloat();
			y = colorValues[1].GetFloat();
			z = colorValues[2].GetFloat();
		}

		mColor = float3(x, y, z);
	}

	if (data.HasMember("Alpha") && data["Alpha"].IsFloat()) 
	{
		const rapidjson::Value& alphaValue = data["Alpha"];
		
		mAlpha = alphaValue.GetFloat();
	}
	
	if (data.HasMember("HasAlpha") && data["HasAlpha"].IsBool()) 
	{
		const rapidjson::Value& hasAlphaValue = data["HasAlpha"];

		mHasAlpha = hasAlphaValue.GetBool();
	}
}

void ImageComponent::SetImage(unsigned int resourceId) 
{
	if (mImage != nullptr)
		App->GetResource()->ReleaseResource(mImage->GetUID());
	mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
}

void ImageComponent::ResizeByRatio()
{
	float originalRatio = mImage->GetWidth() / mImage->GetHeight() ;
	if (mCanvas->GetScreenSpace()) //Ortographic Mode
	{
		Transform2DComponent* component = ((Transform2DComponent*)GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		float currentRatio = component->GetSize().x / component->GetSize().y;
		float ratio = currentRatio / originalRatio;
		float2 newSize = float2(component->GetSize().x, component->GetSize().y * ratio);
		component->SetSize(newSize);
	}
	else 
	{
		float currentRatio = GetOwner()->GetScale().x / GetOwner()->GetScale().y;
		float ratio = currentRatio / originalRatio;
		float3 newScale = float3(GetOwner()->GetScale().x, GetOwner()->GetScale().y * ratio, GetOwner()->GetScale().z);
		GetOwner()->SetScale(newScale);
	}
}

bool ImageComponent::CleanUp()
{
	glDeleteBuffers(1, &mQuadVBO);
	glDeleteVertexArrays(1, &mQuadVAO);
	return true;
}
