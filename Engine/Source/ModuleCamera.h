#pragma once
#include "Module.h"

// Include only the necessary headers from MathGeoLib
#include "Math/float2.h"
#include "Math/float3.h"
#include "Math/float4x4.h"
#include "Geometry/Frustum.h"

typedef uint8_t Uint8;

enum CameraType
{
	fixed,
	movable,
	orbit
};

class ModuleCamera : public Module
{
public:
	ModuleCamera();
	~ModuleCamera();


	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	update_status PostUpdate() override;
	bool CleanUp() override;
	void move(const float3& delta);
	void moveForward(bool backwards);
	void rotate(float angle, const float3& axis);

	void setCameraType(CameraType type) { cameraType = type; }

	const float4x4& GetViewProjMatrix() const { return frustum.ViewProjMatrix(); }
	const float4x4& GetInvViewProjMatrix() const { float4x4 vpMat = frustum.ViewProjMatrix(); vpMat.Inverse(); return vpMat; }
	const float3& GetCameraPos() const { return frustum.pos; }
	void ProcessInput(const Uint8* keyboard, const int x, const int y);
	void UpdateProjectionMatrix(int screenWidth, int screenHeight);
	void DisplaceFromBoundingBox(float3 mins, float3 maxs);


private:
	Frustum frustum;
	float2 mousePos;
	CameraType cameraType = fixed;
};

