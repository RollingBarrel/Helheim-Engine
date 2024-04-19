#pragma once
#include "Component.h"

class TextComponent : public Component
{
public:
    TextComponent(GameObject* owner);
    TextComponent(GameObject* owner, bool active);
    ~TextComponent();

    void Reset() override {}
    void Update() override {}
    bool CleanUp();
    Component* Clone(GameObject* owner) const override;

    void Draw();

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

private:
};
