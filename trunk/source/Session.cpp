#include "stdwx.h"
#include <wx/regex.h>
#include "Session.h"
#include "wxFlybotDLL.h"
#include "wxLogBalloon.h"

Session::Session(void)
{
    m_replies.Clear();
}

Session::Session(UserInfo& userinfo)
{
    m_userinfo = userinfo;
    m_replies.Clear();
}

void Session::ProcessFlags(const Phrase &selectedPhrase)
{
    const wxString flags = selectedPhrase.Flags;
    wxString title = wxString::Format(_("Matched template: %s"), selectedPhrase.MatchExpr);
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

class AnswerThread: public wxThread
{
    wxString m_answer;
    wxString m_cid;
public:
    AnswerThread(wxString &msg, wxString &cid):
      m_answer(msg), m_cid(cid) {}

    virtual void *Entry()
    {
        int intervalSec = wxGetApp().Config.GetSelectedAnswerDelay()*1000;
        wxThread::Sleep( random(intervalSec) );
        FlybotAPI.SendPM(m_cid, m_answer);
        return NULL;
    }
};

wxString Session::GetVariable(const wxString& varName)
{
    if (wxT("LAST") == varName && !m_replies.empty())
    {
        return m_replies.Last();
    }
    else if (wxT("HISTORY") == varName && !m_replies.empty() )
    {
        return m_replies[random(m_replies.Count())];
    }
    
    return m_userinfo[varName];
}

wxString Session::SubstituteVars(const wxString& answer)
{
    wxString result = answer;
    wxRegEx reVar = wxT("\\$\\((.+)\\)"); // '$(varName)'
    while (reVar.Matches(result))
    { 
        // first bracketed subexpression is varName
        wxString varName = reVar.GetMatch(result, 1);

        reVar.Replace(&result, GetVariable(varName));
    }
    return result;
}

int Session::Answer(wxString& msg)
{
    m_replies.Add(msg);

    Phrase selectedPhrase = wxGetApp().Dict.GetMatchedTemplate(msg, &m_usedPhrases);

    // if no matches, exit;
    if (selectedPhrase.empty())
        return 0;

    // replace special vars (start with $) in answer;
    selectedPhrase.Answer = SubstituteVars(selectedPhrase.Answer);

    wxString answer = selectedPhrase.Answer;
    wxString cid = m_userinfo[FLYBOT_API_CID];

    // FIXME: special case - closed PM window will open again
    // when we send answer after delay, therefore answer delay should be ignored
    if (selectedPhrase.Flags.Freq(DICTIONARY_CLOSE_CHAR) > 0)
    {
        FlybotAPI.SendPM(cid, answer);
        ProcessFlags(selectedPhrase);
        return 0;
    }

    ProcessFlags(selectedPhrase);

    // this thread will sleep for desired time interval and then send answer    
    wxThread *answerThread = new AnswerThread(answer, cid);
    if (!cid.empty() && !answer.empty())
    {
        if  (wxTHREAD_NO_ERROR != answerThread->Create())
        {
            wxLogError(_("Cannot create worker thread"));
            return -1;
        }
        answerThread->Run();
    }

    return 0;
}

Session::~Session(void)
{
}
