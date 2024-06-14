#pragma once
#include "Component.h"
#include "float4.h"
#include <string>

class ResourceTexture;

class ENGINE_API DecalComponent : public Component
{
	friend class InspectorPanel;
public:
	DecalComponent() = delete;
	DecalComponent(const DecalComponent& other) = delete;
	explicit DecalComponent(GameObject* owner);
	DecalComponent(const DecalComponent& other, GameObject* owner);
	~DecalComponent();

	DecalComponent& operator=(const DecalComponent& other) = delete;

	unsigned int GetDiffuseId() const;
	unsigned int GetSpecularId() const;
	unsigned int GetNormalId() const;
	unsigned int GetEmisiveId() const;

	bool HasDiffuse() const { return mDiffuseTexture; };
	bool HasSpecular() const { return mSpecularTexture; };
	bool HasNormal() const { return mNormalTexture; };
	bool HasEmisive() const { return mEmisiveTexture; };

	const float4& GetDiffuseColor() const { return mDiffuseColor; }
	const float4& GetEmisiveColor() const { return mEmisiveColor; }

	bool IsSpriteSheet() const { return mIsSpriteSheet; }
	void GetSpriteSheetSize(int& rows, int& columns) const;
	void GetSpriteSheetCurrentPosition(int& row, int& column) const;
	void Play() { mIsPlaying = true; }
	void Pause() { mIsPlaying = false; }
	void Stop();

	void Reset() override;
	void Update() override;
	Component* Clone(GameObject* owner) const override;

	void Save(JsonObject& obj) const override;
	void Load(const JsonObject& data, const std::unordered_map<unsigned int, GameObject*>& uidPointerMap) override;

private:
	void Enable() override;
	void Disable() override;

	ResourceTexture* mDiffuseTexture = nullptr;
	ResourceTexture* mSpecularTexture = nullptr;
	ResourceTexture* mNormalTexture = nullptr;
	ResourceTexture* mEmisiveTexture = nullptr;

	float4 mDiffuseColor;
	float4 mEmisiveColor;

	std::string mDiffuseName = "";
	std::string mSpecularName = "";
	std::string mNormalName = "";
	std::string mEmisiveName = "";


	// Sprite Sheet
	bool mIsSpriteSheet = false;
	
	int mRows = 1;
	int mColumns = 1;

	int mCurrentRow = 0;
	int mCurrentColumn = 0;

	int mDefaultRow = 0;
	int mDefaultColumn = 0;

	long double mElapsedTime = 0;
	int mFPS = 30;
	bool mIsPlaying = false;
	bool mPlayAtStart = false;

};

