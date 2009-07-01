#pragma once
#include "stdwx.h"
#include <shellapi.h>
#include "FlybotConfig.h"

class FlybotTaskBarIcon: public wxTaskBarIcon
{
public:
#if defined(__WXCOCOA__)
	FlybotTaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
		:   wxTaskBarIcon(iconType);
#else
	FlybotTaskBarIcon();
#endif

	bool ShowBalloon(const wxString&, const wxString&, int icon = NIIF_INFO, unsigned int timeout = 4);
	void OnLeftButtonUp(wxTaskBarIconEvent&);
	void OnMenuOpenDict(wxCommandEvent&);
	void OnMenuReload(wxCommandEvent&);
	void OnPower(wxCommandEvent&);
	void OnMenuUseBalloonsClick(wxCommandEvent &evt);
	void OnMenuSlotTimeoutClick(wxCommandEvent &evt);
	void SetupIcon();

	virtual wxMenu *CreatePopupMenu();

	DECLARE_EVENT_TABLE()
};
