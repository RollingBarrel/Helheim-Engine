#include "InspectorPanel.h"
#include "imgui.h"
#include "Application.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleFileSystem.h"
#include "HierarchyPanel.h"
#include "TagsManagerPanel.h"
#include "GameObject.h"
#include "TestComponent.h"
#include "MeshRendererComponent.h"
#include "PointLightComponent.h"
#include "SpotLightComponent.h"
#include "ScriptComponent.h"
#include "CameraComponent.h"
#include "AIAGentComponent.h"
#include "ImporterMaterial.h"
#include "Tag.h"
#include "MathFunc.h"
#include "NavMeshObstacleComponent.h"
#include "AnimationComponent.h"
#include "ModuleOpenGL.h"
#include "Script.h"
#include "AnimationController.h"

#include "ResourceMaterial.h"

InspectorPanel::InspectorPanel() : Panel(INSPECTORPANEL, true) {}

void InspectorPanel::Draw(int windowFlags)
{
	HierarchyPanel* hierarchyPanel = (HierarchyPanel*)App->GetEditor()->GetPanel(HIERARCHYPANEL);
	GameObject* focusedObject = hierarchyPanel->GetFocusedObject();

	if (mLockedGameObject != nullptr) {
		focusedObject = mLockedGameObject;
	}

	if (focusedObject == nullptr) return;

	char nameArray[100];
	strcpy_s(nameArray, focusedObject->mName.c_str());
	bool enabled = focusedObject->IsEnabled();
	ImGui::PushID(focusedObject->mID);
	ImGui::SetNextWindowPos(ImVec2(-100, 100), ImGuiCond_FirstUseEver);
	ImGui::SetNextWindowSize(ImVec2(550, 680), ImGuiCond_Once);
	ImGui::Begin(GetName(), &mOpen, windowFlags);

	if (!focusedObject->IsRoot())
	{
		if (ImGui::Checkbox("##enabled", &enabled))
		{
			focusedObject->SetEnabled(enabled);
		}
		ImGui::SameLine();


		// Rename
		ImGui::PushID(focusedObject->mID);
		ImGui::InputText("##rename", nameArray, IM_ARRAYSIZE(nameArray));
		focusedObject->mName = nameArray;
		ImGui::PopID();

		// Tag
		ImGui::Text("Tag");
		ImGui::SameLine();
		std::vector<Tag*> tags = App->GetScene()->GetAllTags();

		if (ImGui::BeginCombo("##tags", focusedObject->GetTag()->GetName().c_str()))
		{
			for (auto i = 0; i < tags.size(); i++) {
				if (ImGui::Selectable(tags[i]->GetName().c_str()))
				{
					focusedObject->SetTag(tags[i]);
				}
			}

			ImGui::EndCombo();
		}

		ImGui::SameLine();

		if (ImGui::Button("Edit")) {
			App->GetEditor()->OpenPanel(TAGSMANAGERPANEL, true);
		}

		// Lock
		ImGui::SameLine();
		if (ImGui::Checkbox("Lock", &mLocked)) {
			if (mLocked) {
				mLockedGameObject = focusedObject;
			}
			else {
				mLockedGameObject = nullptr;
			}
		}
		DrawTransform(focusedObject);
		DrawComponents(focusedObject);
		ImGui::Separator();
		AddComponentButton(focusedObject);
	}

	if (mSameComponentPopup)
	{
		ShowSameComponentPopup();
	}

	ImGui::End();
	ImGui::PopID();
}

void InspectorPanel::DrawTransform(GameObject* object) {
	ImGui::PushID(object->mID);
	bool headerOpen = ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);

	if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {

		ImGui::OpenPopup("TransformOptions");
	}
	if (ImGui::BeginPopup("TransformOptions")) {
		if (ImGui::Selectable("Reset")) {
			object->ResetTransform();
		}
		ImGui::EndPopup();
	}

	if (headerOpen) {
		if (ImGui::BeginTable("transformTable", 4)) {
			//ImGui::TableSetupColumn("columns", 0 , -FLT_MIN);

			bool modifiedTransform = false;
			float3 newPosition = object->mPosition;
			float3 newRotation = RadToDeg(object->mEulerRotation);
			float3 newScale = object->mScale;

			const char* labels[3] = { "Position", "Rotation", "Scale" };
			const char* axisLabels[3] = { "X", "Y", "Z" };
			float3* vectors[3] = { &newPosition, &newRotation, &newScale };

			for (int i = 0; i < 3; ++i) {
				ImGui::PushID(i);
				ImGui::TableNextRow();

				ImGui::TableNextColumn();
				ImGui::PushItemWidth(-FLT_MIN);
				ImGui::Text(labels[i]);
				ImGui::PopItemWidth();

				for (int j = 0; j < 3; ++j) {
					ImGui::TableNextColumn();
					ImGui::PushItemWidth(-FLT_MIN);
					ImGui::AlignTextToFramePadding();
					ImGui::Text(axisLabels[j]);
					ImGui::SameLine();
					modifiedTransform = ImGui::DragFloat(axisLabels[j], &(*vectors[i])[j], 0.05f, 0.0f, 0.0f, "%.2f") || modifiedTransform;
					ImGui::PopItemWidth();
				}
				ImGui::PopID();
			}

			if (modifiedTransform) {
				object->SetPosition(newPosition);
				object->SetRotation(DegToRad(newRotation));
				object->SetScale(newScale);
			}
		}
		ImGui::EndTable();
	}
	ImGui::PopID();
}

void InspectorPanel::AddComponentButton(GameObject* object) {
	float windowWidth = ImGui::GetWindowWidth();
	float buttonWidth = 150.0f; // Desired width for the button
	float posX = (windowWidth - buttonWidth) * 0.5f;

	ImGui::SetCursorPosX(posX);

	if (ImGui::Button("Add Component", ImVec2(buttonWidth, 0))) {
		ImGui::OpenPopup("AddComponentPopup");
	}

	if (ImGui::BeginPopup("AddComponentPopup")) {
		for (unsigned int i = 0; i <= static_cast<unsigned int>(ComponentType::NONE) - 1; ++i) {
			ComponentType currentComponent = static_cast<ComponentType>(i);
			if (ImGui::MenuItem(Component::GetNameFromType(currentComponent))) {
				mComponent = object->GetComponent(currentComponent);
				if (!mComponent)
				{
					object->CreateComponent(currentComponent);
				}
				else {
					mSameComponentPopup = true;
				}
			}
		}
		ImGui::EndPopup();
	}
}

void InspectorPanel::ShowSameComponentPopup()
{
	ImGuiViewport* mainViewport = ImGui::GetMainViewport();
	ImVec2 centerPos = ImVec2(mainViewport->Pos.x + mainViewport->Size.x * 0.5f,
		mainViewport->Pos.y + mainViewport->Size.y * 0.5f);

	ImGui::SetNextWindowPos(centerPos, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
	ImGui::SetNextWindowSize(ImVec2(380, 107), ImGuiCond_Appearing);

	ImGui::Begin("Can't add the same component multiple times!", &mSameComponentPopup,
		ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoScrollbar);

	ImGui::Text("The component %s can't be added because", Component::GetNameFromType(mComponent->GetType()));
	ImGui::Text("GameObject already contains the same component.");

	ImGui::Spacing();
	ImGui::Spacing();
	ImGui::Spacing();

	float buttonPosX = ImGui::GetWindowContentRegionMax().x - 120;

	ImGui::SetCursorPosX(buttonPosX);

	if (ImGui::Button("Cancel", ImVec2(120, 25))) {
		mSameComponentPopup = false;
	}

	ImGui::End();
}

/****************** COMPONENTS ********************/

void InspectorPanel::RightClickPopup(Component* component) {

	if (ImGui::IsItemHovered() && ImGui::IsMouseReleased(ImGuiMouseButton_Right)) {
		ImGui::OpenPopup(std::to_string(component->GetID()).c_str());
	}

	if (ImGui::BeginPopup(std::to_string(component->GetID()).c_str())) {
		if (ImGui::MenuItem("Delete Component")) {
			component->mOwner->AddComponentToDelete(component);
			ImGui::CloseCurrentPopup();
		}
		if (ImGui::MenuItem("Reset Component")) {
			component->Reset();
			ImGui::CloseCurrentPopup();
		}

		switch (component->GetType()) {
		case ComponentType::TEST: {
			if (ImGui::MenuItem("Custom Test Component Option")) {
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Custom Test Component Option 2")) {
				ImGui::CloseCurrentPopup();
			}
			break;
		}
		case ComponentType::MESHRENDERER: {
			if (ImGui::MenuItem("Custom MeshRendererComponent Option")) {
				ImGui::CloseCurrentPopup();
			}
			if (ImGui::MenuItem("Custom MeshRendererComponent Option")) {
				ImGui::CloseCurrentPopup();
			}
			break;
		}
		}

		ImGui::EndPopup();
	}

}

void InspectorPanel::DragAndDropSource(Component* component) {
	if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
	{
		ImGui::SetDragDropPayload("_COMPONENT", component, sizeof(*component));

		ImGui::Text(Component::GetNameFromType(component->GetType()));
		ImGui::EndDragDropSource();
	}
}

void InspectorPanel::DragAndDropTarget(GameObject* object, Component* target) {
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::InvisibleButton("##", ImVec2(-1, 5));
	if (ImGui::BeginDragDropTarget())
	{
		LOG("Droped payload");
		if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_COMPONENT"))
		{
			Component* movedComponent = (Component*)payload->Data;
			if (target != nullptr ? movedComponent->GetID() != target->GetID() : true) {
				Component* pMovedComponent = object->RemoveComponent(movedComponent);
				object->AddComponent(pMovedComponent, target);
			}
		}
		ImGui::EndDragDropTarget();
	}
	ImGui::PopStyleVar();
}

void InspectorPanel::DrawComponents(GameObject* object) {
	for (auto component : object->mComponents) {
		ImGui::PushID(component->mID);
		DragAndDropTarget(object, component);

		bool isOpen = ImGui::CollapsingHeader(Component::GetNameFromType(component->GetType()), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth | ImGuiTreeNodeFlags_AllowItemOverlap);
		RightClickPopup(component);

		bool isEnabled = component->IsEnabled();
		if (ImGui::Checkbox("Enable", &isEnabled)) {
			(isEnabled) ? component->Enable() : component->Disable(); //Enable and Disable of your component should change mIsEnabled value, not the inspector.
		}

		DragAndDropSource(component);
		
		if (isOpen) {
			switch (component->GetType()) {
				case ComponentType::MESHRENDERER: {
					DrawMeshRendererComponent(reinterpret_cast<MeshRendererComponent*>(component));
					break;
				}
				case ComponentType::AIAGENT: {
					DrawAIAgentComponent(reinterpret_cast<AIAgentComponent*>(component));
					break;
				}
				case ComponentType::POINTLIGHT: {
					DrawPointLightComponent(reinterpret_cast<PointLightComponent*>(component));
					break;
				}
				case ComponentType::SPOTLIGHT: {
					DrawSpotLightComponent(reinterpret_cast<SpotLightComponent*>(component));
					break;
				}
				case ComponentType::CAMERA: {
					DrawCameraComponent(reinterpret_cast<CameraComponent*>(component));
					break;
				}
				case ComponentType::SCRIPT: {
					DrawScriptComponent(reinterpret_cast<ScriptComponent*>(component));
					break;
				}
				case ComponentType::NAVMESHOBSTACLE: {
					DrawNavMeshObstacleComponent(reinterpret_cast<NavMeshObstacleComponent*>(component));
					break;
				}
				case ComponentType::ANIMATION: {
					DrawAnimationComponent(reinterpret_cast<AnimationComponent*>(component));
					break;
				}
				case ComponentType::TEST: {
					DrawTestComponent(reinterpret_cast<TestComponent*>(component));
					break;
				}
			}
		}
		ImGui::PopID();
	}
	DragAndDropTarget(object, nullptr);
}

void InspectorPanel::DrawTestComponent(TestComponent* component) {
	ImGui::Text("Demo Text");
	ImGui::Text("Demo Text 2 ");

	ImGui::SeparatorText("TAGS SYSYEM TEST");
	ImGui::Text("The first name of game object found with");
	ImGui::SameLine();
	std::vector<Tag*> tags = App->GetScene()->GetAllTags();

	if (ImGui::BeginCombo("##tags", tags[component->mTestSavedTag1]->GetName().c_str()))
	{
		for (auto i = 0; i < tags.size(); i++) {
			if (ImGui::Selectable(tags[i]->GetName().c_str()))
			{
				component->mTestSavedTag1 = i;
			}
		}

		ImGui::EndCombo();
	}
	ImGui::Text("tag is ");
	ImGui::SameLine();
	GameObject* found = GameObject::FindGameObjectWithTag(tags[component->mTestSavedTag1]->GetName());
	if (found != nullptr)
	{
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), found->GetName().c_str());
	}
	else {
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), "Not found");
	}


	ImGui::Separator();
	ImGui::Text("There is ");
	ImGui::SameLine();
	ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), std::to_string(GameObject::FindGameObjectsWithTag(tags[component->mTestSavedTag1]->GetName()).capacity()).c_str());
	ImGui::SameLine();
	ImGui::Text(" gameobjects with the same tag.");
}

void InspectorPanel::DrawPointLightComponent(PointLightComponent* component) {
	const float* pCol = component->GetColor();
	float col[3] = { pCol[0], pCol[1] , pCol[2] };
	if (ImGui::ColorPicker3("Color", col))
	{
		component->SetColor(col);
	}
	float intensity = component->GetIntensity();
	if (ImGui::DragFloat("Intensity", &intensity, 1.0f, 0.0f))
	{
		component->SetIntensity(intensity);
	}
	float radius = component->GetRadius();
	if (ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f))
	{
		component->SetRadius(radius);
	}
	ImGui::Checkbox("Debug draw", &component->debugDraw);
}

void InspectorPanel::DrawSpotLightComponent(SpotLightComponent* component) {
	const float* sCol = component->GetColor();
	float col[3] = { sCol[0], sCol[1] , sCol[2] };
	if (ImGui::ColorPicker3("Color", col))
	{
		component->SetColor(col);
	}
	const float* sDir = component->GetDirection();
	float dir[3] = { sDir[0], sDir[1] , sDir[2] };
	if (ImGui::DragFloat3("Direction", dir, 0.05f, -1.f, 1.f))
	{
		component->SetDirection(dir);
	}
	float intensity = component->GetIntensity();
	if (ImGui::DragFloat("Intensity", &intensity, 1.0f, 0.0f))
	{
		component->SetIntensity(intensity);
	}
	float radius = component->GetRadius();
	if (ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f))
	{
		component->SetRadius(radius);
	}
	float innerAngle = RadToDeg(component->GetInnerAngle());
	float outerAngle = RadToDeg(component->GetOuterAngle());
	if (ImGui::DragFloat("Inner angle", &innerAngle, 1.0f, 0.0f, outerAngle))
	{
		component->SetInnerAngle(DegToRad(innerAngle));
	}
	if (ImGui::DragFloat("Outer angle", &outerAngle, 1.0f, innerAngle, 90.f))
	{
		component->SetOuterAngle(DegToRad(outerAngle));
	}
	ImGui::Checkbox("Debug draw", &component->debugDraw);
}

void InspectorPanel::DrawMeshRendererComponent(MeshRendererComponent* component) {

	ImGui::SeparatorText("Material");

	MaterialVariables(component);

	ImGui::Text(" ");
	bool shouldDraw = component->ShouldDraw();
	if (ImGui::Checkbox("Draw bounding box:", &shouldDraw)) {
		component->SetShouldDraw(shouldDraw);
	}
}

void InspectorPanel::DrawAIAgentComponent(AIAgentComponent* component)
{
	ImGui::SeparatorText("Agent Parameters");

	float radius = component->GetRadius();
	if (ImGui::DragFloat("Radius", &radius, 1.0f, 0.0f))
	{
		component->SetRadius(radius);
	}
	float height = component->GetHeight();
	if (ImGui::DragFloat("Height", &height, 1.0f, 0.0f))
	{
		component->SetHeight(height);
	}
	float stepHeight = component->GetStepHeight();
	if (ImGui::DragFloat("StepHeight", &stepHeight, 1.0f, 0.0f))
	{
		component->SetStepHeight(stepHeight);
	}

	int maxSlope = component->GetMaxSlope();
	if (ImGui::SliderInt("Max Slope", &maxSlope, 0, 60)) {
		component->SetMaxSlope(maxSlope);
	}

	ImGui::SeparatorText("Steering Parameters");

	float speed = component->GetSpeed();
	if (ImGui::DragFloat("Speed", &speed, 1.0f, 0.0f))
	{
		component->SetSpeed(speed);
	}

	float angularSpeed = component->GetAngularSpeed();
	if (ImGui::DragFloat("Angular Speed", &angularSpeed, 1.0f, 0.0f))
	{
		component->SetAngularSpeed(angularSpeed);
	}

	float acceleration = component->GetAcceleration();
	if (ImGui::DragFloat("acceleration", &acceleration, 1.0f, 0.0f))
	{
		component->SetAcceleration(acceleration);
	}

	float stoppingDistance = component->GetStoppingDistance();
	if (ImGui::DragFloat("Stopping Distance", &stoppingDistance, 1.0f, 0.0f))
	{
		component->SetStoppingDistance(stoppingDistance);
	}



}

void InspectorPanel::MaterialVariables(MeshRendererComponent* renderComponent)
{
	ResourceMaterial* material = const_cast<ResourceMaterial*>(renderComponent->GetResourceMaterial());

	if (ImGui::Checkbox("Enable Diffuse map", &material->mEnableDiffuseTexture))
	{
		App->GetOpenGL()->BatchEditMaterial(renderComponent);
	}
	if (ImGui::Checkbox("Enable Specular map", &material->mEnableSpecularGlossinessTexture))
	{
		App->GetOpenGL()->BatchEditMaterial(renderComponent);
	}
	if (ImGui::Checkbox("Enable Shininess map", &material->mEnableShininessMap))
	{
		App->GetOpenGL()->BatchEditMaterial(renderComponent);
	}
	if (ImGui::Checkbox("Enable Normal map", &material->mEnableNormalMap))
	{
		App->GetOpenGL()->BatchEditMaterial(renderComponent);
	}

	if (!material->IsDiffuseTextureEnabled())
	{
		if (ImGui::ColorPicker3("Diffuse", material->mDiffuseFactor.ptr()))
		{
			App->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
	}
	if (!material->IsSpecularGlossinessTextureEnabled())
	{
		if (ImGui::ColorPicker3("Specular", material->mSpecularFactor.ptr()))
		{
			App->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
	}
	if (!material->IsShininessMapEnabled())
	{
		if (ImGui::DragFloat("Shininess", &material->mGlossinessFactor, 0.05f, 0.0f, 10000.0f, "%.2f"))
		{
			App->GetOpenGL()->BatchEditMaterial(renderComponent);
		}
	}
}

void InspectorPanel::DrawNavMeshObstacleComponent(NavMeshObstacleComponent* component)
{
	ImGui::SeparatorText("Navigation Mesh Obstacle");

	float Radius = component->GetRadius();
	ImGui::InputFloat("Radius", &Radius);
	component->SetRadius(Radius);

	float Height = component->GetHeight();
	ImGui::InputFloat("Height", &Height);
	component->SetHeight(Height);

}


void InspectorPanel::DrawCameraComponent(CameraComponent* component)
{
	ImGui::SeparatorText("Camera");

	float Near = component->GetNearPlane();
	float* NearBar = &Near;
	const char* NearLabel = "NearPlane";

	float Far = component->GetFarPlane();
	float* FarBar = &Far;
	const char* FarLabel = "FarPlane";

	float FOV = RadToDeg(component->GetVerticicalFOV());
	float* FOVBar = &FOV;
	const char* FOVLabel = "FOV";

	bool modifiedValue = false;

	ImGui::PushID(NearLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Near Plane");
	ImGui::SameLine();
	modifiedValue = ImGui::DragFloat(NearLabel, &(*NearBar), 0.05f, 0.0f, 2000, "%.2f") || modifiedValue;
	ImGui::PopID();

	ImGui::PushID(FarLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("Far Plane");
	ImGui::SameLine();
	modifiedValue = ImGui::DragFloat(FarLabel, &(*FarBar), 0.05f, 0.0f, 2000, "%.2f") || modifiedValue;
	ImGui::PopID();

	ImGui::PushID(FOVLabel);
	ImGui::AlignTextToFramePadding();
	ImGui::Text("FOV");
	ImGui::SameLine();
	modifiedValue = ImGui::DragFloat(FOVLabel, &(*FOVBar), 0.05f, 0.0f, 2000, "%.2f") || modifiedValue;
	ImGui::PopID();

	if (modifiedValue)
	{
		component->SetNearPlane(Near);
		component->SetFarPlane(Far);
		component->SetVerticicalFOV(DegToRad(FOV));
	}

	//ImGui::Checkbox("Enable Diffuse map", &(new bool(true)));
	// Is culling
}

void InspectorPanel::DrawScriptComponent(ScriptComponent* component)
{

	const char* currentItem = component->GetScriptName();

	if (ImGui::BeginCombo("##combo", currentItem)) // The second parameter is the label previewed before opening the combo.
	{
		std::vector<std::string> scriptNames;
		App->GetFileSystem()->DiscoverFiles(ASSETS_SCRIPT_PATH, ".emeta", scriptNames);
		for (int i = 0; i < scriptNames.size(); ++i)
		{
			// Find the position of the last occurrence of '/'
			size_t slashPos = scriptNames[i].find_last_of('/');
			if (slashPos != std::string::npos)
			{
				// Erase the part before the last '/'
				scriptNames[i].erase(0, slashPos + 1);
			}
			// Find the position of the first occurrence of '.'
			size_t dotPos = scriptNames[i].find_first_of('.');
			if (dotPos != std::string::npos)
			{
				// Erase the part starting from the first '.'
				scriptNames[i].erase(dotPos);
			}
		}

		for (int n = 0; n < scriptNames.size(); n++)
		{
			bool is_selected = (currentItem == scriptNames[n]); // You can store your selection however you want, outside or inside your objects
			if (ImGui::Selectable(scriptNames[n].c_str(), is_selected)) {
				currentItem = scriptNames[n].c_str();
				component->LoadScript(currentItem);
				currentItem = component->GetScriptName();
			}

			if (is_selected) {
				ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
			}

		}
		ImGui::EndCombo();
	}



	component->mScript;
	std::vector<std::pair<std::string, std::pair<VariableType, void*>>> variables;



	ImGui::SeparatorText("Attributes");
	
	for (ScriptVariable* variable : component->mData) { 
		switch (variable->mType)
		{
		case VariableType::INT:
			ImGui::DragInt(variable->mName, (int*)variable->mData);
			break;
		case VariableType::FLOAT:
			ImGui::DragFloat(variable->mName, (float*)variable->mData);
			break;
		case VariableType::BOOL:
			ImGui::Checkbox(variable->mName, (bool*)variable->mData);
			break;
		case VariableType::FLOAT3:
			ImGui::DragFloat3(variable->mName, (float*)variable->mData);
			break;
		case VariableType::GAMEOBJECT:
		{
			
			GameObject* go = *(GameObject**)variable->mData;
			ImGui::Text(variable->mName);
			ImGui::SameLine();
			const char* str ="";
			if (!go) {
				str = "Drop a GameObject Here";
			}
			else {
				str = go->GetName().c_str();
			}
			ImGui::BulletText(str);
			if (ImGui::BeginDragDropTarget()) {

				if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("_TREENODE")) {
					*(GameObject**)variable->mData = *(GameObject**)payload->Data;
				}
				ImGui::EndDragDropTarget();
			}
			break;
		}
		default:
			break;
		}
	}
	
}


void InspectorPanel::DrawAnimationComponent(AnimationComponent* component) {

	ImGui::SeparatorText("Animation");
	ImGui::Text("HELLO");

	static bool play = false;

	if(ImGui::Button("Play"))
	{
		if (component->GetAnimation() == nullptr)
			return;
		component->OnStart();

		play = true;
	}

	if(play)
		component->OnUpdate();

}
