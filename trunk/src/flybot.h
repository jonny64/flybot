#pragma once
#include "stdwx.h" 

#include "tray.h"
#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "wxFlybotDLL.h"
#include "ChatBotAPI.h"

#define FLYBOT_API __declspec(dllexport) bool  __stdcall

extern "C" FLYBOT_API init(BotInit*);

void RunApp();