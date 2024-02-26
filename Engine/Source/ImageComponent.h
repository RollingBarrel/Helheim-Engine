#pragma once
#include "Component.h"

class ImageComponent :
    public Component
{
public:
    ImageComponent(bool active, GameObject* owner);
    ~ImageComponent();

    void Reset() override {}
    void Update() override {}
    Component* Clone(GameObject* owner) const override;

    void Draw() const;

private:
    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;
};
