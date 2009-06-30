#pragma once
#include "stdwx.h"
#include "FlybotTaskBarIcon.h"

class wxLogBaloon : public wxLog
{
	FlybotTaskBarIcon *m_taskBarIcon;
public:
	wxLogBaloon(FlybotTaskBarIcon *);
	~wxLogBaloon(void);
	
protected:
    // implement sink function
    virtual void DoLogString(const wxChar *, time_t);
};
