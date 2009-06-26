#pragma once
#include "stdwx.h"
#include "ChatBotAPI.h"
#include "UserInfo.h"

struct _FlybotAPI
{
	void Init(const BotInit*);
	bool SendPM(const wxString&, const wxString&);
	bool QueryUserinfo(const WCHAR*, UserInfo*);
};

extern _FlybotAPI FlybotAPI;
