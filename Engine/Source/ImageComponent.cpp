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
	mTransform = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
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
	mMaskableImage = original.mMaskableImage;
	mImageToDraw = original.mImageToDraw;
	mResourceId = original.mResourceId;

	mFileName = original.mFileName;

	mColor = original.mColor;
	mAlpha = original.mAlpha;
	mMaskedPixels = original.mMaskedPixels;

	mTexOffset = original.mTexOffset;
	mHasDiffuse = original.mHasDiffuse;
	mMantainRatio = original.mMantainRatio;

	mShouldDraw = original.mShouldDraw;
	mIsMaskable = original.mIsMaskable;

	mQuadVBO = original.mQuadVBO;
	mQuadVAO = original.mQuadVAO;

	mIsSpritesheet = original.mIsSpritesheet;
	mColumns = original.mColumns;
	mRows = original.mRows;
	mCurrentFrame = original.mCurrentFrame;
	mElapsedTime = original.mElapsedTime;
	mFPS = original.mFPS;
	mIsPlaying = original.mIsPlaying;

	mCanvas = original.mCanvas;
	mTransform = original.mTransform;
}

ImageComponent:: ~ImageComponent() 
{
	CleanUp();
	mCanvas = nullptr;
	mTransform = nullptr;
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
	mImageToDraw = mIsMaskable ? mMaskableImage : mImage;

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
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

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


		glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());
		//glUniform1i(glGetUniformLocation(UIImageProgram, "hasDiffuse"), mHasDiffuse);
		//glUniform2fv(glGetUniformLocation(UIImageProgram, "offSet"), 1, mTexOffset.ptr());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mImageToDraw->GetOpenGLId());

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
	obj.AddBool("IsSpritesheet", mIsSpritesheet);
	obj.AddInt("Columns", mColumns);
	obj.AddInt("Rows", mRows);
	obj.AddInt("Speed", mFPS);
	obj.AddBool("IsPlaying", mIsPlaying);
}

void ImageComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);
	mResourceId = data.GetInt("ImageID");
	SetImage(mResourceId);

	float col[3];
	data.GetFloats("Color", col);
	mColor = float3(col[0], col[1], col[2]);
	mAlpha = data.GetFloat("Alpha");
	mIsSpritesheet = data.GetBool("IsSpritesheet");
	mColumns = data.GetInt("Columns");
	mRows = data.GetInt("Rows");
	mFPS = data.GetInt("Speed");
	mIsPlaying = data.GetBool("IsPlaying");
}

void ImageComponent::SetImage(unsigned int resourceId) 
{
    mImage = (ResourceTexture*)App->GetResource()->RequestResource(resourceId, Resource::Type::Texture);
	mMaskedPixels = GetPixelData(mImage);
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

void ImageComponent::ApplyMask(ImageComponent* mask)
{
	LOG("Name: %s", GetOwner()->GetName().c_str());
	LOG("Screen Position: %f, %f, %f", mTransform->GetPosition().x, mTransform->GetPosition().y, mTransform->GetPosition().z); // TODO: Not the screen position

	if (mask == nullptr) return;
	if (mMaskedPixels.empty() || !mIsMaskable) return;

	// Get the pixel data of the mask
	std::vector<unsigned char> maskPixels = mask->GetPixelData(mask->mImage);

	for (int y = 0; y < mMaskableImage->GetHeight(); ++y)
	{
		for (int x = 0; x < mMaskableImage->GetWidth(); ++x)
		{
			int index = (y * mMaskableImage->GetWidth() + x) * 4; // 4 for RGBA
			if (mask->Contains(x, y))
			{
				// Set the alpha of the pixel to the alpha of the corresponding pixel in the mask
				mMaskedPixels[index + 3] = maskPixels[index + 3];
			}
			else
			{
				mMaskedPixels[index + 3] = 0;
			}
		}
	}

	// Update the texture with the modified pixel data
	glBindTexture(GL_TEXTURE_2D, mMaskableImage->GetOpenGLId());
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mMaskableImage->GetWidth(), mMaskableImage->GetHeight(), GL_RGBA, GL_UNSIGNED_BYTE, mMaskedPixels.data());
	glBindTexture(GL_TEXTURE_2D, 0);
}

void ImageComponent::UpdateMaskedImageStatus()
{
	if (mIsMaskable && mMaskableImage == nullptr)
	{
		// Create a copy of mImage
		mMaskableImage = new ResourceTexture(*mImage);
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

bool ImageComponent::Contains(int x, int y)
{
	if (mImage && mTransform)
	{
		float3 screenPos = mTransform->GetPosition(); // TODO: Not the screen position
		int adjustedX = x - screenPos.x;
		int adjustedY = y - screenPos.y;
		return adjustedX >= 0 && adjustedX < mImage->GetWidth() && adjustedY >= 0 && adjustedY < mImage->GetHeight();
	}
	return false;
}

bool ImageComponent::CleanUp()
{
	glDeleteBuffers(1, &mQuadVBO);
	glDeleteVertexArrays(1, &mQuadVAO);
	return true;
}
