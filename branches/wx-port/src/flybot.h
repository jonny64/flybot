#pragma once

#include "stdwx.h"
#include "tray.h"

#define FLYBOT_API __declspec(dllexport) bool  __stdcall


struct BotInit g_botAPI = {0};
HANDLE ThreadId;


extern "C" FLYBOT_API init(BotInit*);

void RunApp();