#pragma once
#include "stdwx.h"

#include "FlybotTaskBarIcon.h"
#include "UserInfo.h"
#include "Session.h"
#include "Dictionary.h"
#include "wxFlybotDLL.h"
#include "FlybotAPI.h"

#define FLYBOT_API __declspec(dllexport) bool  __stdcall
#define APP_NAME "flybot"
#define APP_BUILD_DATE __DATE__
#define APP_VERSION "0.32"

extern "C" FLYBOT_API init(BotInit*);
void RunApp();