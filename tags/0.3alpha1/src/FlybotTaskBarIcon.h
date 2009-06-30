#pragma once

#include "stdwx.h"
#include <shellapi.h>

class FlybotTaskBarIcon: public wxTaskBarIcon
{
public:
#if defined(__WXCOCOA__)
	FlybotTaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
		:   wxTaskBarIcon(iconType)
#else
	FlybotTaskBarIcon()
#endif
	{}

	bool ShowBalloon(const wxString&, const wxString&, unsigned int timeout = 4, int icon = NIIF_INFO);
	void OnLeftButtonUp(wxTaskBarIconEvent&);
	void OnMenuOpenDict(wxCommandEvent&);
	void OnMenuReload(wxCommandEvent&);
	void OnPower(wxCommandEvent&);
	void OnMenuCheckmark(wxCommandEvent&);
	void OnMenuSub(wxCommandEvent&);
	void SetupIcon();

	virtual wxMenu *CreatePopupMenu();

	DECLARE_EVENT_TABLE()
};
