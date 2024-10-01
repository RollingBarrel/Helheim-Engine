#include "ImageComponent.h"
#include "Application.h"
#include "ModuleOpenGL.h"
#include "ModuleCamera.h"

#include "ModuleResource.h"
#include "ResourceTexture.h"

#include "CanvasComponent.h"
#include "Transform2DComponent.h"
#include "MaskComponent.h"
#include "CameraComponent.h"

#include "Timer.h"

#include "glew.h"
#include "Math/TransformOps.h"


ImageComponent::ImageComponent(GameObject* owner) : Component(owner, ComponentType::IMAGE)
{
	SetImage(148626881);
	mTransform = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));

	GameObject* canvas = FindCanvasOnParents(this->GetOwner());
	if (canvas) mCanvas = (CanvasComponent*)(canvas->GetComponent(ComponentType::CANVAS));

	GameObject* parent = owner->GetParent();
	if (parent) mMaskComponent = static_cast<MaskComponent*>(parent->GetComponent(ComponentType::MASK));
	if (mMaskComponent) mMask = mMaskComponent->GetMask();

	//If the object has a mask component, set this image as the mask
	Component* mask = owner->GetComponent(ComponentType::MASK);
	if (mask) static_cast<MaskComponent*>(mask)->SetMask(this);
}

ImageComponent::ImageComponent(const ImageComponent& original, GameObject* owner) : Component(owner, ComponentType::IMAGE),
mFileName(original.mFileName), mColor(original.mColor), mAlpha(original.mAlpha), mTexOffset(original.mTexOffset),
mHasDiffuse(original.mHasDiffuse), mMantainRatio(original.mMantainRatio), mShouldDraw(original.mShouldDraw), mIsMaskable(original.mIsMaskable),
mIsSpritesheet(original.mIsSpritesheet), mColumns(original.mColumns), mRows(original.mRows), mCurrentFrame(original.mCurrentFrame), mElapsedTime(original.mElapsedTime),
mFPS(original.mFPS), mIsPlaying(original.mIsPlaying)
{
	if (original.mImage) SetImage(original.mImage->GetUID());

	mTransform = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));

	GameObject* canvas = FindCanvasOnParents(this->GetOwner());
	if (canvas) mCanvas = static_cast<CanvasComponent*>(canvas->GetComponent(ComponentType::CANVAS));

	GameObject* parent = owner->GetParent();
	if (parent) mMaskComponent = static_cast<MaskComponent*>(parent->GetComponent(ComponentType::MASK));
	if (mMaskComponent) mMask = mMaskComponent->GetMask();

	Component* mask = owner->GetComponent(ComponentType::MASK);
	if (mask) static_cast<MaskComponent*>(mask)->SetMask(this);
}

ImageComponent:: ~ImageComponent()
{
	if (mImage)
	{
		App->GetResource()->ReleaseResource(mImage->GetUID());
		mImage = nullptr;
	}

	mCanvas = nullptr;
	mTransform = nullptr;
}

GameObject* ImageComponent::FindCanvasOnParents(GameObject* gameObject)
{
	if (!gameObject) return nullptr;

	GameObject* currentObject = gameObject;

	while (currentObject)
	{
		if (currentObject->GetComponent(ComponentType::CANVAS) != nullptr)
		{
			return currentObject;
		}
		currentObject = currentObject->GetParent();
	}

	return nullptr;
}

void ImageComponent::Draw()
{
	if (mIsMaskable && mMask)
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
	}

	if (mImage && mCanvas && mShouldDraw)
	{
		glEnable(GL_BLEND);
		glDepthMask(GL_TRUE);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		unsigned int UIImageProgram = App->GetOpenGL()->GetUIImageProgram();
		assert(UIImageProgram);
		glUseProgram(UIImageProgram);

		float4x4 proj = float4x4::identity;
		float4x4 model = float4x4::identity;
		float4x4 view = float4x4::identity;

		switch (mCanvas->GetRenderSpace())
		{
		case RenderSpace::Screen: //Ortographic Mode
		{
			Transform2DComponent* component = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
			if (component != nullptr)
			{
				model = component->GetGlobalMatrix();

				//float2 windowSize = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->GetWindowsSize();
				float2 canvasSize = ((CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS)))->GetSize();

				model = float4x4::Scale(1.0f / canvasSize.x * 2.0f, 1.0f / canvasSize.y * 2.0f, 0.0f) * model;

			}
			glEnable(GL_CULL_FACE);
			glDisable(GL_DEPTH_TEST);
			break;
		}
		case RenderSpace::World: //World Mode
		{
			const CameraComponent* camera = App->GetCamera()->GetCurrentCamera();

			proj = camera->GetProjectionMatrix();
			model = GetOwner()->GetWorldTransform();
			view = camera->GetViewMatrix();
			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
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
			model = { float4(right, 0.0f), float4(up, 0.0f),float4(norm, 0.0f),float4(pos, 1.0f) };
			model = model * scaleMatrix;
			//model.Transpose();

			glDisable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
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
			glEnable(GL_DEPTH_TEST);
			break;
		}
		}


		glBindVertexArray(App->GetOpenGL()->GetQuadVAO());

		glUniform4fv(glGetUniformLocation(UIImageProgram, "inputColor"), 1, float4(mColor, mAlpha).ptr());

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glBindTexture(GL_TEXTURE_2D, 0);
		glBindVertexArray(0);

		glDisable(GL_BLEND);
		glEnable(GL_CULL_FACE);
		glFrontFace(GL_CCW);
	}
	if (mIsMaskable) glDisable(GL_STENCIL_TEST);
}

void ImageComponent::BlurDraw()
{
	if (mCanvas && mShouldDraw && mAlpha > 0.0f && mBlurBackground && (mCanvas->GetRenderSpace() == RenderSpace::Screen))
	{
		glUseProgram(App->GetOpenGL()->GetUICopyBlurTexProgram());

		float4x4 proj = float4x4::identity;
		float4x4 model = float4x4::identity;
		float4x4 view = float4x4::identity;

		Transform2DComponent* component = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
		if (component != nullptr)
		{
			model = component->GetGlobalMatrix();

			//float2 windowSize = ((ScenePanel*)App->GetEditor()->GetPanel(SCENEPANEL))->GetWindowsSize();
			float2 canvasSize = ((CanvasComponent*)(FindCanvasOnParents(this->GetOwner())->GetComponent(ComponentType::CANVAS)))->GetSize();

			model = float4x4::Scale(1.0f / canvasSize.x * 2.0f, 1.0f / canvasSize.y * 2.0f, 0.0f) * model;

		}

		glUniform4fv(3, 1, float4(mColor, mAlpha).ptr());

		glBindTexture(GL_TEXTURE_2D, mImage->GetOpenGLId());

		glBindVertexArray(App->GetOpenGL()->GetQuadVAO());

		glUniformMatrix4fv(0, 1, GL_TRUE, &model[0][0]);
		glUniformMatrix4fv(1, 1, GL_TRUE, &view[0][0]);
		glUniformMatrix4fv(2, 1, GL_TRUE, &proj[0][0]);

		glDrawArrays(GL_TRIANGLES, 0, 6);
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
	obj.AddBool("ShouldDraw", mShouldDraw);
	obj.AddBool("IsMaskable", mIsMaskable);
	obj.AddBool("IsSpritesheet", mIsSpritesheet);
	obj.AddInt("Columns", mColumns);
	obj.AddInt("Rows", mRows);
	obj.AddInt("Speed", mFPS);
	obj.AddBool("IsPlaying", mIsPlaying);
	obj.AddBool("BlurBack", mBlurBackground);
}

void ImageComponent::Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap)
{
	Component::Load(data, uidPointerMap);

	if (data.HasMember("ImageID")) SetImage(data.GetInt("ImageID"));
	if (data.HasMember("Alpha")) mAlpha = data.GetFloat("Alpha");
	if (data.HasMember("ShouldDraw")) mShouldDraw = data.GetBool("ShouldDraw");
	if (data.HasMember("IsMaskable")) mIsMaskable = data.GetBool("IsMaskable");
	if (data.HasMember("IsSpritesheet")) mIsSpritesheet = data.GetBool("IsSpritesheet");
	if (data.HasMember("Columns")) mColumns = data.GetInt("Columns");
	if (data.HasMember("Rows")) mRows = data.GetInt("Rows");
	if (data.HasMember("Speed")) mFPS = data.GetInt("Speed");
	if (data.HasMember("IsPlaying")) mIsPlaying = data.GetBool("IsPlaying");
	if (data.HasMember("Color"))
	{
		float col[3];
		data.GetFloats("Color", col);
		mColor = float3(col[0], col[1], col[2]);
	}
	if (data.HasMember("BlurBack")) mBlurBackground = data.GetBool("BlurBack");
}

void ImageComponent::SetImage(unsigned int resourceId)
{
	if (mImage) App->GetResource()->ReleaseResource(mImage->GetUID());
	mImage = static_cast<ResourceTexture*>(App->GetResource()->RequestResource(resourceId, Resource::Type::Texture));
}

void ImageComponent::FillSpriteSheetVBO()
{
	int row = mCurrentFrame / mColumns;
	int column = mCurrentFrame % mColumns;

	float uStart = (float)column / mColumns;
	float vStart = (float)row / mRows;
	float uEnd = uStart + 1.0f / mColumns;
	float vEnd = vStart + 1.0f / mRows;
}

void ImageComponent::ResizeByRatio()
{
	float originalRatio = mImage->GetWidth() / mImage->GetHeight();
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
	Transform2DComponent* component = static_cast<Transform2DComponent*>(GetOwner()->GetComponent(ComponentType::TRANSFORM2D));
	if (component)
	{
		float4x4 proj = float4x4::identity;
		float4x4 model = component->GetGlobalMatrix();
		float4x4 view = float4x4::identity;

		float2 canvasSize = mCanvas->GetSize();
		model = float4x4::Scale(1.0f / canvasSize.x * 2.0f, 1.0f / canvasSize.y * 2.0f, 0.0f) * model;

		glBindVertexArray(App->GetOpenGL()->GetQuadVAO());
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
			mElapsedTime = 0.0;
		}
	}
}

std::vector<unsigned char> ImageComponent::GetPixelData(ResourceTexture* texture)
{
	std::vector<unsigned char> pixels(texture->GetPixelsSize() * 4); // Assuming RGBA format

	glBindTexture(GL_TEXTURE_2D, texture->GetOpenGLId());
	glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels.data());

	glBindTexture(GL_TEXTURE_2D, 0);

	return pixels;
}

