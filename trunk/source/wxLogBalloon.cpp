#include "stdwx.h"
#include "wxLogBalloon.h"
#include "wxFlybotDLL.h"
#include "wx/datetime.h"
wxLogBalloon::wxLogBalloon(FlybotTaskBarIcon *tb)
{
    m_taskBarIcon = tb;
}

void wxLogBalloon::DoLogStatus(const wxString& msg)
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

void wxLogBalloon::DoLogText(const wxString& msg, int icon)
{
    if ( msg.empty())
        return;

    // msg title and body are separated by special char
    wxArrayString tokens = wxSplit(msg, BALLOON_LOGGER_SEPARATOR_CHAR, BALLOON_LOGGER_ESCAPE_CHAR);
    wxASSERT( tokens.Count() > 0 );

    // if no title specified use default one ('flybot')
    wxString balloonText = tokens[0];
    wxString balloonTitle = tokens.Count() > 1? tokens[1] : wxT("flybot");

    if (NULL != m_taskBarIcon)
    {
        m_taskBarIcon->ShowBalloon(balloonTitle, balloonText, LOG_BALLOON_TIMEOUT_MS, icon);
    }
}


void wxLogBalloon::DoLogTextAtLevel(wxLogLevel level, const wxString& msg)
{
    switch ( level ) 
    {
    case wxLOG_FatalError:
        DoLogText(_("Fatal error: ") + msg + _("Program aborted."), wxICON_ERROR);
        abort();
        break;

    case wxLOG_Error:
        DoLogText(msg, wxICON_ERROR);
        break;

    case wxLOG_Warning:
        DoLogText(msg, wxICON_WARNING);
        break;

    case wxLOG_Status:
        DoLogStatus(msg);
        break;

    case wxLOG_Info:
    case wxLOG_Message:
    default:    // log unknown log levels too
        if (wxGetApp().Config.BalloonsEnabled())
            DoLogText(msg);
        break;

    case wxLOG_Trace:
    case wxLOG_Debug:
#ifdef __WXDEBUG__
        {
            wxString msg = level == wxLOG_Trace ? wxT("Trace: ")
                : wxT("Debug: ");
            msg << msg;
            DoLogText(msg);
        }
#endif // Debug
        break;
    }
}


void wxLogBalloon::DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info)
{
    //msg = wxString::Format("%s%s", msg, wxDateTime(info.timestamp) );
    DoLogTextAtLevel(level, msg);
}

wxLogBalloon::~wxLogBalloon(void)
{
}

