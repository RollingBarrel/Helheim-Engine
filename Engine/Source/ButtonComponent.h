#pragma once
#include "Component.h"

class ButtonComponent : public Component
{
public:
    ButtonComponent(GameObject* owner);
    ButtonComponent(GameObject* owner, bool active);
    ~ButtonComponent();

    void Reset() override {}
    void Update() override {}
    Component* Clone(GameObject* owner) const override;

    void Save(Archive& archive) const override;
    void LoadFromJSON(const rapidjson::Value& data, GameObject* owner) override;

    void OnClicked() const;

private:

};
