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

		GameObject* gameObject = new GameObject("GO 1", nullptr);

		gameObject->CreateComponent(ComponentType::MESHRENDERER);
		//Component* newComponent = new TestComponent(gameObject);

		scene->AddChild(gameObject);

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

	const char* TestLoadScene() {
		LOG("TestsLoadScene");

		const char* json = "{\"Scene\": {\"hola\":\"hey\" }}";

		return json;
	}

	const char* TestLoadSceneWithGameObject() {
		LOG("TestsLoadScene");

		const char* json = "{\"Scene\": {\"GameObjects\": [{\"UID\":\"1\", \"ParentUID\": 2, \"Name\": \"Game Object 1\"}]}}";

		return json;
	}

	const char* TestLoadSceneWithGameObjectsAndComponents() {
		LOG("TestsLoadScene");

		const char* json = 
			"{\"Scene\": \n"
				"{\"GameObjects\": [\n"
					"{\"UID\":\"1\", \"ParentUID\": 2, \"Name\": \"Game Object 1\", \"Translation\": [1, 2, 3],\n"
					"\"Components\": [\n"
						"{\"ComponentType\": 1, \"HasResource\": false},\n"
						"{\"ComponentType\": 2, \"FOV\": 57.295780}\n"
					"]},\n"
					"{\"UID\":\"1\", \"ParentUID\": 2, \"Name\": \"Game Object 2\", \"Translation\": [1, 2, 3],\n"
					"\"Components\": [\n"
						"{\"ComponentType\": 3, \"HasResource\": true},\n"
						"{\"ComponentType\": 4, \"Playing\": true}\n"
					"]}\n"
				"]}\n"
			"}";

		return json;
	}

	GameObject* GenerateEmptyScene() {
		return App->GetScene()->GetRoot();
	}

	GameObject* GenerateSceneWithGameObject() {
		GameObject* scene = App->GetScene()->GetRoot();

		GameObject* gameObject = new GameObject("GO 1", nullptr);

		gameObject->CreateComponent(ComponentType::MESHRENDERER);
		//Component* newComponent = new TestComponent(gameObject);

		scene->AddChild(gameObject);

		return scene;
	}

	GameObject GenerateSceneWithGameObjectsWithComponents() {
		GameObject* scene = App->GetScene()->GetRoot();

		GameObject gameObject = GameObject("Test Game Object 1", scene);

		Component* newComponent = new TestComponent(&gameObject);

		GameObject gameObject2 = GameObject("Test Game Object 2", scene);
		Component* newComponent2 = new TestComponent(&gameObject2);
		Component* newComponent3 = new TestComponent(&gameObject2);

		scene->AddChild(&gameObject);
		scene->AddChild(&gameObject2);

		return scene;
	}
};