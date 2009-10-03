#include "stdwx.h"
#include <wx/utils.h>
#include "FlybotTaskBarIcon.h"
#include "resource.h"
#include "wxFlybotDLL.h"
#include "flybot.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DECLARE_APP(wxFlybotDLL)

enum 
{
    wxID_OPEN_DICT = 10001,
    wxID_RELOAD_DICT,
    wxID_SLOT_TIMEOUT_SUB,
    wxID_ANSWER_TIMEOUT_SUB,
    wxID_POWER,
    wxID_USE_BALLOONS,
    wxID_SLOT_TIMEOUT_BEGIN = 10050,
    wxID_ANSWER_DELAY_BEGIN = 10100
};


BEGIN_EVENT_TABLE(FlybotTaskBarIcon, wxTaskBarIcon)
    EVT_TASKBAR_LEFT_UP(FlybotTaskBarIcon::OnLeftButtonUp)
    EVT_MENU(wxID_OPEN_DICT, FlybotTaskBarIcon::OnMenuClick)
    EVT_MENU(wxID_RELOAD_DICT, FlybotTaskBarIcon::OnMenuClick)
    EVT_MENU(wxID_USE_BALLOONS, FlybotTaskBarIcon::OnMenuClick)
    EVT_MENU(wxID_POWER, FlybotTaskBarIcon::OnMenuClick)
    EVT_UPDATE_UI(wxID_USE_BALLOONS, FlybotTaskBarIcon::OnUpdateUI)
    EVT_UPDATE_UI(wxID_POWER, FlybotTaskBarIcon::OnUpdateUI)
END_EVENT_TABLE()

FlybotTaskBarIcon::FlybotTaskBarIcon()
{
    SetupIcon();
}

void FlybotTaskBarIcon::OnMenuSlotTimeoutClick(wxCommandEvent &evt)
{
    wxGetApp().Config.Write(SETTING_SLOT_TIMEOUT, evt.GetId() - wxID_SLOT_TIMEOUT_BEGIN);
}

void FlybotTaskBarIcon::OnMenuAnswerDelayClick(wxCommandEvent &evt)
{
    wxGetApp().Config.Write(SETTING_ANSWER_DELAY, evt.GetId() - wxID_ANSWER_DELAY_BEGIN);
}

void FlybotTaskBarIcon::OnMenuClick(wxCommandEvent& evt)
{
    bool newState;
    switch (evt.GetId())
    {
        case wxID_USE_BALLOONS:
            newState = !wxGetApp().Config.BalloonsEnabled();
            wxGetApp().Config.Write(SETTING_USE_BALLOONS, newState );
            break;
            
        case wxID_OPEN_DICT:
            wxGetApp().OpenDictionary();
            break;
            
        case wxID_RELOAD_DICT:
            wxGetApp().ReloadDictionary();
            break;

        case wxID_POWER:    
            wxGetApp().SwitchState();
            SetupIcon();
            break;

        default:
            ;
    }
}

void FlybotTaskBarIcon::OnUpdateUI(wxUpdateUIEvent &evt)
{
    switch (evt.GetId())
    {
        case wxID_USE_BALLOONS:
            evt.Check(wxGetApp().Config.BalloonsEnabled());
            break;

        case wxID_POWER:
            evt.Check(wxGetApp().GetEnabledState());
            break;

        default:
            ;
    }
}

// converts time in seconds to it string representataion (xx hrs/days/weeks/etc.)
static wxString ToTimeString(int seconds)
{
    const wxString labels[] = {_("min."), _("hr."), _("day"), _("wk.") };
    const int limits[] = {60, 3600, 86400, 86400*7};

    wxString tail = _("sec.");
    int time = seconds;
    int i = 0;
    while (i < ARRAYSIZE(limits) && seconds >= limits[i])
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

    menu->AppendCheckItem(wxID_POWER, _("&Online"));
    menu->AppendCheckItem(wxID_USE_BALLOONS, _("&Show balloons"));
    
    menu->AppendSeparator();
    
    // form slot timeouts submenu
    wxMenu *submenuSlot = new wxMenu;
    int index = 0;
    vector<int> timeouts = conf->GetSlotTimeouts();
    FOREACH(vector<int>, it, timeouts)
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
    submenuSlot->Check(wxID_SLOT_TIMEOUT_BEGIN + conf->GetSlotTimeoutId(), true);
    menu->Append(wxID_SLOT_TIMEOUT_SUB, _("Slot timeout"), submenuSlot);

    // form answer delays submenu
    wxMenu *submenuDelay = new wxMenu;
    index = 0;
    vector<int> delays = conf->GetAnswerDelays();
    FOREACH(vector<int>, it, delays)
    {
        submenuDelay->AppendRadioItem(
            wxID_ANSWER_DELAY_BEGIN + index, 
            wxString::Format( wxT("%d %s"), *it, _("sec."))
            );
        Connect(wxID_ANSWER_DELAY_BEGIN + index, 
            wxEVT_COMMAND_MENU_SELECTED, 
            wxCommandEventHandler(FlybotTaskBarIcon::OnMenuAnswerDelayClick));
        index++;
    }
    submenuDelay->Check(wxID_ANSWER_DELAY_BEGIN + conf->GetAnswerDelayId(), true);
    menu->Append(wxID_ANSWER_TIMEOUT_SUB, _("Answer delay"), submenuDelay);

    menu->AppendSeparator();

    menu->Append(wxID_RELOAD_DICT, _("&Reload dicttionary"));
    menu->Append(wxID_OPEN_DICT, _("&Open dictionary"));

    return menu;
}

void FlybotTaskBarIcon::OnLeftButtonUp(wxTaskBarIconEvent&)
{
    wxGetApp().SwitchState();
    SetupIcon();
}

void FlybotTaskBarIcon::SetupIcon()
{
    HICON hIconOnline = LoadIcon(wxGetInstance(), MAKEINTRESOURCE(IDI_ICON_ONLINE));
    HICON hIconOffline = LoadIcon(wxGetInstance(), MAKEINTRESOURCE(IDI_ICON_OFFLINE));
    HICON hIcon = wxGetApp().GetEnabledState()? hIconOnline : hIconOffline;

    wxString tooltipText =  wxString::Format(wxT("flybot %s (%s)"), APP_VERSION, APP_BUILD_DATE);
    wxIcon trayIcon;
    trayIcon.SetHICON(hIcon);
    // TODO: find out why normal loading from resources doesn't work
    // SetupIcon(wxIcon(IDI_ICON_ONLINE), wxT("flybot 0.3") )
    if (!SetIcon(trayIcon, tooltipText) )
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
