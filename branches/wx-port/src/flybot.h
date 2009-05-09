// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FLYBOT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FLYBOT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
//#include "wx/wx.h"
#include <windows.h>
#include <process.h>

#include "ChatBotAPI.h"
#include "tray.h"

#define FLYBOT_API __declspec(dllexport) bool  __stdcall


struct BotInit g_botAPI = {0};
HANDLE ThreadId;


extern "C" FLYBOT_API init(BotInit*);

void RunApp();