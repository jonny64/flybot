#pragma once
#include "stdwx.h"
#include "ChatBotAPI.h"
#include "UserInfo.h"

struct _FlybotAPI
{
	void Init(const BotInit*);
	bool SendPM(const wxString&, const wxString&);
	bool QueryUserinfo(const WCHAR*, UserInfo*);
	bool ClosePM(wxString&);
	bool AddToIgnore(wxString&);
	bool GiveSlot(wxString&);
};

extern _FlybotAPI FlybotAPI;
