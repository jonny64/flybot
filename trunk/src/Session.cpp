#include "stdwx.h"
#include "Session.h"
#include "wxFlybotDLL.h"
#include "wxLogBalloon.h"

Session::Session(void)
{
}

Session::Session(UserInfo& userinfo)
{
	m_userinfo = userinfo;
}

void Session::ProcessFlags(const Phrase &selectedPharase)
{
	const wxString flags = selectedPharase.Flags;
	wxString title = wxString::Format(_("Matched template: %s"), selectedPharase.MatchExpr);
	wxString message = wxT("");
	
	if (flags.Freq(DICTIONARY_CLOSE_CHAR) > 0)
	{
		FlybotAPI.ClosePM(m_userinfo[FLYBOT_API_CID]);
		message = wxString::Format(_("Closed PM from %s"), m_userinfo[FLYBOT_API_NICK]);
	}
	if (flags.Freq(DICTIONARY_SLOT_CHAR) > 0)
	{
		int slotTimeout = wxGetApp().Config.GetSelectedSlotTimeout();
		FlybotAPI.GiveSlot(m_userinfo[FLYBOT_API_CID], slotTimeout);
		
		message = wxString::Format(_("Slot was given to %s"), m_userinfo[FLYBOT_API_NICK]);
	}
	if (flags.Freq(DICTIONARY_IGNORE_CHAR) > 0)
	{
		FlybotAPI.AddToIgnore(m_userinfo[FLYBOT_API_CID]);
		message = wxString::Format(_("%s added to ignore list") , m_userinfo[FLYBOT_API_NICK]);
	}

	if (!message.empty())
	{
		wxLogMessage(title, message);
	}
}

int Session::Answer(wxString& msg)
{
	Phrase selectedPhrase = wxGetApp().Dict.GetMatchedTemplate(msg, &m_usedPhrases);

	// if no matches, exit;
	if (selectedPhrase.empty())
		return 0;

	// TODO: replace special vars in answer;

	ProcessFlags(selectedPhrase);

	// TODO: wait desired time interval

	// send answer
	wxString answer = selectedPhrase.Answer;
	wxString cid = m_userinfo[FLYBOT_API_CID];
	if (!cid.empty() && !answer.empty())
	{
		FlybotAPI.SendPM(cid, answer);
	}

	return 0;
}

Session::~Session(void)
{
}
