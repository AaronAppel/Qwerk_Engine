#ifndef _ServiceLocator_H_
#define _ServiceLocator_H_

// Class ServiceLocator provides global access for systems
// necessary throughout the code such as memory management,
// audio playback, input, etc.
// To add a system add it to RegisterService(), UnregisterService,
// GetService(), and ServicesLoaded().

#include "Engine_Enums.h"
#include "Utilities/PrintFunctions.h"
#include "Systems/ResourceManager.h"
#include "Systems/Input/InputManager.h"

class ResourceManager;
class InputManager;
class GLFWwindow;
class EventManager;
class SceneManager;
class Factory;
class Editor;

namespace QwerkE
{
	// TODO: Improve service accessing
	class ServiceLocator
	{
	public:
		// Services need to be instantiated, then registered
		// TODO: Services...
		// Audio, Networking, Graphics (Renderer, GUI), Utilities (Conversion, FileIO, Printing),
		// Physics, Event, Debug, Memory, Window, Application, Input, Resources
		static void RegisterService(eEngineServices serviceType, void* service);

		static void* UnregisterService(eEngineServices serviceType); // TODO: Potentially dangerous!

		static void* GetService(eEngineServices serviceType);
		// TODO: Improve logic and code design
		static eEngineMessage ServicesLoaded(); // determine if all services are loaded

	private:
		static ResourceManager* m_ResourceManager;
		static InputManager* m_InputManager;
		static GLFWwindow* m_AppWindow;
        static EventManager* m_EventManager;
        static SceneManager* m_SceneManager;
        static Factory* m_Factory;
        static Editor* m_Editor;
	};
}

#endif //!_ServiceLocator_H_
