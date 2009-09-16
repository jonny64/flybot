#pragma once
#include "stdwx.h" 

#include "FlybotTaskBarIcon.h"
#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "wxFlybotDLL.h"
#include "FlybotAPI.h"

#define FLYBOT_API __declspec(dllexport) bool  __stdcall
#define APP_BUILD_DATE wxT(__DATE__)
#define APP_VERSION wxT("0.3")

extern "C" FLYBOT_API init(BotInit*);

void RunApp();