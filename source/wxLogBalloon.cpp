#include "stdwx.h"
#include "wxLogBalloon.h"
#include "wxFlybotDLL.h"

wxLogBalloon::wxLogBalloon(FlybotTaskBarIcon *tb)
{
    m_taskBarIcon = tb;
}

void wxLogBalloon::DoLogString(const wxString& message, time_t WXUNUSED(t), int icon)
{
    if ( message.empty())
        return;

    // message title and body are separated by special char
    wxArrayString tokens = wxSplit(message, BALLOON_LOGGER_SEPARATOR_CHAR, BALLOON_LOGGER_ESCAPE_CHAR);
    wxASSERT( tokens.Count() > 0 );

    // if no title specified use default one
    wxString balloonText = tokens[0];
    wxString balloonTitle = tokens.Count() > 1? tokens[1] : wxT("flybot");

    if (NULL != m_taskBarIcon)
    {
        m_taskBarIcon->ShowBalloon(balloonTitle, balloonText, LOG_BALLOON_TIMEOUT_MS, icon);
    }
}

#define IMPLEMENT_BALLOON_LOG_FUNCTION(level)                                                                                     \
void wxLog##level(const wxString &title, const wxString &message)\
{                                                                \
    wxString msg  = wxString::Format(                            \
        wxT("%s%c%s"),                                           \
        message,                                                 \
        BALLOON_LOGGER_SEPARATOR_CHAR,                           \
        title                                                    \
    );                                                           \
    /* use default implementation, which later calls DoLog */    \
    wxLog##level(msg);                                           \
}


IMPLEMENT_BALLOON_LOG_FUNCTION(Error)
IMPLEMENT_BALLOON_LOG_FUNCTION(Warning)
IMPLEMENT_BALLOON_LOG_FUNCTION(Message)
IMPLEMENT_BALLOON_LOG_FUNCTION(Info)
IMPLEMENT_BALLOON_LOG_FUNCTION(Status)

void wxLogBalloon::DoLog(wxLogLevel level, const wxString& szString, time_t t)
{
    switch ( level ) 
    {
    case wxLOG_FatalError:
        DoLogString(_("Fatal error: ")) + szString + _("Program aborted."), t, wxICON_ERROR);
        Flush();
        abort();
        break;

    case wxLOG_Error:
        DoLogString(szString, t, wxICON_ERROR);
        break;

    case wxLOG_Warning:
        DoLogString(szString, t, wxICON_WARNING);
        break;

    case wxLOG_Info:
        if ( GetVerbose() )
    case wxLOG_Message:
    case wxLOG_Status:
    default:    // log unknown log levels too
        if (wxGetApp().Config.BalloonsEnabled())
            DoLogString(szString, t);
        break;

    case wxLOG_Trace:
    case wxLOG_Debug:
#ifdef __WXDEBUG__
        {
            wxString msg = level == wxLOG_Trace ? wxT("Trace: ")
                : wxT("Debug: ");
            msg << szString;
            DoLogString(msg, t);
        }
#endif // Debug
        break;
    }
}

wxLogBalloon::~wxLogBalloon(void)
{
}

