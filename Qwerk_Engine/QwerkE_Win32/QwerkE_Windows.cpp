// It is required to add the following preprocessor defines to this project properties.
// Win32Bit/Win64Bit, and DEBUG/RELEASE

// Use the QwerkE_Framework
// Add a runtime editor that has extra functionality like:
// level editor, entity editor, shader editor, scripting, etc...


// #include "../Shared_Engine/Engine_Enums.h"
#include "../Shared_Engine/Engine.h"

int main() // End user implementation
{
	QwerkE::Engine::Run();

	return 0;
}