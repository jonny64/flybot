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

void Session::ProcessFlags(const wxString &flags)
{
	// TODO: use hash char->func ?!
	if (flags.Freq(DICTIONARY_CLOSE_CHAR) > 0)
	{
		FlybotAPI.ClosePM(m_userinfo[wxT("CID")]);
	}
	if (flags.Freq(DICTIONARY_SLOT_CHAR) > 0)
	{
		FlybotAPI.GiveSlot(m_userinfo[wxT("CID")]);
	}
	if (flags.Freq(DICTIONARY_IGNORE_CHAR) > 0)
	{
		FlybotAPI.ClosePM(m_userinfo[wxT("CID")]);
	}
}

int Session::Answer(wxString& msg)
{
	Phrase selectedPhrase = wxGetApp().Dict.GetMatchedTemplate(msg, &m_usedPhrases);
	
	// if no matches, exit;
	if (selectedPhrase.empty())
		return 0;

	// TODO: replace special vars in answer;
	// process answer additional flags;
	ProcessFlags(selectedPhrase.Flags);

	// wait desired time interval

	// send answer
	wxString answer = selectedPhrase.Answer;
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
