#include "stdwx.h"
#include "wxLogBaloon.h"
#include "wxFlybotDLL.h"

wxLogBaloon::wxLogBaloon(MyTaskBarIcon *tb)
{
	m_taskBarIcon = tb;
}

void wxLogBaloon::DoLogString(const wxChar *szString, time_t WXUNUSED(t))
{
	if (NULL != m_taskBarIcon)
	{
		m_taskBarIcon->ShowBalloon(wxString(wxT("Information")), wxString(szString));
	}
}

wxLogBaloon::~wxLogBaloon(void)
{
}
