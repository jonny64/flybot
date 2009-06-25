#include "stdwx.h"
#include "UserInfo.h"
#include "Session.h"

extern struct BotInit g_botAPI;

Session::Session(void)
{
	m_dictionary.Load();
}

Session::Session(UserInfo& userinfo)
{
	m_userinfo = userinfo;
}

int Session::Answer(wxString& msg)
{
	wxString answer = m_dictionary.GetAnswer(msg);
	
	// if no matches, exit;
	if (answer.empty())
		return 0;

	// TODO: replace special vars in answer;
	// process answer additional flags;
	// wait desired time interval

	// send answer
	wxString cid = m_userinfo[wxT("CID")];
	if (!cid.empty())
	{
		g_botAPI.SendMessage2(SEND_PM, cid.c_str(), answer.c_str(), answer.size() + sizeof(wxChar));
	}

	return 0;
}

Session::~Session(void)
{
}
