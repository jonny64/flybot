#pragma once

#include "stdwx.h"

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

    void OnLeftButtonDClick(wxTaskBarIconEvent&);
    void OnMenuOpenDict(wxCommandEvent&);
    void OnMenuReload(wxCommandEvent&);
	void OnPower(wxCommandEvent&);
    void OnMenuSetNewIcon(wxCommandEvent&);
    void OnMenuSetOldIcon(wxCommandEvent&);
       void OnMenuCheckmark(wxCommandEvent&);
       void OnMenuUICheckmark(wxUpdateUIEvent&);
    void OnMenuSub(wxCommandEvent&);
    void SwitchIcon();

    virtual wxMenu *CreatePopupMenu();

DECLARE_EVENT_TABLE()
};
