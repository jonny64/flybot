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

void Session::ProcessFlags(const Phrase &selectedPharase)
{
	const wxString flags = selectedPharase.Flags;
	wxString message = wxT("");
	if (flags.Freq(DICTIONARY_CLOSE_CHAR) > 0)
	{
		FlybotAPI.ClosePM(m_userinfo[FLYBOT_API_CID]);
		message = _("Closed PM from %s, matched template:\n%s");
	}
	if (flags.Freq(DICTIONARY_SLOT_CHAR) > 0)
	{
		FlybotAPI.GiveSlot(m_userinfo[FLYBOT_API_CID]);
		message = _("Slot was given to %s, matched template:\n%s");
	}
	if (flags.Freq(DICTIONARY_IGNORE_CHAR) > 0)
	{
		FlybotAPI.AddToIgnore(m_userinfo[FLYBOT_API_CID]);
		message = _("%s added to ignore list, matched template:\n %s");
	}

	if (!message.empty())
		wxLogMessage(message, m_userinfo[wxT("NICK")], selectedPharase.MatchExpr);
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
