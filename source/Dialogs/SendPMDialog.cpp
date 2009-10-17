#include "stdwx.h"
#include "SendPMDialog.h"
#include "..\wxFlybotDLL.h"

DECLARE_APP(wxFlybotDLL)

SendPMDialog::SendPMDialog( wxWindow* parent )
:
SendPMDialogBase( parent )
{

}

void SendPMDialog::OnCloseDialog( wxCloseEvent& event )
{
    event.Skip();
}

void SendPMDialog::OnInitDialog( wxInitDialogEvent& WXUNUSED(event) )
{
    wxArrayString nickAutoCompleteHistory = wxGetApp().Config.GetNickHistory();
    this->m_textNick->AutoComplete(nickAutoCompleteHistory);

}

void SendPMDialog::OnTextComboNick( wxCommandEvent& WXUNUSED(event) )
{
	// TODO: Implement OnTextComboNick
}

void SendPMDialog::OnSendClick( wxCommandEvent& WXUNUSED(event) )
{
	wxGetApp().AddDelayedPM(m_textNick->GetValue(), m_PMtext->GetValue());
    wxGetApp().Config.AddNickHistory(m_textNick->GetValue());
    this->Close();
}
