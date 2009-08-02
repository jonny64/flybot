#include "stdwx.h"
#include <wx/utils.h>
#include "FlybotTaskBarIcon.h"
#include "resource.h"
#include "wxFlybotDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DECLARE_APP(wxFlybotDLL)

enum 
{
    PU_OPEN_DICT = 10001,
    PU_RELOAD_DICT,
    PU_SLOT_TIMEOUT_SUB,
    PU_ANSWER_TIMEOUT_SUB,
    PU_BALOON_SUB,
    PU_POWER,
    PU_TIMEOUT1,
    PU_TIMEOUT2,
    PU_CHECKMARK,
    wxID_SLOT_TIMEOUT_BEGIN = 10050
};


BEGIN_EVENT_TABLE(FlybotTaskBarIcon, wxTaskBarIcon)
    EVT_TASKBAR_LEFT_UP(FlybotTaskBarIcon::OnLeftButtonUp)
    EVT_MENU(PU_OPEN_DICT, FlybotTaskBarIcon::OnMenuOpenDict)
    EVT_MENU(PU_RELOAD_DICT,    FlybotTaskBarIcon::OnMenuReload)
    EVT_MENU_RANGE(wxID_YES, wxID_NO, FlybotTaskBarIcon::OnMenuUseBalloonsClick)
    EVT_MENU(PU_POWER, FlybotTaskBarIcon::OnPower)
END_EVENT_TABLE()

FlybotTaskBarIcon::FlybotTaskBarIcon()
{
}

void FlybotTaskBarIcon::OnMenuOpenDict(wxCommandEvent& )
{
    wxGetApp().OpenDictionary();
}

void FlybotTaskBarIcon::OnMenuReload(wxCommandEvent& )
{
    wxGetApp().ReloadDictionary();
}

void FlybotTaskBarIcon::OnMenuSlotTimeoutClick(wxCommandEvent &evt)
{
    wxGetApp().Config.SetSelectedSlotTimeoutId(evt.GetId() - wxID_SLOT_TIMEOUT_BEGIN);
}

void FlybotTaskBarIcon::OnMenuUseBalloonsClick(wxCommandEvent& evt)
{
    wxGetApp().Config.Write(SETTING_USE_BALLOONS,  evt.GetId() == wxID_YES);
}

// converts time in seconds to it string representataion (xx hrs/days/weeks/etc.)
static wxString ToTimeString(int seconds)
{
    const wxString labels[] = {_("min."), _("hr."), _("day"), _("wk.") };
    const int limits[] = {60, 3600, 86400, 86400*7};
    const int LIMITS_COUNT = 5;

    wxString tail = _("sec.");
    int time = seconds;
    int i = 0;
    while (i < LIMITS_COUNT && seconds >= limits[i])
    { 
        time = seconds / limits[i];
        tail = labels[i];
        i++;
    }
    return wxString::Format( wxT("%d %s"), time, tail);
}

// Overridables
wxMenu *FlybotTaskBarIcon::CreatePopupMenu()
{
    FlybotConfig *conf = &wxGetApp().Config;
    wxMenu *menu = new wxMenu;

    menu->Append(PU_OPEN_DICT, _("&Open dictionary"));
    menu->Append(PU_RELOAD_DICT, _("&Reload dicttionary"));
    menu->AppendSeparator();
    
    // form slot timeouts submenu
    wxMenu *submenuSlot = new wxMenu;
    int index = 0;
    for (list<int>::iterator it = conf->SlotTimeouts.begin(); it != conf->SlotTimeouts.end(); ++it)
    {
        submenuSlot->AppendRadioItem(
            wxID_SLOT_TIMEOUT_BEGIN + index, 
            wxString::Format( ToTimeString(*it) )
            );
        Connect(wxID_SLOT_TIMEOUT_BEGIN + index, 
            wxEVT_COMMAND_MENU_SELECTED, 
            wxCommandEventHandler(FlybotTaskBarIcon::OnMenuSlotTimeoutClick));
        index++;
    }
    submenuSlot->Check(wxID_SLOT_TIMEOUT_BEGIN + conf->GetSelectedSlotTimeoutId(), true);
    menu->Append(PU_SLOT_TIMEOUT_SUB, _("Slot timeout"), submenuSlot);

    /*
    wxMenu *submenuAnswr = new wxMenu;
    submenuAnswr->AppendRadioItem(PU_TIMEOUT1, _("Answer timeout 1"));
    submenuAnswr->AppendRadioItem(PU_TIMEOUT2, _("Answer timeout 2"));
    menu->Append(PU_ANSWER_TIMEOUT_SUB, _("Answer timeout"), submenuAnswr);
    */

    // form 'enable status balloons' submenu
    wxMenu *submenuBalloon = new wxMenu;
    submenuBalloon->AppendRadioItem(wxID_YES, _("Yes"));
    submenuBalloon->AppendRadioItem(wxID_NO, _("No"));
    submenuBalloon->Check(conf->BalloonsEnabled()? wxID_YES:wxID_NO, true);
    menu->Append(PU_BALOON_SUB, _("&Show balloons"), submenuBalloon);
    menu->AppendSeparator();

    menu->Append(PU_POWER, _("&On/Off"));    

    return menu;
}

void FlybotTaskBarIcon::OnLeftButtonUp(wxTaskBarIconEvent&)
{
    wxGetApp().SwitchState();
    SetupIcon();
}

void FlybotTaskBarIcon::OnPower(wxCommandEvent&)
{
    wxGetApp().SwitchState();
    SetupIcon();
}

void FlybotTaskBarIcon::SetupIcon()
{
    HICON hIconOnline = LoadIcon(wxGetInstance(), MAKEINTRESOURCE(IDI_ICON_ONLINE));
    HICON hIconOffline = LoadIcon(wxGetInstance(), MAKEINTRESOURCE(IDI_ICON_OFFLINE));
    HICON hIcon = wxGetApp().GetEnabledState()? hIconOnline : hIconOffline;

    wxIcon trayIcon;
    trayIcon.SetHICON(hIcon);
    // TODO: find out why normal loading from resources doesn't work
    // SetupIcon(wxIcon(IDI_ICON_ONLINE), wxT("flybot 0.3 alpha") )
    if (!SetIcon(trayIcon, wxT("flybot 0.3 alpha")) )
        wxLogError(_("Could not set icon."));
}

bool FlybotTaskBarIcon::ShowBalloon(const wxString &title, const wxString &message, int icon, unsigned int timeout)
{
    if (!IsOk())
        return false;

    NOTIFYICONDATA notifyData = {0};
    notifyData.uFlags = NIF_INFO | NIF_TIP;
    notifyData.dwInfoFlags = icon | NIIF_NOSOUND;
    notifyData.uTimeout = timeout * 1000;    

    // find our icon (see wxTaskBarIcon implementation for details)
    notifyData.hWnd = GetHwndOf((wxFrame *)m_win);
    notifyData.uID = 99;

    wxStrncpy(notifyData.szInfo, message.c_str(), WXSIZEOF(notifyData.szInfo));
    wxStrncpy(notifyData.szInfoTitle, title.c_str(), WXSIZEOF(notifyData.szInfoTitle));

    // targeting Win2000+
    notifyData.cbSize = NOTIFYICONDATA_V2_SIZE;

    return m_iconAdded && TRUE == Shell_NotifyIcon(NIM_MODIFY, &notifyData);
}
