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

		GameObject* gameObject = new GameObject("GO 1", scene);

		gameObject->CreateComponent(ComponentType::MESHRENDERER);
		//Component* newComponent = new TestComponent(gameObject);

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

		/*scene->AddChild(&gameObject);
		scene->AddChild(&gameObject2);*/

	}

	const char* TestLoadSceneWithGameObject() {
		LOG("TestLoadSceneWithGameObject");

		const char* json = "{\"Scene\": {\"GameObjects\": [{\"UID\":1, \"ParentUID\": 2, \"Name\": \"Game Object 1\"}]}}";

		return json;
	}

	const char* TestLoadSceneWithGameObjectsAndComponents() {
		LOG("TestLoadSceneWithGameObjectsAndComponents");

		const char* json = 
			"{\"Scene\": \n"
				"{\"GameObjects\": [\n"
					"{\"UID\":1, \"ParentUID\": 0, \"Name\": \"Game Object 1\", \"Translation\": [0, 1, -10], \"Rotation\": [0, 0, 0, 1], \"Scale\": [1, 1, 1],\n"
					"\"Components\": [\n"
						"{\"ComponentType\": 0, \"HasResource\": false},\n"
						"{\"ComponentType\": 1, \"FOV\": 57.295780}\n"
					"]},\n"
					"{\"UID\":3, \"ParentUID\": 0, \"Name\": \"Game Object 2\", \"Translation\": [0, 0, 0], \"Rotation\": [0, 0, 0, 1], \"Scale\": [1, 1, 1],\n"
					"\"Components\": [\n"
						"{\"ComponentType\": 1, \"HasResource\": true},\n"
						"{\"ComponentType\": 1, \"Playing\": true}\n"
					"]}\n"
				"]}\n"
			"}";

		return json;
	}

	const char* TestLoadSceneWithGameObjectsWithGameObjectsAsChildrenAndComponents() {
		LOG("TestLoadSceneWithGameObjectsWithGameObjectsAsChildrenAndComponents");

		const char* json =
			"{\"Scene\": \n"
				"{\"GameObjects\": [\n"
					"{\"UID\":1, \"ParentUID\": 0, \"Name\": \"Game Object 1\", \"Translation\": [0, 1, -10], \"Rotation\": [0, 0, 0, 1], \"Scale\": [1, 1, 1],\n"
					"\"Components\": [\n"
						"{\"ComponentType\": 0, \"MeshID\": \"519408695\", \"MaterialID\": \"20\"},\n"
						"{\"ComponentType\": 1, \"FOV\": 57.295780}\n"
					"]},\n"
					"{\"UID\":2, \"ParentUID\": 0, \"Name\": \"Game Object 2\", \"Translation\": [0, 0, 0], \"Rotation\": [0, 0, 0, 1], \"Scale\": [1, 1, 1],\n"
					"\"Components\": [\n"
						"{\"ComponentType\": 1, \"HasResource\": true},\n"
						"{\"ComponentType\": 1, \"Playing\": true}\n"
					"]},\n"
					"{\"UID\":3, \"ParentUID\": 1, \"Name\": \"Game Object 3\", \"Translation\": [0, 1, -10], \"Rotation\": [0, 0, 0, 1], \"Scale\": [1, 1, 1],\n"
					"\"Components\": [\n"
						"{\"ComponentType\": 1, \"HasResource\": false},\n"
						"{\"ComponentType\": 1, \"FOV\": 57.295780}\n"
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