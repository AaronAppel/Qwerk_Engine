#ifndef _Main_H_
#define _Main_H_

#if _WIN32
#define Win32Bit
#else
#define Win64Bit
#endif

#define FrameworkDir "../../QwerkE_Framework/"
#define AssetsDir "../../Assets/"
// #define LibrariesDir "../../QwerkE_Framework/Libraries/"

#if 1 // Use the Engine?
#include "../../Source/Engine.h"
#else // Or just the Framework?
#include "../../QwerkE_Framework/QwerkE.h"
#endif

#endif // !_Main_H_