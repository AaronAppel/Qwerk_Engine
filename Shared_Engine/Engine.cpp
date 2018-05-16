#include "Engine.h"
#include "../QwerkE_Framework/QwerkE_Framework/Framework.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/glew/GL/glew.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/glfw/GLFW/glfw3.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/imgui/imgui.h"
#include "../QwerkE_Framework/QwerkE_Common/Libraries/imgui/imgui_impl_glfw_gl3.h"
#include "Engine_Enums.h"
#include "Editor/imgui_Editor.h"

#include "../QwerkE_Framework/QwerkE_Framework/Systems/ResourceManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/ServiceLocator.h"
#include "../QwerkE_Framework/QwerkE_Common/Utilities/Helpers.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Events/EventManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Scenes/SceneManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Factory/Factory.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Graphics/Gfx_Classes/Sprite.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Graphics/Graphics_Header.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Graphics/Gfx_Classes/FrameBufferObject.h"

#include "../QwerkE_Framework/QwerkE_Framework/CallbackFunctions.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Physics/PhysicsManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Renderer.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/MessageManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Audio/AudioManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Debugger/Debugger.h"
#include "../QwerkE_Framework/QwerkE_Framework/Components/Time.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Graphics/Model/Mesh/MeshFactory.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Graphics/ShaderProgram/ShaderFactory.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/JobManager/JobManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Networking/NetworkManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Window/Window.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Window/WindowManager.h"
#include "../QwerkE_Framework/QwerkE_Framework/Systems/Window/glfw_Window.h"

// private engine variables
static WindowManager* m_Window = nullptr;
static SceneManager* m_SceneManager = nullptr;
static bool m_IsRunning = false;
static Editor* m_Editor = nullptr;

namespace QwerkE
{
	namespace Engine
	{
		void Engine::Run()
		{
			// TODO: check if(initialized) in case user defined simple API.
			// Might want to create another function for the game loop and
			// leave Run() smaller and abstracted from the functionality.
			QwerkE::Framework::Startup();

			m_SceneManager = (SceneManager*)QwerkE::ServiceLocator::GetService(eEngineServices::Scene_Manager);
			m_SceneManager->GetCurrentScene()->SetIsEnabled(true);

			// QwerkE::Framework::Run();

			m_Editor = (Editor*)new imgui_Editor();

			m_Window = (WindowManager*)QwerkE::ServiceLocator::GetService(eEngineServices::WindowManager);

			m_IsRunning = true;

			double timeSinceLastFrame = 0.0;
			float frameRate = 0.0f;
			QwerkE::Time::SetDeltatime(&timeSinceLastFrame);
			QwerkE::Time::SetFrameRate(&frameRate);

			// TODO: GL state init should be in a Window() or OpenGLManager()
			// class or some type of ::Graphics() system.
			glClearColor(0.5f, 0.7f, 0.7f, 1.0f);
			// turn on depth buffer testing
			glEnable(GL_DEPTH_TEST);

			// depth cull for efficiency
			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);
			if (Wind_CCW) glFrontFace(GL_CCW);
			else glFrontFace(GL_CW);

			// turn on alpha blending
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			glViewport(0, 0, g_WindowWidth, g_WindowHeight);

			QwerkE::ServiceLocator::LockServices(true); // prevent service changes

			// Deltatime + FPS Tracking //
			// Deltatime
			double deltaTime = 0.0f; // Time between current frame and last frame
			double lastFrame = helpers_Time(); // Time of last frame initialized to current time
			// Limit framerate
			int FPS_MAX = 120; // maximum number of frames that can be run be second
			float FPS_MAX_DELTA = 1.0f / FPS_MAX;
			timeSinceLastFrame = FPS_MAX; // Amount of seconds since the last frame ran initialized to run 1st time
			// Printing framerate
			float printPeriod = 3.0f; // Print period in seconds
			float timeSincePrint = printPeriod; // Seconds since last print initialized to print 1st frame
			short framesSincePrint = 0;

			// Application Loop
			while (m_IsRunning) // Run until close requested
			{
				// setup frame
				// Calculate deltatime of current frame
				double currentFrame = helpers_Time();
				deltaTime = currentFrame - lastFrame; // time since last frame
				lastFrame = currentFrame; // save last frame

				// FPS display + tracking
				if (timeSincePrint >= printPeriod) // print period
				{
					frameRate = 1.0f / timeSincePrint * framesSincePrint;
					// OutputPrint("\nFPS: %f", frameRate); // FPS printout
					// OutputPrint("\nFrames: %i", framesSincePrint); // Frames printout
					timeSincePrint = 0.0f;
					framesSincePrint = 0;
				}

				timeSincePrint += (float)deltaTime;
				timeSinceLastFrame += deltaTime;

				if (timeSinceLastFrame >= FPS_MAX_DELTA) // Run frame?
				{
					/* Game Loop */
					/* New Frame */
					Engine::NewFrame();

					/* Input */
					Engine::Input();

					/* Logic */
					Engine::Update(timeSinceLastFrame);

					/* Render */
					Engine::Draw();

					// FPS
					framesSincePrint++; // Framerate tracking
					timeSinceLastFrame = 0.0; // FPS_Max
				}
				else
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

		void Engine::NewFrame()
		{
			/* Reset */
			// TODO: Reset things...
			ImGui_ImplGlfwGL3_NewFrame();
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
			if (inputManager->GetIsKeyDown(eKeys::eKeys_P)) // pause entire scene
			{
				m_SceneManager->GetCurrentScene()->TogglePauseState();
			}
			if (inputManager->GetIsKeyDown(eKeys::eKeys_F)) // pause actor updates
			{
				static bool isFrozen = false;
				isFrozen = !isFrozen; // toggle bool
				m_SceneManager->GetCurrentScene()->SetIsFrozen(isFrozen);
			}
			if (inputManager->GetIsKeyDown(eKeys::eKeys_Escape))
			{
				WindowManager* windowManager = (WindowManager*)QwerkE::ServiceLocator::GetService(eEngineServices::WindowManager);
				m_Window->GetWindow(0)->SetClosing(true); // close glfw
				Framework::Stop();
				Engine::Stop();
			}
		}

		void Engine::Draw()
		{
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // new frame

			m_Editor->Draw();

			//m_SceneManager->Draw();
			QwerkE::Framework::Draw();
		}

		bool Engine::StillRunning()
		{
			return m_IsRunning;
		}
	}
}
