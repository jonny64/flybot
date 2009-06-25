#pragma once
#include "stdwx.h" 

#include "tray.h"
#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "wxFlybotDLL.h"

#define FLYBOT_API __declspec(dllexport) bool  __stdcall

extern "C" FLYBOT_API init(BotInit*);
struct BotInit g_botAPI = {0};

void RunApp();