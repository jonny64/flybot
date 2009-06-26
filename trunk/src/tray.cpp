#include "stdwx.h"
#include "tray.h"
#include "resource.h"
#include "wxFlybotDLL.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

DECLARE_APP(wxFlybotDLL)

enum {
    PU_OPEN_DICT = 10001,
    PU_RELOAD_DICT,
    PU_SLOT_TIMEOUT_SUB,
    PU_ANSWER_TIMEOUT_SUB,
    PU_BALOON_SUB,
    PU_POWER,
	PU_TIMEOUT1,
	PU_TIMEOUT2,
	PU_YES,
	PU_NO,
	PU_CHECKMARK
};


BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_OPEN_DICT, MyTaskBarIcon::OnMenuOpenDict)
    EVT_MENU(PU_RELOAD_DICT,    MyTaskBarIcon::OnMenuReload)
    EVT_MENU(PU_SLOT_TIMEOUT_SUB, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_ANSWER_TIMEOUT_SUB, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_TIMEOUT1,MyTaskBarIcon::OnMenuCheckmark)
    EVT_MENU(PU_TIMEOUT2,MyTaskBarIcon::OnMenuCheckmark)
    EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DCLICK(MyTaskBarIcon::OnLeftButtonDClick)
	EVT_MENU(PU_BALOON_SUB, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_POWER, MyTaskBarIcon::OnPower)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuOpenDict(wxCommandEvent& )
{
    //dialog->Show(true);
}

void MyTaskBarIcon::OnMenuReload(wxCommandEvent& )
{
	wxGetApp().ReloadDictionary();
}

void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& event)
{
}

void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent&event)
{
}

void MyTaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
}

void MyTaskBarIcon::OnMenuSetOldIcon(wxCommandEvent&)
{
    if (IsIconInstalled())
    {
        RemoveIcon();
    }
    else
    {
        wxMessageBox(wxT("wxTaskBarIcon Sample - icon already is the old version"));
    }
}

void MyTaskBarIcon::OnMenuSub(wxCommandEvent&)
{
    wxMessageBox(wxT("You clicked on a submenu!"));
}

// Overridables
wxMenu *MyTaskBarIcon::CreatePopupMenu()
{
	
    wxMenu *menu = new wxMenu;
	
	menu->Append(PU_OPEN_DICT, _T("&Open dict"));
    menu->Append(PU_RELOAD_DICT, _T("&Reload dict"));
    menu->AppendSeparator();
    // menu->Append(PU_CHECKMARK, _T("Checkmark"),wxT(""), wxITEM_CHECK);
    
	wxMenu *submenuSlot = new wxMenu;
    submenuSlot->AppendRadioItem(PU_TIMEOUT1, _T("Slot timeout 1"));
    submenuSlot->AppendRadioItem(PU_TIMEOUT2, _T("Slot timeout 2"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _T("Slot timeout"), submenuSlot);

	wxMenu *submenuAnswr = new wxMenu;
    submenuAnswr->AppendRadioItem(PU_TIMEOUT1, _T("Answer timeout 1"));
    submenuAnswr->AppendRadioItem(PU_TIMEOUT2, _T("Answer timeout 2"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _T("Answer timeout"), submenuAnswr);

	wxMenu *submenuBaloon = new wxMenu;
    submenuBaloon->AppendRadioItem(PU_YES, _T("Yes"));
    submenuBaloon->AppendRadioItem(PU_NO, _T("No"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _T("Show baloons"), submenuBaloon);
	menu->AppendSeparator();

	menu->Append(PU_POWER, _T("&On/Off"));    

    return menu;
}

void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
	SwitchIcon();
}

void MyTaskBarIcon::OnPower(wxCommandEvent&)
{
	SwitchIcon();
}

void MyTaskBarIcon::SwitchIcon()
{
	static bool online = false;
	online = !online;
	
	HICON hIconOnline = LoadIcon(wxGetInstance(), MAKEINTRESOURCE(IDI_ICON_ONLINE));
	HICON hIconOffline = LoadIcon(wxGetInstance(), MAKEINTRESOURCE(IDI_ICON_OFFLINE));
	HICON hIcon = online? hIconOnline : hIconOffline;

	wxIcon trayIcon;
	trayIcon.SetHICON(hIcon);
	// TODO: find out why normal loading from resources doesn't work
	// SetIcon(wxIcon(IDI_ICON_ONLINE), wxT("flybot 0.3 alpha") )
	if (!SetIcon(trayIcon, wxT("flybot 0.3 alpha")) )
		wxMessageBox(wxT("Could not set icon."));
}
