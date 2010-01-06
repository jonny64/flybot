#pragma once
#include "stdwx.h"
#include "FlybotTaskBarIcon.h"

const wxChar BALLOON_LOGGER_SEPARATOR_CHAR = wxChar('#');
const wxChar BALLOON_LOGGER_ESCAPE_CHAR = wxChar('\\');
const int LOG_BALLOON_TIMEOUT_MS = 4000;


#define MESSAGE_WITH_TITLE(message, title)                                                                                     \
    wxString::Format(											\
	    wxT("%s%c%s"),                                           \
	    message,                                                 \
	    BALLOON_LOGGER_SEPARATOR_CHAR,                           \
	    title                                                    \
    )

class wxLogBalloon : public wxLog
{
    FlybotTaskBarIcon *m_taskBarIcon;
    void DoLogStatus(const wxString& msg);
public:
    wxLogBalloon(FlybotTaskBarIcon *);
    ~wxLogBalloon(void);
    
protected:
    void DoLogRecord(wxLogLevel level, const wxString& msg, const wxLogRecordInfo& info);
    void DoLogTextAtLevel(wxLogLevel level, const wxString& msg);
    void DoLogText(const wxString& msg, int icon = wxICON_INFORMATION);
};
