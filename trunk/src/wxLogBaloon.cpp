#include "stdwx.h"
#include "wxLogBaloon.h"
#include "wxFlybotDLL.h"

wxLogBaloon::wxLogBaloon(FlybotTaskBarIcon *tb)
{
	m_taskBarIcon = tb;
}

void wxLogBaloon::DoLogString(const wxChar *szString, time_t WXUNUSED(t), int icon)
{
	if (!(0 <= icon - NIIF_INFO && icon - NIIF_INFO < 3));
		return;

	wxString titles[] = {_("Information"), _("Warning"), _("Error")};
	if (NULL != m_taskBarIcon)
	{
		m_taskBarIcon->ShowBalloon(titles[icon - NIIF_INFO], wxString(szString), icon);
	}
}

void wxLogBaloon::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
	switch ( level ) {
		case wxLOG_FatalError:
			DoLogString(wxString(_("Fatal error: ")) + szString + _("Program aborted."), t, NIIF_ERROR);
			Flush();
			abort();
			break;

		case wxLOG_Error:
			DoLogString(wxString(szString), t, NIIF_ERROR);
			break;

		case wxLOG_Warning:
			DoLogString(wxString(szString), t, NIIF_WARNING);
			break;

		case wxLOG_Info:
			if ( GetVerbose() )
		case wxLOG_Message:
		case wxLOG_Status:
		default:    // log unknown log levels too
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

wxLogBaloon::~wxLogBaloon(void)
{
}
