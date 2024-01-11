#include "GameObject.h"
#include "Globals.h"
#include "Application.h"
#include "ModuleScene.h"
#include "Component.h"
#include "TestComponent.h"


class TestSceneGameObjects {
public:
	TestSceneGameObjects() {}
	//~TestSceneGameObjects() {}

	GameObject* TestScene() {
		LOG("TestScene\n");

		return App->GetScene()->GetRoot();
	}
	GameObject* TestSceneWithGameObject() {
		LOG("TestSceneWithGameObject\n");
		GameObject* scene = App->GetScene()->GetRoot();

		GameObject gameObject = GameObject("Test Game Object 1", scene);

		Component* newComponent = new TestComponent(&gameObject);

		scene->AddChild(&gameObject);

		return scene;

	}
	GameObject* TestSceneWithGameObjects() {
		LOG("TestSceneWithGameObjects\n");

		GameObject* scene = App->GetScene()->GetRoot();

		GameObject gameObject = GameObject("Test Game Object 1", scene);

		Component* newComponent = new TestComponent(&gameObject);

		GameObject gameObject2 = GameObject("Test Game Object 2", scene);
		Component* newComponent2 = new TestComponent(&gameObject2);
		Component* newComponent3 = new TestComponent(&gameObject2);

		scene->AddChild(&gameObject);
		scene->AddChild(&gameObject2);

	}
};