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
#include "MaskComponent.h"

#include "Math/TransformOps.h"

ImageComponent::ImageComponent(GameObject* owner, bool active) : Component(owner, ComponentType::IMAGE) 
{
	FillVBO();
	CreateVAO();
	SetImage(mResourceId);

	mCanvas = (CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS));
	mTransform = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));

	GameObject* parent = GetOwner()->GetParent();
	if (parent != nullptr)
	{
		mMaskComponent = static_cast<MaskComponent*>(parent->GetComponent(ComponentType::MASK));
	}
	if (mMaskComponent != nullptr)
	{
		mMask = mMaskComponent->GetMask();
	}

	//If the object has a mask component, set this image as the mask
	Component* mask = owner->GetComponent(ComponentType::MASK);
	if (mask != nullptr)
	{
		static_cast<MaskComponent*>(mask)->SetMask(this);
	}
}

ImageComponent::ImageComponent(GameObject* owner) : Component(owner, ComponentType::IMAGE) 
{
	FillVBO();
	CreateVAO();

	SetImage(mResourceId);
	GameObject* canvas = FindCanvasOnParents(this->GetOwner());
	if (canvas != nullptr)
	{
		mCanvas = (CanvasComponent*)(canvas->GetComponent(ComponentType::CANVAS));
	}

	mTransform = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));

	GameObject* parent = GetOwner()->GetParent();
	if (parent != nullptr)
	{
		mMaskComponent = static_cast<MaskComponent*>(parent->GetComponent(ComponentType::MASK));
	}
	if (mMaskComponent != nullptr)
	{
		mMask = mMaskComponent->GetMask();
	}

	//If the object has a mask component, set this image as the mask
	Component* mask = owner->GetComponent(ComponentType::MASK);
	if (mask != nullptr)
	{
		static_cast<MaskComponent*>(mask)->SetMask(this);
	}

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

	SetImage(original.mResourceId);
	mMask = original.mMask;
	mMaskComponent = original.mMaskComponent;
	mResourceId = original.mResourceId;
	SetImage(mResourceId);
	mFileName = original.mFileName;

	mColor = original.mColor;
	mAlpha = original.mAlpha;

	mTexOffset = original.mTexOffset;
	mHasDiffuse = original.mHasDiffuse;
	mMantainRatio = original.mMantainRatio;

	mShouldDraw = original.mShouldDraw;
	mIsMaskable = original.mIsMaskable;

	//mQuadVBO = original.mQuadVBO;
	//mQuadVAO = original.mQuadVAO;

	mIsSpritesheet = original.mIsSpritesheet;
	mColumns = original.mColumns;
	mRows = original.mRows;
	mCurrentFrame = original.mCurrentFrame;
	mElapsedTime = original.mElapsedTime;
	mFPS = original.mFPS;
	mIsPlaying = original.mIsPlaying;

	mTransform = original.mTransform;
	//mCanvas = original.mCanvas;
	GameObject* canvas = FindCanvasOnParents(GetOwner());
	if (canvas != nullptr)
	{
		mCanvas = (CanvasComponent*)(canvas->GetComponent(ComponentType::CANVAS));
	}
	else
	{
		canvas = nullptr;
	}
}

ImageComponent:: ~ImageComponent() 
{
	CleanUp();
	mCanvas = nullptr;

	if (mImage)
	{
		App->GetResource()->ReleaseResource(mImage->GetUID());
	}
	mTransform = nullptr;

	App->GetResource()->ReleaseResource(mResourceId);
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
	if (mIsMaskable)
	{
		glEnable(GL_STENCIL_TEST);

		glStencilFunc(GL_ALWAYS, 1, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
		glStencilMask(0xFF);
		glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

		mMask->RenderMask();

		switch (mMaskComponent->GetMaskingMode())
		{
		case MaskComponent::MaskingMode::Normal:
		{
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			break;
		}
		case MaskComponent::MaskingMode::Inverse:
		{
			glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
			break;
		}
		}
		glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	}

	if (mIsSpritesheet)
	{
		FillSpriteSheetVBO();
		CreateVAO();
	}

	if (mImage && mCanvas && mShouldDraw)
	{
		unsigned int UIImageProgram = App->GetOpenGL()->GetUIImageProgram();
		if (UIImageProgram == 0) return;

		glEnable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glUseProgram(UIImageProgram);

		float4x4 proj = float4x4::identity;
		float4x4 model = float4x4::identity;
		float4x4 view = float4x4::identity;

		switch (mCanvas->GetRenderSpace())
		{
		case RenderSpace::Screen: //Ortographic Mode
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
			break;
		}
		case RenderSpace::World: //World Mode
		{
			const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();

			proj = camera->GetProjectionMatrix();
			model = GetOwner()->GetWorldTransform();
			view = camera->GetViewMatrix();
			glDisable(GL_CULL_FACE);
			break;
		}
		case RenderSpace::Billboard: //World Mode aligned to the camera
		{
			const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();

			proj = camera->GetProjectionMatrix();
			view = camera->GetViewMatrix();
			float3 pos = GetOwner()->GetWorldPosition();
			float3 scale = GetOwner()->GetWorldScale();
			float3x3 scaleMatrix = float3x3::identity;
			scaleMatrix[0][0] = scale.x;
			scaleMatrix[1][1] = scale.y;
			scaleMatrix[2][2] = scale.z;

			float3 norm = camera->GetFrustum().front;
			float3 up = camera->GetFrustum().up;
			float3 right = -up.Cross(norm).Normalized();
			model = { float4(right, 0), float4(up, 0),float4(norm, 0),float4(pos, 1) };
			model = model * scaleMatrix;
			//model.Transpose();

			glDisable(GL_CULL_FACE);
			break;
		}
		case RenderSpace::WorldAxisBillboard: //World Mode aligned to the camera
		{
			const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();

			proj = camera->GetProjectionMatrix();
			view = camera->GetViewMatrix();
			float3 pos = GetOwner()->GetWorldPosition();
			float3 scale = GetOwner()->GetWorldScale();
			float3x3 scaleMatrix = float3x3::identity;
			scaleMatrix[0][0] = scale.x;
			scaleMatrix[1][1] = scale.y;
			scaleMatrix[2][2] = scale.z;

			float3 norm = (pos - camera->GetFrustum().pos).Normalized();
			float3 up = float3::unitY;
			float3 right = -up.Cross(norm).Normalized();
			norm = up.Cross(right).Normalized();
			model = { float4(right, 0), float4(up, 0),float4(norm, 0),float4(pos, 1) };
			model = model * scaleMatrix;
			//model.Transpose();

			glDisable(GL_CULL_FACE);
			break;
		}
		}


		glBindVertexArray(mQuadVAO);

		glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());
		//glUniform1i(glGetUniformLocation(UIImageProgram, "hasDiffuse"), mHasDiffuse);
		//glUniform2fv(glGetUniformLocation(UIImageProgram, "offSet"), 1, mTexOffset.ptr());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());
		glUniform1i(glGetUniformLocation(UIImageProgram, "Texture"), 0);

		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

		glUseProgram(0);
		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}
	if (mIsMaskable) glDisable(GL_STENCIL_TEST);
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

void ImageComponent::PlayAnimation()
{
	mIsPlaying = true;
}

void ImageComponent::PauseAnimation()
{
	mIsPlaying = false;
}

void ImageComponent::StopAnimation()
{
	mIsPlaying = false;
	mCurrentFrame = 0;
	mElapsedTime = 0;
}

void ImageComponent::Save(JsonObject& obj) const
{
	Component::Save(obj);
	obj.AddInt("ImageID", mImage->GetUID());
	obj.AddFloats("Color", mColor.ptr(), 3);;
	obj.AddFloat("Alpha", mAlpha);
	obj.AddBool("ShouldDraw", mShouldDraw);
	obj.AddBool("IsMaskable", mIsMaskable);
	obj.AddBool("IsSpritesheet", mIsSpritesheet);
	obj.AddInt("Columns", mColumns);
	obj.AddInt("Rows", mRows);
	obj.AddInt("Speed", mFPS);
	obj.AddBool("IsPlaying", mIsPlaying);
}

void ImageComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	if (data.HasMember("ImageID"))
	{
		mResourceId = data.GetInt("ImageID");
		SetImage(mResourceId);
	}
	float col[3];
	if (data.HasMember("Color"))
	{
		data.GetFloats("Color", col);
		mColor = float3(col[0], col[1], col[2]);
	}
	if (data.HasMember("Alpha"))
	{
		mAlpha = data.GetFloat("Alpha");
	}
	if (data.HasMember("ShouldDraw"))
	{
		mShouldDraw = data.GetBool("ShouldDraw");
	}
	if (data.HasMember("IsMaskable"))
	{
		mIsMaskable = data.GetBool("IsMaskable");
	}
	if (data.HasMember("IsSpritesheet"))
	{
		mIsSpritesheet = data.GetBool("IsSpritesheet");
	}
	if (data.HasMember("Colums"))
	{
		mColumns = data.GetInt("Columns");
	}
	if (data.HasMember("Rows"))
	{
		mRows = data.GetInt("Rows");
	}
	if (data.HasMember("Speed"))
	{
		mFPS = data.GetInt("Speed");
	}
	if (data.HasMember("IsPlaying"))
	{
		mIsPlaying = data.GetBool("IsPlaying");
	}
}

void ImageComponent::SetImage(unsigned int resourceId) 
{
	if (mImage)
	{
		App->GetResource()->ReleaseResource(mImage->GetUID());
	}

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

	if (mQuadVBO == 0) 
	{
		glGenBuffers(1, &mQuadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	}
	else 
	{
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);
	}
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

	if (mQuadVBO == 0) 
	{
		glGenBuffers(1, &mQuadVBO);
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);
	}
	else 
	{
		glBindBuffer(GL_ARRAY_BUFFER, mQuadVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), &vertices);
	}
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
	if (mCanvas->GetRenderSpace() == RenderSpace::Screen) //Ortographic Mode
	{
		Transform2DComponent* component = ((Transform2DComponent*)GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		float currentRatio = component->GetSize().x / component->GetSize().y;
		float ratio = currentRatio / originalRatio;
		float2 newSize = float2(component->GetSize().x, component->GetSize().y * ratio);
		component->SetSize(newSize);
	}
	else 
	{
		float currentRatio = GetOwner()->GetWorldScale().x / GetOwner()->GetWorldScale().y;
		float ratio = currentRatio / originalRatio;
		float3 newScale = float3(GetOwner()->GetWorldScale().x, GetOwner()->GetWorldScale().y * ratio, GetOwner()->GetWorldScale().z);
		GetOwner()->SetWorldScale(newScale);
	}
}

void ImageComponent::RenderMask()
{
	unsigned int UIMaskProgram = App->GetOpenGL()->GetUIMaskProgramId();
	if (UIMaskProgram == 0) return;

	glUseProgram(UIMaskProgram);

	// Orthographic mode is used for stencil mask rendering
	Transform2DComponent* component = reinterpret_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
	if (component != nullptr)
	{
		float4x4 proj = float4x4::identity;
		float4x4 model = component->GetGlobalMatrix();
		float4x4 view = float4x4::identity;

		float2 canvasSize = mCanvas->GetSize();
		model = float4x4::Scale(1 / canvasSize.x * 2, 1 / canvasSize.y * 2, 0) * model;

		glBindVertexArray(mQuadVAO);
		glUniform4fv(glGetUniformLocation(UIMaskProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());
		glUniform1i(glGetUniformLocation(UIMaskProgram, "Texture"), 0);

		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindVertexArray(0);
	}

	glUseProgram(0);
}

void ImageComponent::Update()
{
	if (mIsSpritesheet && mIsPlaying)
	{
		mElapsedTime += App->GetCurrentClock()->GetDelta() / 1000.0f;
		float frameDuration = 1.0f / mFPS;

		if (mElapsedTime > frameDuration)
		{
			mCurrentFrame = (mCurrentFrame + 1) % (mColumns * mRows);
			mElapsedTime = 0;
		}
	}
}

std::vector<unsigned char> ImageComponent::GetPixelData(ResourceTexture* texture)
{
	int numPixels = texture->GetWidth() * texture->GetHeight();

	std::vector<unsigned char> pixels(texture->GetPixelsSize() * 4); // Assuming RGBA format

	glBindTexture(GL_TEXTURE_2D, texture->GetOpenGLId());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	glBindTexture(GL_TEXTURE_2D, 0);

	return pixels;
}

bool ImageComponent::CleanUp()
{
	if (mQuadVBO != 0)
	{
		glDeleteBuffers(1, &mQuadVBO);
	}

	if (mQuadVAO != 0)
	{
		glDeleteVertexArrays(1, &mQuadVAO);
	}
	if (mImage != nullptr)
	{
		App->GetResource()->ReleaseResource(mImage->GetUID());
	}
	return true;
}
