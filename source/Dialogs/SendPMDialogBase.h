///////////////////////////////////////////////////////////////////////////
// C++ code generated with wxFormBuilder (version Apr 16 2008)
// http://www.wxformbuilder.org/
//
// PLEASE DO "NOT" EDIT THIS FILE!
///////////////////////////////////////////////////////////////////////////

#ifndef __SendPMDialogBase__
#define __SendPMDialogBase__

#include <wx/string.h>
#include <wx/stattext.h>
#include <wx/gdicmn.h>
#include <wx/font.h>
#include <wx/colour.h>
#include <wx/settings.h>
#include <wx/textctrl.h>
#include <wx/combobox.h>
#include <wx/button.h>
#include <wx/sizer.h>
#include <wx/dialog.h>

///////////////////////////////////////////////////////////////////////////

#define wxID_PM_TEXT 1000
#define wxID_COMBO_NICK 1001
#define wxID_BUTTON_SEND 1002

///////////////////////////////////////////////////////////////////////////////
/// Class SendPMDialogBase
///////////////////////////////////////////////////////////////////////////////
class SendPMDialogBase : public wxDialog 
{
	private:
	
	protected:
		wxStaticText* m_staticPM;
		wxTextCtrl* m_PMtext;
		wxStaticText* m_staticNick;
		wxComboBox* m_comboNick;
		wxButton* m_buttonSend;
		
		// Virtual event handlers, overide them in your derived class
		virtual void OnCloseDialog( wxCloseEvent& event ){ event.Skip(); }
		virtual void OnInitDialog( wxInitDialogEvent& event ){ event.Skip(); }
		virtual void OnTextComboNick( wxCommandEvent& event ){ event.Skip(); }
		virtual void OnSendClick( wxCommandEvent& event ){ event.Skip(); }
		
	
	public:
		SendPMDialogBase( wxWindow* parent, wxWindowID id = wxID_ANY, const wxString& title = wxT("Send PM to offline user"), const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxSize( 400,120 ), long style = wxCLOSE_BOX|wxDEFAULT_DIALOG_STYLE|wxRESIZE_BORDER );
		~SendPMDialogBase();
	
};

#endif //__SendPMDialogBase__
