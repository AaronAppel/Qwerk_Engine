#include "../ResourceViewer.h"
#include "../MaterialEditor.h"

// TODO: Why can't I include imgui.h? include order? It is #included in .h
//#include "../../QwerkE_Framework/Libraries/imgui/imgui.h"

#include "../QwerkE_Framework/Libraries/glew/GL/glew.h"
#include "../QwerkE_Framework/Source/Core/Graphics/DataTypes/FrameBufferObject.h"
#include "../QwerkE_Framework/Source/Core/Graphics/GraphicsUtilities/GraphicsHelpers.h"
#include "../QwerkE_Framework/Source/Core/Graphics/DataTypes/Material.h"
#include "../QwerkE_Framework/Source/Core/Graphics/Mesh/Mesh.h"
#include "../QwerkE_Framework/Source/Core/Graphics/DataTypes/Texture.h"
#include "../QwerkE_Framework/Source/Core/Graphics/Shader/ShaderProgram.h"
#include "../QwerkE_Framework/Source/Core/Scenes/Entities/GameObject.h"
#include "../QwerkE_Framework/Source/Core/Scenes/Entities/Components/RenderComponent.h"
#include "../QwerkE_Framework/Source/Core/Scenes/Entities/Components/Camera/CameraComponent.h"
#include "../QwerkE_Framework/Source/Core/Scenes/Scene.h"
#include "../QwerkE_Framework/Source/Core/Scenes/ViewerScene.h"
#include "../QwerkE_Framework/Source/Core/Scenes/Scenes.h"
#include "../QwerkE_Framework/Source/Core/Resources/Resources.h"
#include "../QwerkE_Framework/Source/Core/Audio/Audio.h"
#include "../QwerkE_Framework/Source/Core/Factory/Factory.h"

#include <string>

namespace QwerkE {

    ResourceViewer::ResourceViewer()
    {
        // TODO: Review references for necessity
        m_MaterialEditor = new MaterialEditor();
        m_Materials = Resources::SeeMaterials();
        m_Textures = Resources::SeeTextures();
        m_Shaders = Resources::SeeShaderPrograms();
        m_Meshes = Resources::SeeMeshes();
        m_Sounds = Resources::SeeSounds();
        m_FBO = new FrameBufferObject();
        m_FBO->Init();

        m_ViewerScene = new ViewerScene();

        // m_Subject = ((Factory*)QwerkE::Services::GetService(eEngineServices::Factory_Entity))->CreateTestModel(m_ViewerScene, vec3(0, -3.5, 15));
        // m_Subject->SetRotation(vec3(0,180,0));

        m_TagPlane = Factory::CreatePlane(m_ViewerScene, vec3(2, -2, 10));
        m_TagPlane->SetRotation(vec3(90, 0, 0));
        m_TagPlane->SetScale(vec3(0.3f, 0.3f, 0.3f));

        // m_ViewerScene->AddObjectToScene(m_Subject);
        m_ViewerScene->AddObjectToScene(m_TagPlane);

        m_ViewerScene->Initialize();
        m_ViewerScene->SetIsEnabled(true);
        ((CameraComponent*)m_ViewerScene->GetCameraList().at(0)->GetComponent(Component_Camera))->SetViewportSize(vec2(1, 1));

        Scenes::AddScene(m_ViewerScene);

        DrawModelThumbnails();
    }

    ResourceViewer::~ResourceViewer()
    {
        delete m_MaterialEditor;
        delete m_FBO;
    }

    void ResourceViewer::Draw()
    {
        if (ImGui::Begin("Resources"))
        {
            if (ImGui::Button("Refresh"))
            {
                DrawModelThumbnails();
            }
            ImGui::SameLine();

            // select what resource to view
            if (ImGui::Button("Textures"))
                m_CurrentResource = 0;
            ImGui::SameLine();
            if (ImGui::Button("Materials"))
                m_CurrentResource = 1;
            ImGui::SameLine();
            if (ImGui::Button("Shaders"))
                m_CurrentResource = 2;
            ImGui::SameLine();
            if (ImGui::Button("Fonts"))
                m_CurrentResource = 3;
            ImGui::SameLine();
            if (ImGui::Button("Models"))
                m_CurrentResource = 4;
            ImGui::SameLine();
            if (ImGui::Button("Sounds"))
                m_CurrentResource = 5;

            // draw list of resources
            ImVec2 winSize = ImGui::GetWindowSize();
            m_ItemsPerRow = (unsigned char)(winSize.x / (m_ImageSize.x * 1.5f) + 1.0f); // (* up the image size for feel), + avoid dividing by 0
            unsigned int counter = 0;
            ImGui::Separator();
            // TODO: Consider using imgui groups for easier hover support
            switch (m_CurrentResource)
            {
            case 0:
                // draw texture thumbnails
                for (const auto& p : *m_Textures)
                {
                    if (counter % m_ItemsPerRow)
                        ImGui::SameLine();

                    ImGui::ImageButton((ImTextureID)p.second->s_Handle, m_ImageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), 1);

                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        // image name or something might be better. use newly create asset tags

                        if (ImGui::IsMouseDown(0))
                        {
                            ImGui::ImageButton((ImTextureID)p.second->s_Handle, ImVec2(256, 256), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), 1);
                        }

                        ImGui::Text(p.second->s_Name.c_str());
                        ImGui::Text(std::to_string(p.second->s_Handle).c_str());
                        //ImGui::Text("TagName");
                        ImGui::EndTooltip();
                    }
                    counter++;
                }
                break;
            case 1:
                // draw material thumbnails
                for (const auto& p : *m_Materials)
                {
                    if (counter % m_ItemsPerRow)
                        ImGui::SameLine();

                    ImGui::ImageButton((ImTextureID)p.second->GetMaterialByType(eMaterialMaps::MatMap_Diffuse)->s_Handle, m_ImageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), 1);
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        // image name or something might be better. use newly create asset tags
                        ImGui::Text(p.second->GetMaterialName().c_str());
                        ImGui::Text(std::to_string(p.second->GetMaterialByType(eMaterialMaps::MatMap_Diffuse)->s_Handle).c_str());
                        //ImGui::Text("TagName");
                        ImGui::EndTooltip();
                    }
                    if (ImGui::IsItemClicked())
                    {
                        m_ShowMatEditor = true;
                        m_MatName = p.second->GetMaterialName();
                    }
                    counter++;
                }
                break;
            case 2:
                for (auto p : *m_Shaders)
                {
                    if (counter % m_ItemsPerRow)
                        ImGui::SameLine();

                    if (ImGui::Button(p.first.c_str()))
                    {
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text(std::to_string(p.second->GetProgram()).c_str());
                        ImGui::EndTooltip();
                    }
                    counter++;
                }
                break;
            case 3:
                // for (m_Fonts)
                break;
            case 4:
                for (unsigned int i = 0; i < m_ModelImageHandles.size(); i++)
                {
                    if (counter % m_ItemsPerRow)
                        ImGui::SameLine();

                    ImGui::ImageButton((ImTextureID)m_ModelImageHandles.at(i), m_ImageSize, ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), 1);
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        if (ImGui::IsMouseDown(0))
                        {
                            ImGui::ImageButton((ImTextureID)m_ModelImageHandles.at(i), ImVec2(256, 256), ImVec2(0.0f, 1.0f), ImVec2(1.0f, 0.0f), 1);
                        }
                        // image name or something might be better. use newly create asset tags
                        ImGui::Text(std::to_string(m_ModelImageHandles[0]).c_str());
                        ImGui::EndTooltip();
                    }
                    counter++;
                }
                break;
            case 5:
                for (auto p : *m_Sounds)
                {
                    if (counter % m_ItemsPerRow)
                        ImGui::SameLine();

                    if (ImGui::Button(p.first.c_str()))
                    {
                        Audio::PlaySound(p.first.c_str());
                    }
                    if (ImGui::IsItemHovered())
                    {
                        ImGui::BeginTooltip();
                        ImGui::Text(std::to_string(p.second).c_str());
                        ImGui::EndTooltip();
                    }
                    counter++;
                }
                break;
            }

            if (m_ShowMatEditor)
            {
                m_MaterialEditor->Draw(Resources::GetMaterial(m_MatName.c_str()), &m_ShowMatEditor);
            }

            ImGui::End();
        }
        else
            ImGui::End();
    }

    void ResourceViewer::DrawModelThumbnails()
    {
        // Dump old values. maybe calculate what changed in the future
        m_ModelImageHandles.clear();

        for (const auto& p : *m_Meshes)
        {
            m_FBO->Bind();

            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

            // TODO: Loop through renderables to setup
            //((RenderComponent*)m_Subject->GetComponent(Component_Render))->SetMeshAtIndex(0, m_Resources->GetMesh(null_mesh));

            // TODO: RenderRoutine needs to update its uniform functions properly
            //((RenderComponent*)m_Subject->GetComponent(Component_Render))->SetModel(p.second);
            //((RenderComponent*)m_TagPlane->GetComponent(Component_Render))->SetColour(vec4(128, 128, 128, 255)); // TODO: use model asset tag color

            // draw scene
            m_ViewerScene->Draw();

            // GLuint tempTexture;
            // glGenTextures(1, &tempTexture);
            // glBindTexture(GL_TEXTURE_2D, tempTexture);
            // glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1280, 720, 0, 0, 1);
            // glCopyTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, 0, 0, 1280, 720);


            m_FBO->UnBind();

            // m_ModelImageHandles.push_back(tempTexture);
        }
        m_ModelImageHandles.push_back(m_FBO->GetTextureID());
    }

}
