#pragma once
#include "stdwx.h"
#include "FlybotTaskBarIcon.h"

class wxLogBalloon : public wxLog
{
	FlybotTaskBarIcon *m_taskBarIcon;
public:
	wxLogBalloon(FlybotTaskBarIcon *);
	~wxLogBalloon(void);
	
protected:
    void DoLogString(const wxChar *, time_t, int icon  = NIIF_INFO);
	virtual void DoLog(wxLogLevel level, const wxChar *szString, time_t t);
};
