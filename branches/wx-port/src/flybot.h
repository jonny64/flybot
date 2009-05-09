// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the FLYBOT_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// FLYBOT_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#include "wx/wx.h"
#include <windows.h>
#include <process.h>

#include "ChatBotAPI.h"
#include "tray.h"


//#ifdef FLYBOT_EXPORTS
#define FLYBOT_API __declspec(dllexport)
//#else
//#define FLYBOT_API __declspec(dllimport)
//#endif


struct BotInit _init = {0};
CRITICAL_SECTION logcs;
HANDLE ThreadId;


extern "C" FLYBOT_API bool init(BotInit*);

void RunApp();