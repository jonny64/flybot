#include "stdwx.h"
#include "Session.h"
#include "wxFlybotDLL.h"

Session::Session(void)
{
}

Session::Session(UserInfo& userinfo)
{
	m_userinfo = userinfo;
}

int Session::Answer(wxString& msg)
{
	Phrase selectedPhrase = wxGetApp().Dict.GetMatchedTemplate(msg, &m_usedPhrases);
	
	// if no matches, exit;
	if (selectedPhrase.empty())
		return 0;

	// TODO: replace special vars in answer;
	// process answer additional flags;
	wxString answer = selectedPhrase.Answer;

	// wait desired time interval

	// send answer
	wxString cid = m_userinfo[wxT("CID")];
	if (!cid.empty())
	{
		FlybotAPI.SendPM(cid, answer);
	}

	return 0;
}

Session::~Session(void)
{
}
