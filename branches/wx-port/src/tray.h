#pragma once

#include "wx/wx.h"
#include "wx/taskbar.h"

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
    virtual wxMenu *CreatePopupMenu();

DECLARE_EVENT_TABLE()
};


// Define a new application
class MyApp: public wxApp
{
public:
    bool OnInit(void);
};

class MyDialog: public wxDialog
{
public:
    MyDialog(wxWindow* parent, const wxWindowID id, const wxString& title,
        const wxPoint& pos, const wxSize& size, const long windowStyle = wxDEFAULT_DIALOG_STYLE);
    ~MyDialog();

    void OnOK(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnCloseWindow(wxCloseEvent& event);
    void Init(void);

protected:
    MyTaskBarIcon   *m_taskBarIcon;
#if defined(__WXCOCOA__)
    MyTaskBarIcon   *m_dockIcon;
#endif

DECLARE_EVENT_TABLE()
};
