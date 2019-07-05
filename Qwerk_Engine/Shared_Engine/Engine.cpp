#include "Engine.h"

#include "Engine_Enums.h"
#include "Editor/imgui_Editor.h"

#include "../QwerkE_Framework/Framework.h"

#include "../QwerkE_Framework/QwerkE_Common/Libraries/glew/GL/glew.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/glfw/GLFW/glfw3.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/imgui/imgui.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/imgui/imgui_impl_glfw_gl3.h"

#include "../QwerkE_Framework/QwerkE_Common/Utilities/Helpers.h"

#include "../QwerkE_Framework/Graphics/Graphics_Header.h"
#include "../QwerkE_Framework/Graphics/FrameBufferObject.h"
#include "../QwerkE_Framework/Graphics/Mesh/MeshFactory.h"

#include "../QwerkE_Framework/Systems/ResourceManager/ResourceManager.h"
#include "../QwerkE_Framework/Systems/ServiceLocator.h"
#include "../QwerkE_Framework/Systems/Events/EventManager.h"
#include "../QwerkE_Framework/Systems/SceneManager.h"
#include "../QwerkE_Framework/Systems/Factory/Factory.h"
#include "../QwerkE_Framework/Systems/Window/CallbackFunctions.h"
#include "../QwerkE_Framework/Systems/Physics/PhysicsManager.h"
#include "../QwerkE_Framework/Systems/Renderer/Renderer.h"
#include "../QwerkE_Framework/Systems/MessageManager.h"
#include "../QwerkE_Framework/Systems/Audio/AudioManager.h"
#include "../QwerkE_Framework/Systems/Debugger/Debugger.h"
#include "../QwerkE_Framework/Systems/ShaderFactory/ShaderFactory.h"
#include "../QwerkE_Framework/Systems/JobManager/JobManager.h"
#include "../QwerkE_Framework/Systems/Networking/NetworkManager.h"
#include "../QwerkE_Framework/Systems/Window/Window.h"
#include "../QwerkE_Framework/Systems/Window/WindowManager.h"
#include "../QwerkE_Framework/Systems/Window/glfw_Window.h"

#include "../QwerkE_Framework/Modules/Time.h"

// private engine variables
static WindowManager* m_WindowManager = nullptr;
static SceneManager* m_SceneManager = nullptr;
static bool m_IsRunning = false;
static Editor* m_Editor = nullptr;

struct FrameTimer
{
	double timeSinceLastFrame = 0.0;
	float frameRate = 0.0f;
	double deltaTime = 0.0f; // Time between current frame and last frame
	double lastFrame = 0.0f; // Time of last frame initialized to current time

	// Limit framerate
	int FPS_MAX = 120; // maximum number of frames that can be run be second
	float FPS_MAX_DELTA = 1.0f / FPS_MAX;

	// timeSinceLastFrame = FPS_MAX; // Amount of seconds since the last frame ran initialized to run 1st time
	// Printing framerate
	float printPeriod = 3.0f; // Print period in seconds
	float timeSincePrint = 0.0f; // Seconds since last print initialized to print 1st frame
	short framesSincePrint = 0;

	FrameTimer() {}
	FrameTimer(int maxFPS, float printPeriod)
	{
		FPS_MAX = maxFPS;
		timeSincePrint = printPeriod;
		FPS_MAX_DELTA = 1.0f / FPS_MAX;
		timeSinceLastFrame = FPS_MAX;
	}
	~FrameTimer() {}
};

namespace QwerkE
{
	namespace Engine
	{
		void Engine::Run()
		{
			// TODO: check if(initialized) in case user defined simple API.
			// Might want to create another function for the game loop and
			// leave Run() smaller and abstracted from the functionality.
			if (QwerkE::Framework::Startup() == eEngineMessage::_QFailure)
			{
				ConsolePrint("\nFramework failed to load! Shutting down engine.\n");
				return;
			}

			QwerkE::ServiceLocator::LockServices(true); // prevent service changes

			m_SceneManager = (SceneManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Scene_Manager);
			m_SceneManager->GetCurrentScene()->SetIsEnabled(true); // enable default scene

			m_WindowManager = (WindowManager*)QwerkE::ServiceLocator::GetService(eEngineServices::WindowManager);

			m_IsRunning = true;

			// Deltatime
			FrameTimer fps(120, 3.0f);
			fps.lastFrame = helpers_Time(); // Time of last frame initialized to current time

			QwerkE::Time::SetDeltatime(&fps.timeSinceLastFrame);
			QwerkE::Time::SetFrameRate(&fps.frameRate);

			// TODO: GL state init should be in a Window() or OpenGLManager()
			// class or some type of ::Graphics() system.
			glClearColor(0.5f, 0.7f, 0.7f, 1.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// turn on depth buffer testing
			glEnable(GL_DEPTH_TEST);
			glPointSize(10);
			glLineWidth(10);

			// depth cull for efficiency
			// TODO: This is also in the framework
			// glEnable(GL_CULL_FACE);
			// glDisable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			if (Wind_CCW) glFrontFace(GL_CCW);
			else glFrontFace(GL_CW);

			// turn on alpha blending
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glViewport(0, 0, g_WindowWidth, g_WindowHeight);

#ifdef dearimgui
			m_Editor = (Editor*)new imgui_Editor();
#else
			m_Editor = (Editor*)new ????_Editor();
#endif // editor

			// Application Loop
			while (m_IsRunning) // Run until close requested
			{
				// setup frame
				// Calculate deltatime of current frame
				double currentFrame = helpers_Time();
				fps.deltaTime = currentFrame - fps.lastFrame; // time since last frame
				fps.lastFrame = currentFrame; // save last frame

				// FPS display + tracking
				if (fps.timeSincePrint >= fps.printPeriod) // print period
				{
					fps.frameRate = 1.0f / fps.timeSincePrint * fps.framesSincePrint;
					// OutputPrint("\nFPS: %f", frameRate); // FPS printout
					// OutputPrint("\nFrames: %i", framesSincePrint); // Frames printout
					fps.timeSincePrint = 0.0f;
					fps.framesSincePrint = 0;
				}

				fps.timeSincePrint += (float)fps.deltaTime;
				fps.timeSinceLastFrame += fps.deltaTime;

				// if(fps.framesSincePrint < fps.FPS_MAX) // TODO: Consider using frame count for more calculating framerate
				/* Application Loop */
				if (fps.timeSinceLastFrame >= fps.FPS_MAX_DELTA) // Run frame?
				{
					/* New Frame */
					Engine::NewFrame();

					/* Input */
					Engine::Input();

					/* Logic */
					Engine::Update(fps.timeSinceLastFrame);

					/* Render */
					Engine::Draw();

					// FPS
					fps.framesSincePrint++; // Framerate tracking
					fps.timeSinceLastFrame = 0.0; // FPS_Max
				}
				// else
				{
					// skip frame
				}
			}

			// unlock services for clean up
			QwerkE::ServiceLocator::LockServices(false);

			QwerkE::Framework::TearDown();
		}

		void Engine::Stop()
		{
			m_IsRunning = false;
		}

		void Engine::NewFrame() /* Reset */
		{
			Framework::NewFrame();
			m_Editor->NewFrame();
		}

		void Engine::Input()
		{
			glfwPollEvents(); // TODO: Better GLFW interface?
			// TODO: Tell input manager it is a new frame and it should update key states
		}

		void Engine::Update(double deltatime)
		{
			m_SceneManager->Update(deltatime);
			m_Editor->Update();

			// QwerkE::Framework::Update();

			//if (glfwGetKey(m_Window, GLFW_KEY_ESCAPE)) // DEBUG: A simple way to close the window while testing
			InputManager* inputManager = (InputManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Input_Manager);
			if (inputManager->FrameKeyAction(eKeys::eKeys_P, eKeyState::eKeyState_Press)) // pause entire scene
			{
				static bool paused = false;
				paused = !paused;
				if (paused)
				{
					m_SceneManager->GetCurrentScene()->SetState(eSceneState::SceneState_Paused);
				}
				else
				{
					m_SceneManager->GetCurrentScene()->SetState(eSceneState::SceneState_Running);
				}
			}
			if (inputManager->FrameKeyAction(eKeys::eKeys_Z, eKeyState::eKeyState_Press))// pause actor updates
			{
				static bool frozen = false;
				frozen = !frozen;
				if (frozen)
				{
					m_SceneManager->GetCurrentScene()->SetState(eSceneState::SceneState_Frozen);
				}
				else
				{
					m_SceneManager->GetCurrentScene()->SetState(eSceneState::SceneState_Running);
				}
			}
			if (inputManager->FrameKeyAction(eKeys::eKeys_F, eKeyState::eKeyState_Press))
			{
				m_Editor->ToggleFullScreenScene();
			}
			if (inputManager->FrameKeyAction(eKeys::eKeys_Escape, eKeyState::eKeyState_Press))
			{
				WindowManager* windowManager = (WindowManager*)QwerkE::ServiceLocator::GetService(eEngineServices::WindowManager);
				m_WindowManager->GetWindow(0)->SetClosing(true); // close glfw
				Framework::Stop();
				Engine::Stop();
			}
		}

		void Engine::Draw()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // new frame

			m_Editor->Draw();

			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			m_WindowManager->GetWindow(0)->SwapBuffers();
		}

		bool Engine::StillRunning()
		{
			return m_IsRunning;
		}
	}
}