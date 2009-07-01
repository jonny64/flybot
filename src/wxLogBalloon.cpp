#include "stdwx.h"
#include "wxLogBalloon.h"
#include "wxFlybotDLL.h"

wxLogBalloon::wxLogBalloon(FlybotTaskBarIcon *tb)
{
	m_taskBarIcon = tb;
}

void wxLogBalloon::DoLogString(const wxChar *szString, time_t WXUNUSED(t), int icon)
{
	int id = icon - NIIF_INFO;
	if (!(0 <= id && id < 3))
		return;

	wxString titles[] = {_("Information"), _("Warning"), _("Error")};
	if (NULL != m_taskBarIcon)
	{
		m_taskBarIcon->ShowBalloon(titles[id], wxString(szString), icon);
	}
}

void wxLogBalloon::DoLog(wxLogLevel level, const wxChar *szString, time_t t)
{
	switch ( level ) 
	{
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
