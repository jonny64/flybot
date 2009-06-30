#pragma once
#include "stdwx.h"
#include "tray.h"

class wxLogBaloon : public wxLog
{
	MyTaskBarIcon *m_taskBarIcon;
public:
	wxLogBaloon(MyTaskBarIcon *);
	~wxLogBaloon(void);
	
protected:
    // implement sink function
    virtual void DoLogString(const wxChar *, time_t);
};
