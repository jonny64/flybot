// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "../sample.xpm"
#endif

#include "smile.xpm"

#include "wx/taskbar.h"
#include "tray.h"

// Declare two frames
MyDialog   *dialog = NULL;

BEGIN_EVENT_TABLE(MyDialog, wxDialog)
    EVT_BUTTON(wxID_OK, MyDialog::OnOK)
    EVT_BUTTON(wxID_EXIT, MyDialog::OnExit)
    EVT_CLOSE(MyDialog::OnCloseWindow)
END_EVENT_TABLE()


MyDialog::MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
    const wxPoint& pos, const wxSize& size, const long windowStyle):
  wxDialog(parent, id, title, pos, size, windowStyle)
{
    Init();
}

MyDialog::~MyDialog()
{
    delete m_taskBarIcon;
#if defined(__WXCOCOA__)
    delete m_dockIcon;
#endif
}

void MyDialog::OnOK(wxCommandEvent& WXUNUSED(event))
{
    Show(false);
}

void MyDialog::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyDialog::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
    Destroy();
}

void MyDialog::Init(void)
{
  (void)new wxStaticText(this, wxID_ANY, _T("Press 'Hide me' to hide me, Exit to quit."),
                         wxPoint(10, 20));

  (void)new wxStaticText(this, wxID_ANY, _T("Double-click on the taskbar icon to show me again."),
                         wxPoint(10, 40));

  (void)new wxButton(this, wxID_EXIT, _T("Exit"), wxPoint(185, 230), wxSize(80, 25));
  (new wxButton(this, wxID_OK, _T("Hide me"), wxPoint(100, 230), wxSize(80, 25)))->SetDefault();
  Centre(wxBOTH);
   
  m_taskBarIcon = new MyTaskBarIcon();
#if defined(__WXCOCOA__)
  m_dockIcon = new MyTaskBarIcon(wxTaskBarIcon::DOCK);
#endif
  if (!m_taskBarIcon->SetIcon(wxICON(sample), wxT("wxTaskBarIcon Sample")))
        wxMessageBox(wxT("Could not set icon."));
}


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
	PU_NO
};


BEGIN_EVENT_TABLE(MyTaskBarIcon, wxTaskBarIcon)
    EVT_MENU(PU_OPEN_DICT, MyTaskBarIcon::OnMenuOpenDict)
    EVT_MENU(PU_RELOAD_DICT,    MyTaskBarIcon::OnMenuReload)
    EVT_MENU(PU_SLOT_TIMEOUT_SUB, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_ANSWER_TIMEOUT_SUB, MyTaskBarIcon::OnMenuSub)
    //EVT_MENU(PU_CHECKMARK,MyTaskBarIcon::OnMenuCheckmark)
    //EVT_UPDATE_UI(PU_CHECKMARK,MyTaskBarIcon::OnMenuUICheckmark)
    EVT_TASKBAR_LEFT_DCLICK  (MyTaskBarIcon::OnLeftButtonDClick)
    EVT_MENU(PU_BALOON_SUB, MyTaskBarIcon::OnMenuSub)
    EVT_MENU(PU_POWER, MyTaskBarIcon::OnPower)
END_EVENT_TABLE()

void MyTaskBarIcon::OnMenuOpenDict(wxCommandEvent& )
{
    //dialog->Show(true);
}

void MyTaskBarIcon::OnMenuReload(wxCommandEvent& )
{
    //dialog->Close(true);
}

static bool check = true;

void MyTaskBarIcon::OnMenuCheckmark(wxCommandEvent& )
{
       check =!check;
}

void MyTaskBarIcon::OnMenuUICheckmark(wxUpdateUIEvent &event)
{
       event.Check( check );
}

void MyTaskBarIcon::OnMenuSetNewIcon(wxCommandEvent&)
{
    wxIcon icon(smile_xpm);

    if (!SetIcon(icon, wxT("wxTaskBarIcon Sample - a different icon")))
        wxMessageBox(wxT("Could not set new icon."));
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
    menu->Append(PU_OPEN_DICT, _T("&Reload dict"));
    menu->AppendSeparator();
    // menu->Append(PU_CHECKMARK, _T("Checkmark"),wxT(""), wxITEM_CHECK);
    
	wxMenu *submenuSlot = new wxMenu;
    submenuSlot->Append(PU_TIMEOUT1, _T("Slot timeout 1"));
    submenuSlot->Append(PU_TIMEOUT2, _T("Slot timeout 2"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _T("Slot timeout"), submenuSlot);

	wxMenu *submenuAnswr = new wxMenu;
    submenuAnswr->Append(PU_TIMEOUT1, _T("Answer timeout 1"));
    submenuAnswr->Append(PU_TIMEOUT2, _T("Answer timeout 2"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _T("Answer timeout"), submenuAnswr);

	wxMenu *submenuBaloon = new wxMenu;
    submenuBaloon->Append(PU_YES, _T("Yes"));
    submenuBaloon->Append(PU_NO, _T("No"));
	menu->Append(PU_ANSWER_TIMEOUT_SUB, _T("Show baloons"), submenuBaloon);
	menu->AppendSeparator();

	menu->Append(PU_POWER, _T("&On/Off"));    

    return menu;
}

void MyTaskBarIcon::OnLeftButtonDClick(wxTaskBarIconEvent&)
{
    // dialog->Show(true);
}

void MyTaskBarIcon::OnPower(wxCommandEvent&)
{
}