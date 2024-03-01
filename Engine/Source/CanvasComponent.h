#pragma once
#include "Component.h"

class ImageComponent;

class CanvasComponent : public Component
{
public:
    CanvasComponent(bool active, GameObject* owner);
    ~CanvasComponent();

    void Update() override;
    void Init();
    Component* Clone(GameObject* owner) const override;
    void Reset() override;
    void Draw();

private:
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    std::pair<int, int> screenReferenceSize;
    std::pair<int, int> size;
    float screenFactor;
    ImageComponent* mImageComponent;
};

