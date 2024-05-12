#include "ImageComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleScene.h"
#include "ModuleCamera.h"
#include "ModuleResource.h"
#include "ModuleWindow.h"
#include "ModuleUI.h"
#include "ModuleEditor.h"
#include "Timer.h"

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
	FillVBO();
	CreateVAO();

	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
}

ImageComponent::ImageComponent(GameObject* owner) : Component(owner, ComponentType::IMAGE) 
{
	FillVBO();
	CreateVAO();

    SetImage(mResourceId);
	GameObject* canvas = FindCanvasOnParents(this->GetOwner());
	if (canvas!= nullptr)
	mCanvas = (CanvasComponent*)(canvas->GetComponent(ComponentType::CANVAS));

	/*ButtonComponent* component = static_cast<ButtonComponent*>(owner->GetComponent(ComponentType::BUTTON));
	if (component != nullptr) 
	{
		component->AddEventHandler(EventType::PRESS, std::bind(&ImageComponent::OnPress, this));
		component->AddEventHandler(EventType::HOVER, std::bind(&ImageComponent::OnHover, this));
		component->AddEventHandler(EventType::CLICK, std::bind(&ImageComponent::OnClick, this));
	}*/
}

ImageComponent::ImageComponent(const ImageComponent& original, GameObject* owner) : Component(owner, ComponentType::IMAGE)
{
	FillVBO();
	CreateVAO();

	mImage = original.mImage;
	mResourceId = original.mResourceId;
	mFileName = original.mFileName;

	mColor = original.mColor;
	mAlpha = original.mAlpha;
	mHasAlpha = original.mHasAlpha;

	mTexOffset = original.mTexOffset;
	mHasDiffuse = original.mHasDiffuse;
	mMantainRatio = original.mMantainRatio;

	mQuadVBO = original.mQuadVBO;
	mQuadVAO = original.mQuadVAO;

	mCanvas = original.mCanvas;
}

ImageComponent:: ~ImageComponent() 
{
	CleanUp();
	mCanvas = nullptr;
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
	if (mIsSpritesheet)
	{
		FillSpriteSheetVBO();
	}
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

		glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());
		//glUniform1i(glGetUniformLocation(UIImageProgram, "hasDiffuse"), mHasDiffuse);
		//glUniform2fv(glGetUniformLocation(UIImageProgram, "offSet"), 1, mTexOffset.ptr());

		glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

		if (mAlpha < 1.0) 
		{
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		}

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}
}

Component* ImageComponent::Clone(GameObject* owner) const
{
	return new ImageComponent(*this, owner);
}

void ImageComponent::SetSpritesheetLayout(int columns, int rows)
{
	mColumns = columns;
	mRows = rows;
	mIsSpritesheet = true;
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
		
		mResourceId = imageIdValue.GetInt();
		SetImage(mResourceId);
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

void ImageComponent::FillSpriteSheetVBO()
{
	int row = mCurrentFrame / mColumns;
	int column = mCurrentFrame % mColumns;

	float uStart = (float)column / mColumns;
	float vStart = (float)row / mRows;
	float uEnd = uStart + 1.0f / mColumns;
	float vEnd = vStart + 1.0f / mRows;

	float vertices[] = {
		-0.5f,  0.5f,  uStart, vStart,   // top-left vertex
		-0.5f, -0.5f,  uStart, vEnd,     // bottom-left vertex
		 0.5f, -0.5f,  uEnd,   vEnd,     // bottom-right vertex
		 0.5f,  0.5f,  uEnd,   vStart,   // top-right vertex
		-0.5f,  0.5f,  uStart, vStart,   // top-left vertex
		 0.5f, -0.5f,  uEnd,   vEnd      // bottom-right vertex
	};

	LOG("uStart: %f, vStart: %f, uEnd: %f, vEnd: %f", uStart, vStart, uEnd, vEnd);
	LOG(" ");

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

void ImageComponent::Update()
{
	if (mIsSpritesheet)
	{
		mElapsedTime += App->GetCurrentClock()->GetDelta();
		if (mElapsedTime > mFrameDuration)
		{
			mCurrentFrame = (mCurrentFrame + 1) % (mColumns * mRows);
			if (mColumns != 0 && mRows != 0) 
			{
				mCurrentFrame = (mCurrentFrame + 1) % (mColumns * mRows);
			}
			else 
			{
				mCurrentFrame = 0;
			}
			mElapsedTime -= mFrameDuration;
		}
	}
}

bool ImageComponent::CleanUp()
{
	glDeleteBuffers(1, &mQuadVBO);
	glDeleteVertexArrays(1, &mQuadVAO);
	return true;
}
