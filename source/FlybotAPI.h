#pragma once
#include "stdwx.h"
#include "ChatBotAPI.h"
#include "UserInfo.h"

typedef
struct _FlybotAPI
{
    BotInit m_botAPI;
    wxString ConfigPath;
    void Initialize(const BotInit*);
    bool SendPM(const wxString &cid, const wxString &msg);
    bool QueryUserinfo(const WCHAR*, UserInfo*);
    bool ClosePM(const wxString &cid);
    bool AddToIgnore(const wxString &cid);
    bool GiveSlot(const wxString& cid, int slotTimeoutSeconds);
}
TFlybotAPI;

extern _FlybotAPI FlybotAPI;
