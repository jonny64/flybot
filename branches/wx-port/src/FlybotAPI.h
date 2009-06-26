#pragma once
#include "stdwx.h"
#include "ChatBotAPI.h"

static BotInit m_botAPI;

struct FlybotAPI
{
	static void Init(const BotInit *aInit)
	{
		memset(&m_botAPI, 0, sizeof(BotInit));
		memcpy(&m_botAPI, aInit, sizeof(BotInit));
	}

	static bool SendPM(const wxString& cid, const wxString& msg)
	{
		if (m_botAPI.SendMessage2)
		{
			return m_botAPI.SendMessage2(SEND_PM, cid.c_str(), msg.c_str(), 
				msg.size() + sizeof(wxChar));
		}
		return false;
	}

	static bool QueryUserinfo(const WCHAR* cid, UserInfo *userinfo)
	{
		if (m_botAPI.QueryInfo)
		{
			*userinfo = (WCHAR*)m_botAPI.QueryInfo(QUERY_USER_BY_CID, cid, NULL, 0);
			return true;
		}
		return false;
	}
};
