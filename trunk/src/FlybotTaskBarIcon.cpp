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
	PU_YES,
	PU_NO,
	PU_CHECKMARK
};


BEGIN_EVENT_TABLE(FlybotTaskBarIcon, wxTaskBarIcon)
	EVT_MENU(PU_OPEN_DICT, FlybotTaskBarIcon::OnMenuOpenDict)
	EVT_MENU(PU_RELOAD_DICT,    FlybotTaskBarIcon::OnMenuReload)
	EVT_MENU(PU_SLOT_TIMEOUT_SUB, FlybotTaskBarIcon::OnMenuSub)
	EVT_MENU(PU_ANSWER_TIMEOUT_SUB, FlybotTaskBarIcon::OnMenuSub)
	EVT_MENU(PU_TIMEOUT1,FlybotTaskBarIcon::OnMenuCheckmark)
	EVT_MENU(PU_TIMEOUT2,FlybotTaskBarIcon::OnMenuCheckmark)
	EVT_TASKBAR_LEFT_UP(FlybotTaskBarIcon::OnLeftButtonUp)
	EVT_MENU(PU_BALOON_SUB, FlybotTaskBarIcon::OnMenuSub)
	EVT_MENU(PU_POWER, FlybotTaskBarIcon::OnPower)
END_EVENT_TABLE()

void FlybotTaskBarIcon::OnMenuOpenDict(wxCommandEvent& )
{
	wxGetApp().OpenDictionary();
}

void FlybotTaskBarIcon::OnMenuReload(wxCommandEvent& )
{
	wxGetApp().ReloadDictionary();
}

void FlybotTaskBarIcon::OnMenuSub(wxCommandEvent&)
{
}

void FlybotTaskBarIcon::OnMenuCheckmark(wxCommandEvent&)
{
}

// Overridables
wxMenu *FlybotTaskBarIcon::CreatePopupMenu()
{
	wxMenu *menu = new wxMenu;

	menu->Append(PU_OPEN_DICT, _("&Open dictionary"));
	menu->Append(PU_RELOAD_DICT, _("&Reload dicttionary"));
	/*
	menu->AppendSeparator();
	menu->Append(PU_CHECKMARK, _("Checkmark"),wxT(""), wxITEM_CHECK);

	wxMenu *submenuSlot = new wxMenu;
	submenuSlot->AppendRadioItem(PU_TIMEOUT1, _("Slot timeout 1"));
	submenuSlot->AppendRadioItem(PU_TIMEOUT2, _("Slot timeout 2"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _("Slot timeout"), submenuSlot);

	wxMenu *submenuAnswr = new wxMenu;
	submenuAnswr->AppendRadioItem(PU_TIMEOUT1, _("Answer timeout 1"));
	submenuAnswr->AppendRadioItem(PU_TIMEOUT2, _("Answer timeout 2"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _("Answer timeout"), submenuAnswr);

	wxMenu *submenuBaloon = new wxMenu;
	submenuBaloon->AppendRadioItem(PU_YES, _("Yes"));
	submenuBaloon->AppendRadioItem(PU_NO, _("No"));
	menu->Append(PU_BALOON_SUB, _("Show baloons"), submenuBaloon);
	*/
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

bool FlybotTaskBarIcon::ShowBalloon(const wxString &title, const wxString &message, unsigned int timeout, int icon)
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
