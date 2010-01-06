#pragma once
#include "stdwx.h"
#include <shellapi.h>
#include "FlybotConfig.h"

class FlybotTaskBarIcon: public wxTaskBarIcon
{
    void OnUpdateUI(wxUpdateUIEvent &evt);
    void OnMenuClick(wxCommandEvent &evt);

    void OnMenuSlotTimeoutClick(wxCommandEvent &evt);
    void OnMenuAnswerDelayClick(wxCommandEvent &evt);
public:
    FlybotTaskBarIcon();

    void OnLeftButtonUp(wxTaskBarIconEvent&);
    void SetupIcon();

    virtual wxMenu *CreatePopupMenu();

    DECLARE_EVENT_TABLE()
};
