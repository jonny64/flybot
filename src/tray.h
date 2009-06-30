#pragma once

#include "stdwx.h"
#include <shellapi.h>

class MyTaskBarIcon: public wxTaskBarIcon
{
public:
#if defined(__WXCOCOA__)
	MyTaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
		:   wxTaskBarIcon(iconType)
#else
	MyTaskBarIcon()
#endif
	{}

	bool ShowBalloon(const wxString&, const wxString&, unsigned int timeout = 4, int icon = NIIF_INFO);
	void OnLeftButtonDClick(wxTaskBarIconEvent&);
	void OnMenuOpenDict(wxCommandEvent&);
	void OnMenuReload(wxCommandEvent&);
	void OnPower(wxCommandEvent&);
	void OnMenuCheckmark(wxCommandEvent&);
	void OnMenuSub(wxCommandEvent&);
	void SwitchIcon();

	virtual wxMenu *CreatePopupMenu();

	DECLARE_EVENT_TABLE()
};
