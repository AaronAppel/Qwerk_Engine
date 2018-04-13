#include "imgui_Editor.h"
#include "../Systems/ServiceLocator.h"
#include "../Systems/Graphics/ShaderProgram/ShaderProgram.h"
#include "../../Shared_Generic/Utilities/FileIO/FileUtilities.h"
#include "../../Shared_Generic/Libraries/imgui/imgui.h"
#include "../Systems/Input/InputManager.h"
#include "../Systems/Renderer.h"
#include "../Scene/SceneManager.h"
#include "EntityEditor.h"
#include "../Systems/Time.h"

imgui_Editor::imgui_Editor()
{
	m_SceneManager = (SceneManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Scene_Manager);
	m_Input = (InputManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Input_Manager);
}

imgui_Editor::~imgui_Editor()
{
}

void imgui_Editor::NewFrame()
{
}

void imgui_Editor::Update()
{

}

void imgui_Editor::Draw()
{
    // windows
    static bool shaderEditor = false;

	// menu
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Menu"))
		{
			static int index = 0;
			static const int size = 5;
			const char* d[size] = { "one", "two", "three", "four", "five"};
			ImGui::ListBox("", &index, d, size, 3);
			ImGui::EndMenu();
		}

        if (ImGui::BeginMenu("Tools"))
        {
            const int size = 1;
            static const char* toolsList[size] = {"Shader Editor"};
            static bool* toolsStates[size] = { &shaderEditor };

            for (int i = 0; i < size; i++)
            {
                ImGui::Checkbox(toolsList[i], toolsStates[i]);
            }
            ImGui::EndMenu();
        }

		// FPS display
		static bool showFPS = true;
		if (ImGui::Button("FPS"))
			showFPS = !showFPS;
		ImGui::SameLine();
		if (showFPS) ImGui::Text("%4.2f", QwerkE::Time::GetFrameRate());


		ImGui::EndMainMenuBar();
	}
    
    if (shaderEditor)
    {        
        DrawShaderEditor((ShaderProgram*)((ResourceManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Resource_Manager))->GetShader("TestShader"));
    }

	m_SceneGraph->Draw();
    m_EntityEditor->Draw();
	DrawSceneList();
}

void imgui_Editor::DrawSceneList()
{
	ImGui::Begin("Scene List");

	// TODO: Think of adding multi window support for viewing more than 1
	// enabled scene at a time. Going to have to look at how input would
	// work for that.
	m_SceneManager; // size
	for (int i = 0; i < 1; i++)
	{
		if (ImGui::Button("TestScene") || m_Input->GetIsKeyDown(eKeys::eKeys_1))
		{
			m_SceneManager->SetCurrentScene(eSceneTypes::Scene_TestScene);
		}
		if (ImGui::Button("GameScene") || m_Input->GetIsKeyDown(eKeys::eKeys_2))
		{
			// m_SceneManager->SetCurrentScene(eSceneTypes::Scene_GameScene);
		}
	}

	ImGui::End();
}