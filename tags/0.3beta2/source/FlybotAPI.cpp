#include "stdwx.h"
#include "ChatBotAPI.h"
#include "FlybotAPI.h"
#include "wxFlybotDLL.h"

static BotInit m_botAPI;
_FlybotAPI FlybotAPI;

void _FlybotAPI::Init(const BotInit *aInit)
{
    memset(&m_botAPI, 0, sizeof(BotInit));
    memcpy(&m_botAPI, aInit, sizeof(BotInit));
}

bool _FlybotAPI::SendPM(const wxString& cid, const wxString& msg)
{
	if (m_botAPI.SendMessage2 && !cid.empty() && !msg.empty())
    {
        return m_botAPI.SendMessage2(SEND_PM, cid.c_str(), msg.c_str(), 
            msg.size() + sizeof(wxChar));
    }
    return false;
}

bool _FlybotAPI::QueryUserinfo(const WCHAR* cid, UserInfo *userinfo)
{
    if (m_botAPI.QueryInfo && cid)
    {
        *userinfo = (WCHAR*)m_botAPI.QueryInfo(QUERY_USER_BY_CID, cid, NULL, 0);
        return true;
    }
    return false;
}


bool _FlybotAPI::ClosePM(const wxString& cid)
{    
    if (m_botAPI.SendMessage2 && !cid.empty())
    {
        m_botAPI.SendMessage2( USER_CLOSE, cid.c_str(), NULL, 0 );
        return true;
    }
    return false;
}

bool _FlybotAPI::GiveSlot(const wxString& cid, int slotTimeoutSeconds)
{    
    if (m_botAPI.SendMessage2 && !cid.empty())
    {
        m_botAPI.SendMessage2( USER_SLOT, cid.c_str(), &slotTimeoutSeconds, sizeof(slotTimeoutSeconds) );
        return true;
    }
    return false;
}

bool _FlybotAPI::AddToIgnore(const wxString& cid)
{
    if (m_botAPI.SendMessage2 && !cid.empty())
    {
        BOOL WIN_TRUE = TRUE;
        m_botAPI.SendMessage2( USER_IGNORE, cid.c_str(), &WIN_TRUE, sizeof(BOOL) );
        return true;
    }
    return false;
}
