#pragma once
#include "stdwx.h"
#include "FlybotTaskBarIcon.h"

const wxChar BALLOON_LOGGER_SEPARATOR_CHAR = wxChar('#');
const wxChar BALLOON_LOGGER_ESCAPE_CHAR = wxChar('\\');
const int LOG_BALLOON_TIMEOUT_MS = 4000;

#define DECLARE_BALLOON_LOG_FUNCTION(level)                        \
void wxLog##level(const wxString &title, const wxString &message)

DECLARE_BALLOON_LOG_FUNCTION(FatalError);
DECLARE_BALLOON_LOG_FUNCTION(Error);
DECLARE_BALLOON_LOG_FUNCTION(Warning);
DECLARE_BALLOON_LOG_FUNCTION(Message);
DECLARE_BALLOON_LOG_FUNCTION(Info);

class wxLogBalloon : public wxLog
{
    FlybotTaskBarIcon *m_taskBarIcon;
public:
    wxLogBalloon(FlybotTaskBarIcon *);
    ~wxLogBalloon(void);
    
protected:
    void DoLogString(const wxString&, time_t, int icon  = wxICON_INFORMATION);
    virtual void DoLog(wxLogLevel level, const wxString& szString, time_t t);
};
