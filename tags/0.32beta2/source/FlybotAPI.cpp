#include "stdwx.h"
#include "ChatBotAPI.h"
#include "FlybotAPI.h"
#include "wxFlybotDLL.h"
#include <wx/stdpaths.h>

TFlybotAPI FlybotAPI;

void TFlybotAPI::Initialize(const BotInit *aInit)
{
    memset(&m_botAPI, 0, sizeof(BotInit));
    memcpy(&m_botAPI, aInit, sizeof(BotInit));
    
    if (aInit->apiVersion > 2)
    {
        ConfigPath = wxString(aInit->appConfigPath, MAX_PATH);
    }
    else
    {
        ConfigPath = wxStandardPaths::Get().GetPluginsDir() + wxT("\\Settings\\");
    }
}

bool TFlybotAPI::SendPM(const wxString& cid, const wxString& msg)
{
    if (m_botAPI.SendMessage2 && !cid.empty() && !msg.empty())
    {
        return m_botAPI.SendMessage2(BotInit::SEND_PM, cid.c_str(), msg.t_str(),
                                     (unsigned int)msg.size() + sizeof(wxChar));
    }
    return false;
}

bool TFlybotAPI::QueryUserinfo(const WCHAR* cid, UserInfo *userinfo)
{
    if (m_botAPI.QueryInfo && cid)
    {
        WCHAR* userinfoString = (WCHAR*)m_botAPI.QueryInfo(BotInit::QUERY_USER_BY_CID, cid, NULL, 0);
        if (!userinfoString)
            return false;
            
        *userinfo = UserInfo(userinfoString);
        return true;
    }
    return false;
}


bool TFlybotAPI::ClosePM(const wxString& cid)
{
    if (m_botAPI.SendMessage2 && !cid.empty())
    {
        m_botAPI.SendMessage2(BotInit::USER_CLOSE, cid.c_str(), NULL, 0);
        return true;
    }
    return false;
}

bool TFlybotAPI::GiveSlot(const wxString& cid, int slotTimeoutSeconds)
{
    if (m_botAPI.SendMessage2 && !cid.empty())
    {
        m_botAPI.SendMessage2(BotInit::USER_SLOT, cid.c_str(), &slotTimeoutSeconds, sizeof(slotTimeoutSeconds));
        return true;
    }
    return false;
}

bool TFlybotAPI::AddToIgnore(const wxString& cid)
{
    if (m_botAPI.SendMessage2 && !cid.empty())
    {
        BOOL WIN_TRUE = TRUE;
        m_botAPI.SendMessage2(BotInit::USER_IGNORE, cid.c_str(), &WIN_TRUE, sizeof(BOOL));
        return true;
    }
    return false;
}
