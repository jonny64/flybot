#pragma once
#include "stdwx.h"
#include <shellapi.h>
#include "FlybotConfig.h"
#include "Dialogs/SendPMDialog.h"

class FlybotTaskBarIcon: public wxTaskBarIcon
{
    SendPMDialog *m_sendDlg;

    void OnUpdateUI(wxUpdateUIEvent &evt);
    void OnMenuClick(wxCommandEvent &evt);

    void OnMenuSlotTimeoutClick(wxCommandEvent &evt);
    void OnMenuAnswerDelayClick(wxCommandEvent &evt);
public:
#if defined(__WXCOCOA__)
    FlybotTaskBarIcon(wxTaskBarIconType iconType = DEFAULT_TYPE)
        :   wxTaskBarIcon(iconType);
#else
    FlybotTaskBarIcon();
#endif
    ~FlybotTaskBarIcon();

    bool ShowBalloon(const wxString&, const wxString&, int icon = NIIF_INFO, unsigned int timeout = 4);
    void OnLeftButtonUp(wxTaskBarIconEvent&);
    void SetupIcon();

    virtual wxMenu *CreatePopupMenu();

    DECLARE_EVENT_TABLE()
};
