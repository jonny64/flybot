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

void wxLogBalloon::DoLogStatus(const wxString &msg)
{
    wxString logFileName = FlybotAPI.ConfigPath + wxT("Logs\\flybot.log");
    FILE *logFile = fopen(logFileName.c_str(), "a");
    wxLog *oldLogger = wxLog::SetActiveTarget(new wxLogStderr(logFile));

    wxLogMessage(msg);

    delete wxLog::SetActiveTarget(oldLogger);
    if (logFile != NULL)
    {
        fclose(logFile);
    }
}

void wxLogBalloon::DoLog(wxLogLevel level, const wxString& message, time_t t)
{
    switch ( level ) 
    {
    case wxLOG_FatalError:
        DoLogString(_("Fatal error: ") + message + _("Program aborted."), t, wxICON_ERROR);
        Flush();
        abort();
        break;

    case wxLOG_Error:
        DoLogString(message, t, wxICON_ERROR);
        break;

    case wxLOG_Warning:
        DoLogString(message, t, wxICON_WARNING);
        break;

    case wxLOG_Status:
        DoLogStatus(message);
        break;

    case wxLOG_Info:
    case wxLOG_Message:
    default:    // log unknown log levels too
        if (wxGetApp().Config.BalloonsEnabled())
            DoLogString(message, t);
        break;

    case wxLOG_Trace:
    case wxLOG_Debug:
#ifdef __WXDEBUG__
        {
            wxString msg = level == wxLOG_Trace ? wxT("Trace: ")
                : wxT("Debug: ");
            msg << message;
            DoLogString(msg, t);
        }
#endif // Debug
        break;
    }
}

wxLogBalloon::~wxLogBalloon(void)
{
}

