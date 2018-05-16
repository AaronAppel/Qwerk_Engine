#include "SceneGraph.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/imgui/imgui.h"
#include "../QwerkE_Framework/QwerkE_Framework/Scenes/SceneManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Scenes/Scene.h"
#include "../QwerkE_Framework/QwerkE_Framework/Entities/GameObject.h"
#include "../QwerkE_Framework/QwerkE_Framework/Entities/Components/RenderComponent.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/ServiceLocator.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/ResourceManager.h"
#include "../Editor.h"
#include "../EntityEditor/EntityEditor.h"

#include <map>
#include <string>

SceneGraph::SceneGraph(Editor* editor)
{
    m_Editor = editor;
	m_SceneManager = (SceneManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Scene_Manager);
	m_ResourceManager = (ResourceManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Resource_Manager);
}

SceneGraph::~SceneGraph()
{
}

void SceneGraph::Draw()
{
	static bool isOpen = true;
	ImGui::Begin("SceneGraph", &isOpen);

	std::map<std::string, GameObject*> entities = m_SceneManager->GetCurrentScene()->GetObjectList();
	std::map<std::string, GameObject*>::iterator thing;

	for (thing = entities.begin(); thing != entities.end(); thing++)
	{
		if (ImGui::Button(thing->second->GetName().c_str()))
		{
            m_Editor->GetEntityEditor()->SetCurrentEntity(thing->second->GetName());
        }
	}

	ImGui::End();
}